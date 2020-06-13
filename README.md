# STM8 Device Headers 

Open-Sources device headers for all [STM8 microcontroller](https://www.st.com/en/microcontrollers-microprocessors/stm8-8-bit-mcus.html)
series, namely STM8A, STM8L, STM8AF and STM8AL. 

Device headers
  - are compatible with [SDCC](http://sdcc.sourceforge.net/), 
[Cosmic](https://cosmic-software.com/stm8.php) and [IAR](https://www.iar.com/iar-embedded-workbench).
  - can be mixed with the respective STM8 
[Standard Peripheral Library](https://www.st.com/content/st_com/en/search.html#q=STM8%20Standard%20Peripheral%20Library-t=tools-page=1) (SPL) by STM (see below example). Note that
SPL must be patched for use with [SDCC](http://sdcc.sourceforge.net/), see e.g. [here](https://github.com/gicking/STM8-SPL_SDCC_patch).


# XML Device Description 

Headers were generated from device descriptions in XML format, which are in folder 
[XML](https://github.com/gicking/STM8_headers/tree/master/XML). These descriptions could be used e.g. for
a graphical debugger interface.


# Example Projects 

These projects have been tested successfully with [SDCC](http://sdcc.sourceforge.net/), 
[Cosmic](https://cosmic-software.com/stm8.php) and [IAR](https://www.iar.com/iar-embedded-workbench)

**adc_measure**
  - measure analog voltage every ~500ms
  - print value via UART using printf()

------------------------

**blink_interrupt**
  - blink LED in TIM4 interrupt service routine

------------------------

**blink_noInterrupt**
  - blink LED with a simple NOP() loop

------------------------

**I2C_LCD**
  - Periodically print text to 2x16 char LCD attached to I2C
  - LCD type Batron BTHQ21605V-COG-FSRE-I2C 2X16 (Farnell 1220409)

------------------------

**low-power_auto-wake**
  - enter power-down mode with wake via port-ISR or AWU

------------------------

**millis_delay**
  - implement 1ms ISR, millis(), micros() etc., similar to  Arduino

------------------------

**pin_interrupt**
  - TLI interrupt on pin D7 (not port). Corresponds to INTx on Arduino

------------------------

**pin_read_write**
  - read and write pin states without interrupts

------------------------

**port_interrupt**
  - port interrupt on pin PE5. Corresponds to port change interrupts on Arduino

------------------------

**PWM_generate**
  - generate a PWM on pin PD2/TIM3_CH1 (=pin 3 on sduino)

------------------------

**read_unique_ID**
  - read unique identifier and print via UART
  - Note: UID not supported by all devices

------------------------

**read_write_D-flash**
  - save data to D-flash/EEPROM and read back

------------------------

**read_write_option_bytes**
  - change option bytes, e.g. activate/deactivate the ROM-bootloader

------------------------

**read_write_P-flash**
  - save data to P-flash and read back

------------------------

**serial_gets_printf**
  - serial input/output with gets and printf
  - without interrupts or FIFO

------------------------

**serial_printf**
  - serial output via printf
  - without interrupts or FIFO

------------------------

**serial_printf_FIFO_interrupt**
  - serial output via printf
  - using Tx & Rx FIFO and interrupts
  - similar to Arduino Serial class
  
------------------------

**serial_write**
  - echo read bytes from UART
  - without interrupts or FIFO

------------------------

**STM8_StdPeriphLib**
  - mix with functions/headers of the STM8S Standard Peripheral Library (SPL)
  - SDCC compatibility requires patch, see e.g. [here](https://github.com/gicking/STM8-SPL_SDCC_patch).


