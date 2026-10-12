<div align="center">

# Voice-Controlled Fan

**Say it. Send it. Keep your existing fan.**

Offline voice control for an RF fan and its light.

**100% vibe-coded with AI assistance.** A personal hardware project, not an independently audited product.

[**English**](README.md) · [Español](README.es.md)

![Controller](https://img.shields.io/badge/Controller-ESP32_DevKit_V1-087f8c)
![Radio](https://img.shields.io/badge/Radio-433_MHz-2563eb)
![Voice](https://img.shields.io/badge/Voice-Offline-16a34a)
![License](https://img.shields.io/badge/Own_code-CC0--1.0-52525b)

[Wiring](#wiring) · [Voice Setup](#voice-setup) · [RF Codes](#rf-codes) · [Test Workflow](#test-workflow)

</div>

---

```text
Your voice -> ASRPRO-01 -> UART -> ESP32 DevKit V1 -> 433 MHz TX -> fan
                                                        |
                                  Optional test RX -> ESP32-C3 Mini
```

The ASRPRO recognizes learned phrases locally. The main ESP32 receives short text messages and reproduces the fan remote's RF buttons. No Wi-Fi, cloud account or modification of the fan's mains wiring is required.

> **Not a universal remote.** The included codes belong to the project owner's remote. Capture your own codes before using another fan. The original fan receiver is still required; the C3 Mini is a separate diagnostic receiver, not a replacement for it.

## Hardware

| Part | Role |
| :--- | :--- |
| **ESP32 DevKit V1** | Main Arduino controller; classic dual-core ESP32 |
| **ASRPRO-01** | Offline speech recognition, microphone and UART output |
| **433 MHz RF transmitter** | Replays the remote's signal |
| **ESP32-C3 Mini** | Single-core board used for reception during tests only |
| **433 MHz RF receiver** | Connected to the C3 Mini to capture/check codes |
| USB data cables, jumper wires, suitable antennas | Power, programming and connections |

### Purchase Links

- [ASRPRO-01](https://es.aliexpress.com/item/1005012990097981.html).
- [433 MHz transmitter/receiver kit](https://es.aliexpress.com/item/1005003436580019.html).

Tracking parameters have been removed. Check selected variants, pin labels and voltage ratings; sellers can change their listings. The owner confirmed the purchased kit is **433 MHz** and its transmitter worked with **EN left unconnected**. This does not establish EN behavior for every similar-looking module. RX470C-V01 was the receiver marking discussed during development; do not identify the transmitter from the receiver's model name.

<details>
<summary><strong>ASRPRO-01 used in the project</strong></summary>

<p align="center"><img src="assets/asrpro-01.jpeg" alt="ASRPRO-01 with USB-C and onboard microphone" width="260"></p>

</details>

## Sketches

| File | Upload to | Purpose |
| :--- | :--- | :--- |
| [control-voz.ino](control-voz/control-voz.ino) | DevKit V1 | Main voice-to-RF controller |
| [monitor-voz.ino](monitor-voz/monitor-voz.ino) | DevKit V1 | UART-only diagnostic monitor |
| [prueba-emisor.ino](prueba-emisor/prueba-emisor.ino) | DevKit V1 | Manual RF test, independent of ASRPRO |
| [prueba-receptor.ino](prueba-receptor/prueba-receptor.ino) | C3 Mini | Capture codes, bit length, protocol and pulse length |

The three DevKit sketches are alternatives: upload one at a time. The C3 receiver can run simultaneously on its own USB connection. The main sketch requires two cores and deliberately rejects single-core targets such as the C3.

## Wiring

Disconnect power before wiring. Use the module's printed labels, not an assumed left-to-right pin order.

### Main Controller

| From | To |
| :--- | :--- |
| ASRPRO `PB5` (TX) | DevKit `GPIO27` (UART RX) |
| ASRPRO `GND` | DevKit `GND` |
| DevKit `GPIO25` | Transmitter `DAT` / `DATA` |
| DevKit `GND` | Transmitter `-` / `GND` |
| DevKit `3V3` | Transmitter `+` / VCC for the project's 3.3 V transmitter |
| Transmitter `EN` | **Unconnected in the owner's working setup** |

Power the ASRPRO and DevKit through their own USB ports for the first test. Leave ASRPRO `PB6` and `3V3` unconnected to the DevKit; do not join the boards' 5 V rails. PB6 is unnecessary for receive-only operation and another TX driver there can conflict with the onboard USB-serial bridge. UART logic is 3.3 V, **115200 baud, 8N1**.

### Diagnostic Receiver

| RF receiver | ESP32-C3 Mini |
| :--- | :--- |
| `DATA` | `GPIO4` |
| `GND` | `GND` |
| VCC | Supply permitted by the receiver's exact specification |

The C3 is powered from its own USB. Its wireless receiver does not need a wire to the main ESP32. GPIO numbers are chip GPIO numbers, not physical pin positions. Never apply 5 V to an ESP32 input; if your receiver needs 5 V and outputs 5 V DATA, use suitable level conversion. Check antenna connection, module ratings and local RF rules. Do not connect this project directly to mains or the fan motor.

## Voice Setup

Use [ASRPRO Studio](https://github.com/adriviciano/ASRPRO-Studio) to program and teach the ASRPRO. Its [Windows download](https://github.com/adriviciano/ASRPRO-Studio/releases/latest) manages the external Tianwen tools; this repository does not include their SDK, voice models or firmware binaries.

1. Connect the ASRPRO by USB and open Studio.
2. Import [commands.example.json](asrpro/commands.example.json), or enter the five commands below. Spoken phrases are customizable; keep the **output messages exactly uppercase**.
3. Compile and flash the ASRPRO firmware. This erases learned phrases: do this before recording.
4. Teach the activation phrase (for example, "Hola ventilador") and each command through the **ASRPRO microphone**, repeating when prompted. Importing JSON does not teach audio.
5. Check recognition in Studio, then use the ESP32 UART monitor and finally the main controller. If the ASRPRO is already configured correctly, do not flash it again just to change the ESP32 sketch.

| ID | Example spoken phrase | UART message | Replayed button |
| :--- | :--- | :--- | :--- |
| 1 | Ventilador | `FAN` | Fan button |
| 2 | Luz | `LIGHT` | Light button |
| 3 | Máximo | `MAX` | Maximum speed |
| 4 | Mínimo | `MIN` | Minimum speed |
| 5 | Apagar | `OFF` | Off |

The main sketch handles **messages**, not IDs: `FAN`, `LIGHT`, `MAX`, `MIN`, `OFF`. It ignores `CMD:<id>`, wake/startup and debug lines to avoid executing both the message and its ID. FAN/LIGHT reproduce the remote's button behavior; they may toggle state, not mean separate on/off commands. No fan state is inferred or tracked.

## RF Codes

| Message | Decimal RF code |
| :--- | :--- |
| `FAN` | `54856549` |
| `LIGHT` | `54854779` |
| `MAX` | `54855242` |
| `MIN` | `54855765` |
| `OFF` | `54854456` |

All five use **32 bits**, **rc-switch protocol 1**, **266 µs pulse length** and **6 repeated frames per recognition**. Captures varied around 266–268 µs. A code/protocol describes the pulse encoding; it does not select the radio carrier frequency. The hardware must match the 433 MHz remote; this is not ESP32 Wi-Fi or a 2.4 GHz transmitter.

### Why Six Frames?

The receiver printed two lines per original button press, but printed lines are not a physical frame counter. [rc-switch](https://github.com/sui77/rc-switch/blob/master/RCSwitch.cpp) uses repeated timing patterns to validate reception. The owner reported the isolated test decoded with six repeats, not with two. The main sketch calls `send()` once; `setRepeatTransmit(6)` performs the repetition. Do not call `send()` twice as well. Check FAN/LIGHT produce only one toggle in the actual fan.

### Adapt Another Remote

1. Upload the receiver sketch to the C3 and verify it sees your original remote.
2. Capture each button several times: decimal code, bits, protocol and pulse length. Check whether codes remain stable.
3. Replace the five codes in `procesarOrden()` in the main sketch; update protocol, bit length and pulse length if required.
4. Update the manual transmitter test's MAX code too, and test it before involving voice.
5. Confirm the real fan response. Unsupported encoding or rolling codes may not work with this approach. Use only devices you own or are authorized to control.

## Compile and Upload

Install **esp32 by Espressif Systems** in Arduino IDE Boards Manager and **rc-switch by sui77** in Library Manager. No dependency source is vendored here.

- DevKit V1: select **ESP32 Dev Module**, then open the desired `.ino` in its matching folder.
- C3 Mini receiver: select **ESP32C3 Dev Module**. For native USB, enable **USB CDC On Boot**; USB-UART variants may need different settings.
- Select the correct board's port, verify, upload and open Serial Monitor at **115200**. Never upload the dual-core main program to the C3.
- If a DevKit upload stalls at "Connecting", hold BOOT, briefly press/release EN (reset), then release BOOT when writing begins. Board auto-reset behavior varies.

Optional Windows compile-only check with the installed Arduino CLI:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check-sketches.ps1
```

Use `-ArduinoCli "path\to\arduino-cli.exe"` if necessary. This script does not upload firmware. Build validation and hardware validation are different; a successful compile does not prove RF reception or safe operation.

All four sketches were compiled successfully with Arduino-ESP32 **3.3.11** and rc-switch **2.6.4**. No firmware was uploaded during this check. The six-repeat RF result above comes from the owner's receiver test, not a new end-to-end hardware test.

## Test Workflow

1. **Remote -> C3:** check capture first; it establishes receiver wiring and the remote's codes.
2. **DevKit -> C3:** use the manual transmitter sketch. Send `2` or `6` through Serial Monitor; it transmits MAX once with that number of repeats and never transmits at startup. **This can start the fan at maximum speed.**
3. **ASRPRO -> DevKit:** use the UART-only monitor. Check exact recognized messages.
4. **Full chain:** upload the main sketch. `[VOZ]` confirms a recognized message was queued; `[RF]` confirms software sent pulses, not that the fan received them.

The main program keeps voice reception on **core 1** and RF sending on **core 0**, with a 4096-byte UART buffer and a FIFO of 16 commands. Queue overflow is explicitly logged. This prevents the receive task from waiting on RF `send()`, but cannot fix power, antenna or wiring problems. Repeated spoken commands are not deduplicated; each new recognized message queues an action.

| Symptom | Check |
| :--- | :--- |
| Nothing in UART monitor | PB5 -> GPIO27, common ground, baud rate, learned phrases |
| UART lines but no `[VOZ]` | Exact uppercase message; set `MOSTRAR_UART = true` |
| `[RF]` but no receiver output | DAT, voltage, antenna, frequency, EN variant; logs are not RF confirmation |
| Works without RF, loses voice with RF | Set `TRANSMITIR_RADIO = false`; inspect supply/wiring/interference |
| FAN/LIGHT toggles unexpectedly | Message/ID double handling or remote repeat behavior |
| Main sketch fails on C3 | Use the dual-core DevKit V1; C3 is the test receiver |

## License

Original sketches and documentation: [CC0 1.0 Universal](LICENSE). Third-party components keep their own terms: [notices](THIRD-PARTY-NOTICES.md). No warranty or universal fan compatibility is promised. This is independent of the fan, board and module manufacturers.
