#  Remote E-ink display
Remote E-Ink Display based on STM32L053 and BLE112

## About
This is a simple project for transferring images via BLE to an E-Ink display.

### Hardware
1. [STM32L0538 DISCOVERY](https://www.st.com/en/evaluation-tools/32l0538discovery.html)  
2. [BLE112](https://www.silabs.com/wireless/bluetooth/bluegiga-low-energy-legacy-modules/device.ble112)  

> You can find the electric circuit in the KiCAD format in the section **/hardware**

### Software

There are two parts:
1. **/firmware** - code for STM32L0538 DISCOVERY
2. **/ble112** - code for BLE112

### Scripts
In this section you can find a script for testing using a PC.

> **Requirements:**
> 1. PC with Bluetooth and Linux  
> 2. Python 3 and [pygatt](https://github.com/peplin/pygatt)

## Usage
For build ble112 part:
1. Download [Bluetooth Smart Software and SDK v.1.3.2](https://www.silabs.com/wireless/bluetooth/bluegiga-low-energy-legacy-modules/device.ble112)
2. Build project with **bgbuild.exe** 
```
> bgbuild.exe path/to/remote_e-ink_display/ble/project-ble112.bgproj
```
3. Programming the BLE112
> You may programming with **CC Debuger** or [CCLib](https://github.com/wavesoft/CCLib) for Arduino

For build STM32 part:
1. Download and install **cmake**, **gcc-arm-none-eabi** and **stlink** on your system
2. Generate a project for GNU-make or another system supporting CMake
```
> mkdir path/to/remote_e-ink_display/firmware/build
> cd path/to/remote_e-ink_display/firmware/build
> cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain.cmake ..
```
3. Build firmware
```
> cmake --build .
```
3. Flashing
```
> st-flash write build/remote_e-ink_display.bin 0x8000000
```

## Protocol
Image is transmitted in 172 packets of 9 bytes. Timeouts are used for synchronization. If a packet of the required length has not arrived during RX_TIMEOUT, then the reception is reset to its initial state.

| PC or Smartphone | direction | STM32L053-Discovery  |
| ---------------- |:---------:| :--------------------|
| CMD_START | --> |   |
|   | <-- | i=0 |
| 9 bytes | --> |   |
|   | <-- | i=1 |
| 9 bytes | --> |   |
|   | <-- | i=2 |
|   | ... |   |
|   | <-- | i=171 |
| 9 bytes | --> |   |
|   | <-- | CRC |
| CMD_END or CMD_ERROR | --> |   |
