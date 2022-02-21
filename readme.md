A conceptual prototype for a measurement device.

Two firmware pieces are available. The main software is the RP2040 code found in the root folder and built with CMake. It's purpose is handling user interaction.
Main is located in `display_test.c`

The second firmware targets the SAMD21G18A and is built with Atmel Studio. It is located in `samd21-afe`. It is only responsible for making and providing ADC measurements to the RP2040.