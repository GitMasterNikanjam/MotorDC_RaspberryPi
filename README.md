# MotorDC_RaspberryPi Library

The **MotorDC** library provides a simple C++ interface for controlling DC motors on a Raspberry Pi using the **bcm2835** library.
It supports PWM (Pulse Width Modulation) for speed control and a GPIO pin for direction control.

---

## 📦 Features

* Control **speed** via PWM duty cycle `[-100%, +100%]`.
* Control **direction** via a dedicated GPIO pin.
* Configurable **PWM channel (0 or 1), PWM range, clock divider, direction polarity, and offset duty**.
* **Safe cleanup** (`clean()`) to release pins and stop the motor.
* Read back **current duty, PWM value, direction**, and (optionally) your own **current (amp)** reading.

---

## 📂 File Structure

* `MotorDC.h` → Class definition & API
* `MotorDC.cpp` → Implementation
* `SimpleTest.cpp` → Example usage

---

## ⚙️ Dependencies

Requires the [bcm2835 library](http://www.airspayce.com/mikem/bcm2835/).

Install on Raspberry Pi (if available via package manager), or build from source per the link above.

---

## 🚀 Build & Run

```bash
mkdir -p ./bin
g++ -o ./bin/SimpleTest SimpleTest.cpp MotorDC.cpp -lbcm2835
sudo ./bin/SimpleTest
```

> Root privileges are required because bcm2835 accesses hardware registers.

---

## 🖥️ Example (from `SimpleTest.cpp`)

```cpp
#include <iostream>
#include "MotorDC.h"

MotorDC motorAZ;
MotorDC motorEL;

int main(void)
{
    if (!bcm2835_init()) return -1;

    motorAZ.parameters.CHANNEL_NUM   = 0; // PWM0 → GPIO12
    motorAZ.parameters.CLOCK_DEVIDER = BCM2835_PWM_CLOCK_DIVIDER_32;
    motorAZ.parameters.DIR_PIN       = 1; // Example GPIO
    motorAZ.parameters.DIR_POL       = 0; // 0: LOW=forward for +duty
    motorAZ.parameters.OFFSET_DUTY   = 4; // Deadzone [%]
    motorAZ.parameters.RANGE         = 24000;

    motorEL.parameters = motorAZ.parameters;
    motorEL.parameters.DIR_PIN   = 19;    // Another GPIO for direction
    motorEL.parameters.CHANNEL_NUM = 1;   // PWM1 → GPIO13

    if (!motorAZ.init() || !motorEL.init()) {
        std::cout << "Error: MotorDC init failed.\n";
        return -1;
    }

    std::cout << "Initialization OK!\n";

    float duty;
    while (true) {
        std::cout << "Enter duty [-100..100 %]: ";
        std::cin >> duty;
        motorEL.setDutyCycle(duty);
        std::cout << "Applied duty: " << motorEL.value.duty << "%\n";
    }

    bcm2835_close();
    return 0;
}
```

---

## 📖 API Overview

### `MotorDC::ParametersStructure`

| Name            | Type       | Description                                             |
| --------------- | ---------- | ------------------------------------------------------- |
| `RANGE`         | `uint32_t` | PWM range (affects frequency along with clock divider). |
| `DIR_PIN`       | `int8_t`   | GPIO pin used for direction control.                    |
| `DIR_POL`       | `uint8_t`  | Direction polarity (0=normal, 1=reverse).               |
| `CHANNEL_NUM`   | `int8_t`   | PWM channel (0→GPIO12, 1→GPIO13).                       |
| `CLOCK_DEVIDER` | `uint32_t` | PWM clock divider (use bcm2835 enum values).            |
| `OFFSET_DUTY`   | `float`    | Deadzone/offset duty \[%] applied to non-zero speeds.   |

> PWM frequency: `BASE_CPU_CLOCK / (CLOCK_DIVIDER * RANGE)`

### `MotorDC::ValuesStructure`

| Name   | Type       | Meaning                                                                  |
| ------ | ---------- | ------------------------------------------------------------------------ |
| `amp`  | `float`    | Motor current (mA). You set/update this externally if you have a sensor. |
| `pwm`  | `uint32_t` | Actual PWM register value (0..`RANGE`).                                  |
| `duty` | `float`    | Applied duty cycle \[%].                                                 |
| `dir`  | `int8_t`   | Direction (1=forward, -1=reverse, 0=stopped).                            |

### Methods

* `MotorDC()` / `~MotorDC()`
* `bool init()` – validates parameters, sets up GPIO/PWM.
* `void setDutyCycle(float duty)` – `[-100..100]` %, applies direction + speed.
* `void clean()` – stop and release pins.

---

## 🔌 Hardware Connections (Wiring)

This library outputs:

* **PWM** on either **GPIO12 (PWM0)** or **GPIO13 (PWM1)** depending on `CHANNEL_NUM`.
* **DIR** on a user-chosen GPIO (set `DIR_PIN`).

Your motor driver must accept **PWM + DIR/PH** style inputs (e.g., EN/PH, PWM/IN, or mapped to IN1/IN2 on an H-bridge).
If your driver uses **IN1/IN2**, drive one pin with PWM and hold the other as a logic level for direction.

### Pin Map (Raspberry Pi)

| Function   | RPi Pin                     | Header Name                             |
| ---------- | --------------------------- | --------------------------------------- |
| PWM0       | GPIO12                      | Pin 32                                  |
| PWM1       | GPIO13                      | Pin 33                                  |
| DIR        | (your choice, e.g., GPIO19) | (Pin varies)                            |
| GND        | Any GND                     | Pin 6, 9, 14, …                         |
| 3.3V Logic | —                           | Pi GPIOs are **3.3V** (not 5V tolerant) |

> **Important:** Share **GND** between the Raspberry Pi and your motor driver.
> If your driver expects **5V logic**, use a **level shifter** or pick a driver that accepts 3.3V logic.

### Mermaid Diagram (PWM + DIR to Driver)

```mermaid
flowchart LR
    subgraph RPi [Raspberry Pi]
        PWM0[GPIO12 (PWM0) or GPIO13 (PWM1)]
        DIR[GPIO (DIR_PIN)]
        GND_PI[GND]
    end

    subgraph Driver [Motor Driver (PWM+DIR type)]
        EN[EN/PWM Input]
        PH[DIR/PH Input]
        GND_DRV[GND]
        OUTA[OUT A]
        OUTB[OUT B]
    end

    subgraph Motor [DC Motor]
        M1((+))
        M2((-))
    end

    PWM0 --> EN
    DIR --> PH
    GND_PI --- GND_DRV
    OUTA --- M1
    OUTB --- M2
```

### ASCII Diagram (Fallback)

```
   Raspberry Pi                         Motor Driver (PWM+DIR)                DC Motor
  -----------------                    -------------------------            -------------
  GPIO12/13 (PWM)  ------------------>  EN / PWM Input
  GPIOx (DIR_PIN)  ------------------>  DIR / PH Input
  GND               ------------------- GND
                                         OUTA  ----------------------------> Motor +
                                         OUTB  ----------------------------> Motor -
```

### Example Wiring Table

| Signal | From (RPi)                  | To (Driver) | Notes                                       |
| -----: | --------------------------- | ----------- | ------------------------------------------- |
|    PWM | GPIO12 (CH0) / GPIO13 (CH1) | EN/PWM      | Pick the channel you configure in code.     |
|    DIR | `DIR_PIN` (e.g., GPIO19)    | DIR/PH      | `DIR_POL` defines “forward” level.          |
|    GND | GND                         | GND         | **Must** be common.                         |
|   VMOT | External Supply             | Driver VMOT | Separate motor supply, sized for motor.     |
|  +3.3V | —                           | —           | GPIO logic is 3.3V (don’t feed 5V into Pi). |

---

## 🧠 Tuning Tips

* **Deadzone (OFFSET\_DUTY):** If the motor doesn’t spin at small non-zero duty, increase `OFFSET_DUTY` (e.g., 3–10%).
* **PWM Frequency:** Use a **higher clock** and **lower range** (or vice versa) to tune frequency for your driver.
  Formula: `f_pwm = BASE_CPU_CLOCK / (CLOCK_DIVIDER * RANGE)`.
* **Direction Polarity:** If forward/reverse feel swapped, toggle `DIR_POL` (0 ↔ 1).
* **Safety:** Always start with the motor **elevated/off-load**, then ramp speed slowly.

---

## ⚠️ Notes & Limitations

* Requires **sudo** to run due to bcm2835 hardware access.
* Library currently supports **PWM channel 0 (GPIO12)** and **1 (GPIO13)**.
* `amp` field is **not** measured automatically—populate from your own current sensor if needed.

---

## 📜 License

MIT

---


