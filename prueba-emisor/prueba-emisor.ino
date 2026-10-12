#include <Arduino.h>
#include <RCSwitch.h>

constexpr int PIN_DAT = 25;
constexpr unsigned long CODIGO_MAXIMO = 54855242UL;
RCSwitch emisor;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_DAT, OUTPUT);
  digitalWrite(PIN_DAT, LOW);
  emisor.enableTransmit(PIN_DAT);
  emisor.setProtocol(1);
  emisor.setPulseLength(266);
  delay(500);
  Serial.println("Prueba RF independiente del ASRPRO.");
  Serial.println("Envia 2 o 6 por el monitor serie para emitir MAX con esas repeticiones.");
  Serial.println("Atencion: la prueba puede poner el ventilador al maximo.");
}

void loop() {
  if (!Serial.available()) {
    delay(1);
    return;
  }
  const int tecla = Serial.read();
  if (tecla != '2' && tecla != '6') return;
  const int repeticiones = tecla - '0';
  emisor.setRepeatTransmit(repeticiones);
  Serial.printf("Enviando %lu | 32 bits | protocolo 1 | 266 us | %d tramas\n",
                CODIGO_MAXIMO, repeticiones);
  emisor.send(CODIGO_MAXIMO, 32);
  Serial.println("Pulsos DAT terminados. Comprueba el receptor: esto no confirma RF.");
}
