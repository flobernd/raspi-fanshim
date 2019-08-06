# Raspberry Pi Fan SHIM library

Provides functions to control the pimoroni `Fan SHIM` device on a Raspberry Pi.

## Features

### Fan control

Basic functions to enable/disable the fan or query its state.

```c
/**
 * @brief   Queries the current status of the fan.
 * 
 * @return  `True`, if the fan is currently running or `false`, if not.
 */
bool RaspiFanshimIsFanEnabled(void);

/**
 * @brief   Enables or disables the fan.
 * 
 * @param   enabled Pass `true` to enable the fan, `false` to disable it.
 */
void RaspiFanshimEnableFan(bool enabled);

/**
 * @brief   Toggles the fan.
 */
void RaspiFanshimToggleFan(void);
```

### LED control

Allows setting the color and brightness value of the integrated `APA102` LED.

```c
/**
 * @brief   Updates the color of the `Fan SHIM`s integrated `APA102` LED.
 * 
 * @param   r           The red color component.
 * @param   g           The green color component.
 * @param   b           The blue color component.
 * @param   brightness  The LED brightness (0..31).
 */
void RaspiFanshimUpdateLED(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
```

## Build

Install the required `wiringPi` library.

```bash
sudo apt install wiringpi
```

You can use CMake to generate project files for your favorite C99 compiler.

```bash
git clone --recursive 'https://github.com/flobernd/raspi-fanshim.git'
cd raspi-fanshim
mkdir build && cd build
cmake ..
make
```

## Service

The `RaspiFanshim` projects contains an [example service](./examples/FanshimService.c) that can be used as a replacement for the original python based Fan SHIM system service. It uses less CPU resources and provides more configuration options (in the source file).

This project is work in progress.

## License

RaspiFanshim is licensed under the MIT license.
