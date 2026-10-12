#include <Arduino.h>
#include <RCSwitch.h>

constexpr uint8_t PIN_RECEPTOR = 4;
RCSwitch receptor;

const char *nombreDelCodigo(unsigned long codigo) {
  switch (codigo) {
    case 54854456UL: return "OFF (apagar)";
    case 54855242UL: return "MAX (maximo)";
    case 54855765UL: return "MIN (minimo)";
    case 54856549UL: return "FAN (ventilador)";
    case 54854779UL: return "LIGHT (luz)";
    default: return "desconocido";
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  pinMode(PIN_RECEPTOR, INPUT);
  receptor.enableReceive(digitalPinToInterrupt(PIN_RECEPTOR));
  Serial.println("Receptor preparado en GPIO4. Pulsa un boton del mando.");
}

void loop() {
  if (!receptor.available()) {
    delay(1);
    return;
  }
  const unsigned long codigo = receptor.getReceivedValue();
  // Mostrar tambien parametros y codigos desconocidos permite adaptar otro mando.
  Serial.print(nombreDelCodigo(codigo));
  Serial.print(" | Codigo: ");
  Serial.print(codigo);
  Serial.print(" | Bits: ");
  Serial.print(receptor.getReceivedBitlength());
  Serial.print(" | Protocolo: ");
  Serial.print(receptor.getReceivedProtocol());
  Serial.print(" | Pulso: ");
  Serial.print(receptor.getReceivedDelay());
  Serial.println(" us");
  receptor.resetAvailable();
}
