# STM8 Device Headers

**This is a fork from [https://github.com/gicking/STM8_Headers](https://github.com/gicking/STM8_Headers)**


Main target of this project is to organize examples into small, separate libraries, such as
- [GPIO](https://github.com/ststm8/gpio)
- [UART](https://github.com/ststm8/uart)
- etc

based on main [headers, (past STM8_Headers)](https://github.com/ststm8/headers) library

Thanks a lot to **Georg Icking-Konert** aka **@gicking**

---

Open-Sources device headers for all [STM8 microcontroller](https://www.st.com/en/microcontrollers-microprocessors/stm8-8-bit-mcus.html)
series, namely:
* STM8A
* STM8S
* STM8L
* STM8AF
* STM8AL.

Device headers
  - are compatible with [SDCC](http://sdcc.sourceforge.net/),
[Cosmic](https://cosmic-software.com/stm8.php) and [IAR](https://www.iar.com/iar-embedded-workbench).
  - can be mixed with the respective STM8
[Standard Peripheral Library](https://www.st.com/content/st_com/en/search.html#q=STM8%20Standard%20Peripheral%20Library-t=tools-page=1) (SPL) by STM (see below example). Note that
SPL must be patched for use with [SDCC](http://sdcc.sourceforge.net/), see e.g. [here](https://github.com/gicking/STM8-SPL_SDCC_patch).


# XML Device Description

Headers were generated from device descriptions in XML format, which are in folder
[XML](https://github.com/ststm8/headers/tree/master/XML). These descriptions could be used e.g. for
a graphical debugger interface.


# Related Projects

The below projects have been ported to use these open-sources device headers for STMo.

**atomthreads**
  - port is available under [https://github.com/gicking/atomthreads](https://github.com/gicking/atomthreads), see "ports/stm8_oss"
  - all provided tests have passed for [SDCC](http://sdcc.sourceforge.net/), [Cosmic](https://cosmic-software.com/stm8.php) and
  [IAR](https://www.iar.com/iar-embedded-workbench). Exception is IAR/queue9 which exceeds the size limit of my demo license



