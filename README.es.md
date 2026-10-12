<div align="center">

# Voice-Controlled Fan

**Tu voz. Tu ventilador. Sin cambiar el aparato.**

Control por voz sin conexión para un ventilador RF y su luz.

**Proyecto 100% vibecodeado con ayuda de IA.** Un proyecto personal de hardware, no un producto auditado de forma independiente.

[English](README.md) · [**Español**](README.es.md)

![Controlador](https://img.shields.io/badge/Controlador-ESP32_DevKit_V1-087f8c)
![Radio](https://img.shields.io/badge/Radio-433_MHz-2563eb)
![Voz](https://img.shields.io/badge/Voz-Sin_conexi%C3%B3n-16a34a)
![Licencia](https://img.shields.io/badge/Codigo_propio-CC0--1.0-52525b)

[Conexiones](#conexiones) · [Configurar la Voz](#configurar-la-voz) · [Códigos RF](#códigos-rf) · [Pruebas](#pruebas)

</div>

---

```text
Tu voz -> ASRPRO-01 -> UART -> ESP32 DevKit V1 -> Emisor 433 MHz -> ventilador
                                                        |
                                  Receptor de pruebas -> ESP32-C3 Mini
```

La ASRPRO reconoce las frases aprendidas localmente. El ESP32 recibe mensajes cortos y reproduce los botones RF del mando. No requiere Wi-Fi, cuenta en la nube ni modificar el cableado eléctrico del ventilador.

> **No es un mando universal.** Los códigos incluidos pertenecen al mando del propietario del proyecto. Captura los tuyos antes de utilizar otro ventilador. Se conserva el receptor original del aparato; la C3 Mini es un receptor de diagnóstico independiente, no lo sustituye.

## Materiales

| Componente | Función |
| :--- | :--- |
| **ESP32 DevKit V1** | Controlador Arduino principal; ESP32 clásico de dos núcleos |
| **ASRPRO-01** | Reconocimiento de voz sin conexión, micrófono y salida UART |
| **Emisor RF de 433 MHz** | Reproduce la señal del mando |
| **ESP32-C3 Mini** | Placa de un núcleo utilizada solo como receptor durante las pruebas |
| **Receptor RF de 433 MHz** | Conectado a la C3 para capturar y comprobar códigos |
| Cables USB de datos, puentes y antenas apropiadas | Alimentación, programación y conexiones |

### Enlaces de Compra

- [ASRPRO-01](https://es.aliexpress.com/item/1005012990097981.html).
- [Kit emisor/receptor de 433 MHz comprado para este proyecto](https://es.aliexpress.com/item/1005003436580019.html).

Se han eliminado parámetros de seguimiento. Comprueba variantes, serigrafía y tensiones: los vendedores pueden cambiar sus anuncios. El propietario confirma que su kit es de **433 MHz** y funcionó con **EN sin conectar** en el emisor. No implica que todos los módulos parecidos se comporten igual. RX470C-V01 era la referencia del receptor comentada durante el desarrollo; no identifiques el emisor por el modelo del receptor.

<details>
<summary><strong>ASRPRO-01 del proyecto</strong></summary>

<p align="center"><img src="assets/asrpro-01.jpeg" alt="ASRPRO-01 con USB-C y micrófono integrado" width="260"></p>

</details>

## Sketches

| Archivo | Placa de destino | Función |
| :--- | :--- | :--- |
| [control-voz.ino](control-voz/control-voz.ino) | DevKit V1 | Controlador principal de voz a RF |
| [monitor-voz.ino](monitor-voz/monitor-voz.ino) | DevKit V1 | Monitor de diagnóstico solo UART |
| [prueba-emisor.ino](prueba-emisor/prueba-emisor.ino) | DevKit V1 | Prueba RF manual, independiente de ASRPRO |
| [prueba-receptor.ino](prueba-receptor/prueba-receptor.ino) | C3 Mini | Captura código, bits, protocolo y duración del pulso |

Los tres sketches de la DevKit son alternativas: se carga uno cada vez. El receptor de la C3 puede funcionar simultáneamente por su propio USB. El programa principal requiere dos núcleos y rechaza intencionadamente placas de un núcleo como la C3.

## Conexiones

Desconecta la alimentación antes de cablear. Utiliza los nombres impresos en cada módulo; no supongas el orden de los pines de izquierda a derecha.

### Controlador Principal

| Origen | Destino |
| :--- | :--- |
| ASRPRO `PB5` (TX) | DevKit `GPIO27` (RX UART) |
| ASRPRO `GND` | DevKit `GND` |
| DevKit `GPIO25` | `DAT` / `DATA` del emisor |
| DevKit `GND` | `-` / `GND` del emisor |
| DevKit `3V3` | `+` / VCC del emisor de 3,3 V del proyecto |
| `EN` del emisor | **Sin conectar en el montaje que funcionó al propietario** |

Alimenta ASRPRO y DevKit por sus propios USB en la primera prueba. Deja `PB6` y `3V3` de ASRPRO sin conectar a la DevKit; no unas las líneas de 5 V de ambas placas. PB6 no hace falta para recibir mensajes y otro transmisor allí puede interferir con el puente USB-serie integrado. UART utiliza lógica de 3,3 V y **115200 baudios, 8N1**.

### Receptor de Diagnóstico

| Receptor RF | ESP32-C3 Mini |
| :--- | :--- |
| `DATA` | `GPIO4` |
| `GND` | `GND` |
| VCC | Alimentación permitida por la ficha del receptor exacto |

La C3 se alimenta por su propio USB. Su receptor inalámbrico no necesita cables al ESP32 principal. Los números son GPIO del chip, no posiciones físicas. Nunca apliques 5 V a una entrada ESP32; si el receptor necesita 5 V y entrega DATA de 5 V, utiliza adaptación de nivel apropiada. Comprueba antenas, especificaciones y normas RF locales. No conectes este proyecto directamente a la red eléctrica ni al motor.

## Configurar la Voz

Utiliza [ASRPRO Studio](https://github.com/adriviciano/ASRPRO-Studio) para programar y enseñar las frases a la ASRPRO. Su [descarga para Windows](https://github.com/adriviciano/ASRPRO-Studio/releases/latest) gestiona las herramientas externas Tianwen; este repositorio no incluye su SDK, modelos de voz ni firmware binario.

1. Conecta ASRPRO por USB y abre Studio.
2. Importa [commands.example.json](asrpro/commands.example.json) o introduce los cinco comandos de abajo. Puedes cambiar las frases habladas, pero conserva los **mensajes de salida exactamente en mayúsculas**.
3. Compila y carga el firmware ASRPRO. Esto borra las frases aprendidas: hazlo antes de grabar.
4. Graba la activación (por ejemplo, «Hola ventilador») y cada comando con el **micrófono de ASRPRO**, repitiendo cuando lo indique. Importar el JSON no enseña el audio.
5. Comprueba el reconocimiento en Studio, después en el monitor UART y finalmente con el controlador principal. Si ASRPRO ya está bien configurada, no vuelvas a cargarla solo por cambiar el sketch del ESP32.

| ID | Frase de ejemplo | Mensaje UART | Botón reproducido |
| :--- | :--- | :--- | :--- |
| 1 | Ventilador | `FAN` | Ventilador |
| 2 | Luz | `LIGHT` | Luz |
| 3 | Máximo | `MAX` | Velocidad máxima |
| 4 | Mínimo | `MIN` | Velocidad mínima |
| 5 | Apagar | `OFF` | Apagar |

El programa principal interpreta **mensajes**, no IDs: `FAN`, `LIGHT`, `MAX`, `MIN`, `OFF`. Ignora `CMD:<id>`, activación, arranque y depuración para no ejecutar tanto el mensaje como su ID. FAN/LIGHT reproducen el comportamiento de los botones: pueden alternar el estado, no son órdenes separadas de encender/apagar. No se deduce ni mantiene el estado del ventilador.

## Códigos RF

| Mensaje | Código RF decimal |
| :--- | :--- |
| `FAN` | `54856549` |
| `LIGHT` | `54854779` |
| `MAX` | `54855242` |
| `MIN` | `54855765` |
| `OFF` | `54854456` |

Los cinco utilizan **32 bits**, **protocolo 1 de rc-switch**, **pulso de 266 µs** y **6 tramas repetidas por reconocimiento**. Las capturas variaban entre 266 y 268 µs. Código y protocolo describen la codificación de los pulsos; no seleccionan la frecuencia portadora. El hardware debe coincidir con el mando de 433 MHz; no es Wi-Fi del ESP32 ni un emisor de 2,4 GHz.

### Por Qué Seis Tramas

El receptor imprimía dos líneas por pulsación del mando, pero las líneas no cuentan las tramas físicas. [rc-switch](https://github.com/sui77/rc-switch/blob/master/RCSwitch.cpp) utiliza patrones de tiempos repetidos para validar la recepción. El propietario confirmó que la prueba aislada se recibía con seis repeticiones, no con dos. El sketch llama a `send()` una sola vez; `setRepeatTransmit(6)` repite las tramas. No añadas otra llamada a `send()`. Comprueba que FAN/LIGHT solo alternen una vez en el aparato real.

### Adaptar Otro Mando

1. Sube el receptor a la C3 y comprueba que detecta tu mando original.
2. Captura cada botón varias veces: código decimal, bits, protocolo y pulso. Comprueba si los códigos son estables.
3. Sustituye los cinco códigos de `procesarOrden()` en el programa principal; ajusta protocolo, bits y pulso si hace falta.
4. Cambia también MAX en la prueba manual del emisor y pruébalo antes de involucrar la voz.
5. Comprueba la respuesta del ventilador. Codificaciones no soportadas o códigos variables pueden no funcionar con este método. Controla solo dispositivos propios o con autorización.

## Compilar y Cargar

Instala **esp32 de Espressif Systems** desde el gestor de placas de Arduino IDE y **rc-switch de sui77** desde el gestor de bibliotecas. No se incluyen sus fuentes en este repositorio.

- DevKit V1: selecciona **ESP32 Dev Module** y abre el `.ino` deseado en su carpeta correspondiente.
- Receptor C3 Mini: selecciona **ESP32C3 Dev Module**. Para USB nativo, activa **USB CDC On Boot**; variantes con adaptador USB-UART pueden necesitar otra configuración.
- Selecciona el puerto de la placa correcta, verifica, carga y abre el Monitor Serie a **115200**. No subas el programa de dos núcleos a la C3.
- Si la carga de la DevKit se queda en «Connecting», mantén BOOT, pulsa y suelta brevemente EN (reset), y suelta BOOT cuando empiece a escribir. El reinicio automático depende de la placa.

Comprobación opcional de compilación en Windows con Arduino CLI instalado:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check-sketches.ps1
```

Puedes añadir `-ArduinoCli "ruta\a\arduino-cli.exe"`. El script no carga firmware. Validar la compilación y validar el hardware son cosas distintas: compilar no demuestra que se reciba RF ni que el montaje sea seguro.

## Pruebas

1. **Mando -> C3:** comprueba primero la captura; verifica receptor y códigos del mando.
2. **DevKit -> C3:** utiliza la prueba manual del emisor. Envía `2` o `6` desde el Monitor Serie; emite MAX una vez con esas repeticiones y nunca transmite al arrancar. **Puede activar el ventilador a máxima velocidad.**
3. **ASRPRO -> DevKit:** utiliza el monitor solo UART y comprueba los mensajes exactos.
4. **Cadena completa:** sube el programa principal. `[VOZ]` confirma que el mensaje reconocido se encoló; `[RF]` confirma pulsos enviados por software, no que el ventilador los haya recibido.

La recepción de voz va en **core 1** y el envío RF en **core 0**, con un buffer UART de 4096 bytes y cola FIFO de 16 órdenes. El desbordamiento se informa explícitamente. Esto evita que la recepción espere a `send()`, pero no corrige alimentación, antenas ni cableado. No se deduplican comandos hablados repetidos: cada nuevo mensaje reconocido encola una acción.

| Síntoma | Revisar |
| :--- | :--- |
| Nada en el monitor UART | PB5 -> GPIO27, masa común, baudios, frases aprendidas |
| Líneas UART pero no `[VOZ]` | Mensaje exacto en mayúsculas; activa `MOSTRAR_UART = true` |
| `[RF]` pero nada en el receptor | DAT, tensión, antena, frecuencia y variante de EN; el log no confirma RF |
| Funciona sin RF, pierde voz con RF | Prueba `TRANSMITIR_RADIO = false`; revisa alimentación/cableado/interferencias |
| FAN/LIGHT alterna inesperadamente | Interpretación doble de mensaje/ID o repeticiones del mando |
| El principal falla en C3 | Utiliza la DevKit V1 de dos núcleos; la C3 es el receptor de pruebas |

Los cuatro sketches se han compilado correctamente con Arduino-ESP32 **3.3.11** y rc-switch **2.6.4**. Esta comprobacion no ha cargado firmware en ninguna placa. El resultado RF con seis repeticiones corresponde a la prueba del propietario con el receptor, no a una nueva prueba fisica de toda la cadena.

## Licencia

Sketches y documentación propios: [CC0 1.0 Universal](LICENSE). Los componentes de terceros conservan sus condiciones: [avisos](THIRD-PARTY-NOTICES.md). Sin garantía ni promesa de compatibilidad universal. Proyecto independiente de los fabricantes del ventilador, placas y módulos.
