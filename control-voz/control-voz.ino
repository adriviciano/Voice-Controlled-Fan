#include <Arduino.h>
#include <HardwareSerial.h>
#include <RCSwitch.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#if CONFIG_FREERTOS_UNICORE
#error "Este sketch requiere un ESP32 de dos nucleos."
#endif

constexpr int PIN_RX_VOZ = 27;
constexpr int PIN_TX_RADIO = 25;
constexpr unsigned int BITS_RADIO = 32;
constexpr int PULSO_RADIO_US = 266;
constexpr int REPETICIONES_RADIO = 6;
constexpr unsigned long BAUDIOS = 115200;
constexpr bool TRANSMITIR_RADIO = true;
constexpr bool MOSTRAR_UART = false;
HardwareSerial voz(2);
RCSwitch radio;
QueueHandle_t colaOrdenes = nullptr;

enum class Orden : uint8_t {
  Ninguna = 0,
  Ventilador = 1,
  Luz = 2,
  Maximo = 3,
  Minimo = 4,
  Apagar = 5
};

Orden reconocerOrden(const char *mensaje) {
  if (strcmp(mensaje, "FAN") == 0) return Orden::Ventilador;
  if (strcmp(mensaje, "LIGHT") == 0) return Orden::Luz;
  if (strcmp(mensaje, "MAX") == 0) return Orden::Maximo;
  if (strcmp(mensaje, "MIN") == 0) return Orden::Minimo;
  if (strcmp(mensaje, "OFF") == 0) return Orden::Apagar;
  return Orden::Ninguna;
}

void procesarOrden(Orden orden) {
  const char *nombre = nullptr;
  unsigned long codigo = 0;
  switch (orden) {
    case Orden::Ventilador: nombre = "FAN (ventilador)"; codigo = 54856549UL; break;
    case Orden::Luz: nombre = "LIGHT (luz)"; codigo = 54854779UL; break;
    case Orden::Maximo: nombre = "MAX (maximo)"; codigo = 54855242UL; break;
    case Orden::Minimo: nombre = "MIN (minimo)"; codigo = 54855765UL; break;
    case Orden::Apagar: nombre = "OFF (apagar)"; codigo = 54854456UL; break;
    default: return;
  }
  // send ya repite la trama: no llamar dos veces ni procesar tambien CMD:<id>.
  if (TRANSMITIR_RADIO) radio.send(codigo, BITS_RADIO);
  Serial.printf("[RF] %s | Codigo: %lu | Tramas: %d | %s\n", nombre, codigo,
                REPETICIONES_RADIO, TRANSMITIR_RADIO ? "enviadas" : "radio desactivada");
}

void tareaRadio(void *) {
  // Solo esta tarea utiliza RCSwitch; la UART nunca espera a radio.send().
  radio.enableTransmit(PIN_TX_RADIO);
  radio.setProtocol(1);
  radio.setPulseLength(PULSO_RADIO_US);
  radio.setRepeatTransmit(REPETICIONES_RADIO);
  Serial.printf("[RF] Emisor listo en core %d\n", xPortGetCoreID());
  Orden orden;
  for (;;) {
    if (xQueueReceive(colaOrdenes, &orden, portMAX_DELAY) == pdTRUE) {
      procesarOrden(orden);
      vTaskDelay(1);
    }
  }
}

void recibirVoz() {
  static char linea[192];
  static size_t longitud = 0;
  static bool descartar = false;

  while (voz.available() > 0) {
    const int dato = voz.read();
    if (dato < 0) break;
    const char caracter = static_cast<char>(dato);
    if (caracter == '\n') {
      if (!descartar) {
        linea[longitud] = '\0';
        if (MOSTRAR_UART) Serial.printf("[UART] %s\n", linea);
        const Orden orden = reconocerOrden(linea);
        if (orden != Orden::Ninguna) {
          if (xQueueSend(colaOrdenes, &orden, 0) == pdTRUE) {
            Serial.printf("[VOZ] ID: %u | en cola\n", static_cast<unsigned int>(orden));
          } else {
            Serial.println("[ERROR] Cola RF llena: orden descartada.");
          }
        }
      } else {
        Serial.println("[UART] Linea invalida o demasiado larga descartada.");
      }
      longitud = 0;
      descartar = false;
    } else if (caracter != '\r' && !descartar) {
      // Una linea larga se descarta entera para no interpretar su final como una orden.
      if (caracter == '\0' || longitud >= sizeof(linea) - 1) descartar = true;
      else linea[longitud++] = caracter;
    }
  }
}

void tareaVoz(void *) {
  voz.setRxBufferSize(4096);
  voz.begin(BAUDIOS, SERIAL_8N1, PIN_RX_VOZ, -1);
  Serial.printf("[VOZ] UART lista en core %d\n", xPortGetCoreID());
  for (;;) {
    recibirVoz();
    vTaskDelay(1);
  }
}

void setup() {
  Serial.begin(BAUDIOS);
  delay(500);
  colaOrdenes = xQueueCreate(16, sizeof(Orden));
  if (colaOrdenes == nullptr) {
    Serial.println("[ERROR] No se pudo crear la cola RF.");
    for (;;) delay(1000);
  }
  // La asignacion fija mantiene recepcion y transmision en nucleos distintos.
  if (xTaskCreatePinnedToCore(tareaRadio, "radio", 4096, nullptr, 1, nullptr,
                            0) != pdPASS) {
    Serial.println("[ERROR] No se pudo crear la tarea RF.");
    for (;;) delay(1000);
  }
  if (xTaskCreatePinnedToCore(tareaVoz, "voz", 4096, nullptr, 2, nullptr,
                            1) != pdPASS) {
    Serial.println("[ERROR] No se pudo crear la tarea de voz.");
    for (;;) delay(1000);
  }
  Serial.println("Control listo: voz core 1, radio core 0. FAN, LIGHT, MAX, MIN, OFF.");
}

void loop() {
  delay(1000);
}
