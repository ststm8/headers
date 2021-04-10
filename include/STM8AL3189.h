/*-------------------------------------------------------------------------

  STM8AL3189.h - Device Declarations

  STM8L/STM8AL, high/medium+ density with ROM bootloader

  Copyright (C) 2020, Georg Icking-Konert

  Automotive 8-bit ultra-low-power MCU with 64 Kbytes Flash, LIN, RTC, data EEPROM, timers, USARTs, I2C, SPIs, ADC, DAC, COMPs 

  datasheet: https://www.st.com/resource/en/datasheet/stm8al3189.pdf
  reference: RM0031 https://www.st.com/content/ccc/resource/technical/document/reference_manual/2e/3b/8c/8f/60/af/4b/2c/CD00218714.pdf/files/CD00218714.pdf/jcr:content/translations/en.CD00218714.pdf

  MIT License

  Copyright (c) 2020 Georg Icking-Konert

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
  MODULE DEFINITION FOR MULTIPLE INCLUSION
-------------------------------------------------------------------------*/
#ifndef STM8AL3189_H
#define STM8AL3189_H

// DEVICE NAME
#define DEVICE_STM8AL3189

// DEVICE FAMILY
#define FAMILY_STM8L

// required for C++
#ifdef __cplusplus
  extern "C" {
#endif


/*-------------------------------------------------------------------------
  INCLUDE FILES
-------------------------------------------------------------------------*/
#include <stdint.h>


/*-------------------------------------------------------------------------
  COMPILER SPECIFIC SETTINGS
-------------------------------------------------------------------------*/

// Cosmic compiler
#if defined(__CSMC__)

  // macros to unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)  @far @interrupt void func(void)      ///< handler for interrupt service routine
  #define ISR_HANDLER_TRAP(func) void @far @interrupt func(void)      ///< handler for trap service routine

  // definition of inline functions
  #define INLINE                 @inline                              ///< keyword for inline functions

  // common assembler instructions
  #define NOP()                  _asm("nop")                          ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   _asm("sim")                          ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    _asm("rim")                          ///< enable interrupt handling
  #define TRIGGER_TRAP           _asm("trap")                         ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   _asm("wfi")                          ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           _asm("halt")                         ///< put controller to HALT mode
  #define SW_RESET()             _asm("dc.b $75")                     ///< reset via illegal opcode (works for all devices)

  // data type in bit fields
  #define BITFIELD_UINT          unsigned int                         ///< data type in bit structs (follow C90 standard)


// IAR Compiler
#elif defined(__ICCSTM8__)

  // include intrinsic functions
  #include <intrinsics.h>

  // macros to unify ISR declaration and implementation
  #define STRINGVECTOR(x) #x
  #define VECTOR_ID(x) STRINGVECTOR( vector = (x) )
  #define ISR_HANDLER( a, b )  \
    _Pragma( VECTOR_ID( (b)+2 ) )        \
    __interrupt void (a)( void )
  #define ISR_HANDLER_TRAP(a) \
    _Pragma( VECTOR_ID( 1 ) ) \
    __interrupt void (a) (void)  

  // definition of inline functions
  #define INLINE                 static inline                        ///< keyword for inline functions

  // common assembler instructions
  #define NOP()                  __no_operation()                     ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   __disable_interrupt()                ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    __enable_interrupt()                 ///< enable interrupt handling
  #define TRIGGER_TRAP           __trap()                             ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   __wait_for_interrupt()               ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           __halt()                             ///< put controller to HALT mode
  #define SW_RESET()             __asm("dc8 0x75")                    ///< reset via illegal opcode (works for all devices)

  // data type in bit fields
  #define BITFIELD_UINT          unsigned char                        ///< data type in bit structs (deviating from C90 standard)


// SDCC compiler
#elif defined(__SDCC)

  // store SDCC version in preprocessor friendly way
  #define SDCC_VERSION (__SDCC_VERSION_MAJOR * 10000 \
                      + __SDCC_VERSION_MINOR * 100 \
                      + __SDCC_VERSION_PATCH)

  // unify ISR declaration and implementation
  #define ISR_HANDLER(func,irq)   void func(void) __interrupt(irq)    ///< handler for interrupt service routine
  #if SDCC_VERSION >= 30403  // traps require >=v3.4.3
    #define ISR_HANDLER_TRAP(func)  void func() __trap                ///< handler for trap service routine
  #else
    #error traps require SDCC >=3.4.3. Please update!
  #endif

  // definition of inline functions
  #define INLINE                 static inline                        ///< keyword for inline functions

  // common assembler instructions
  #define NOP()                  __asm__("nop")                       ///< perform a nop() operation (=minimum delay)
  #define DISABLE_INTERRUPTS()   __asm__("sim")                       ///< disable interrupt handling
  #define ENABLE_INTERRUPTS()    __asm__("rim")                       ///< enable interrupt handling
  #define TRIGGER_TRAP           __asm__("trap")                      ///< trigger a trap (=soft interrupt) e.g. for EMC robustness (see AN1015)
  #define WAIT_FOR_INTERRUPT()   __asm__("wfi")                       ///< stop code execution and wait for interrupt
  #define ENTER_HALT()           __asm__("halt")                      ///< put controller to HALT mode
  #define SW_RESET()             __asm__(".db 0x75")                  ///< reset via illegal opcode (works for all devices)

  // data type in bit fields
  #define BITFIELD_UINT          unsigned int                         ///< data type in bit structs (follow C90 standard)

// unsupported compiler -> stop
#else
  #error: compiler not supported
#endif


/*-------------------------------------------------------------------------
  FOR CONVENIENT PIN ACCESS
-------------------------------------------------------------------------*/

#define PIN0     0x01
#define PIN1     0x02
#define PIN2     0x04
#define PIN3     0x08
#define PIN4     0x10
#define PIN5     0x20
#define PIN6     0x40
#define PIN7     0x80


/*-------------------------------------------------------------------------
  DEVICE MEMORY (size in bytes)
-------------------------------------------------------------------------*/

// RAM
#define RAM_ADDR_START                0x000000
#define RAM_ADDR_END                  0x000FFF
#define RAM_SIZE                      4096


// EEPROM
#define EEPROM_ADDR_START             0x001000
#define EEPROM_ADDR_END               0x0017FF
#define EEPROM_SIZE                   2048


// OPTION
#define OPTION_ADDR_START             0x004800
#define OPTION_ADDR_END               0x0048FF
#define OPTION_SIZE                   256


// STORAGE
#define STORAGE_ADDR_START            0x004910
#define STORAGE_ADDR_END              0x004912
#define STORAGE_SIZE                  3


// ID
#define ID_ADDR_START                 0x004926
#define ID_ADDR_END                   0x004931
#define ID_SIZE                       12


// SFR1
#define SFR1_ADDR_START               0x005000
#define SFR1_ADDR_END                 0x0057FF
#define SFR1_SIZE                     2048


// BOOTROM
#define BOOTROM_ADDR_START            0x006000
#define BOOTROM_ADDR_END              0x0067FF
#define BOOTROM_SIZE                  2048


// SFR2
#define SFR2_ADDR_START               0x007F00
#define SFR2_ADDR_END                 0x007FFF
#define SFR2_SIZE                     256


// FLASH
#define FLASH_ADDR_START              0x008000
#define FLASH_ADDR_END                0x017FFF
#define FLASH_SIZE                    65536


// MEMORY WIDTH (>32kB flash exceeds 16bit, as flash starts at 0x8000)
#define FLASH_ADDR_WIDTH            32                    ///< width of address space
#define FLASH_POINTER_T             uint32_t              ///< address variable type


/*-------------------------------------------------------------------------
  UNIQUE IDENTIFIER (size in bytes)
-------------------------------------------------------------------------*/

#define UID_ADDR_START                0x4926                ///< start address of unique identifier
#define UID_SIZE                      12                    ///< size of unique identifier [B]
#define UID(N)                        (*((uint8_t*) (UID_ADDR_START+N)))    ///< read unique identifier byte N


/*-------------------------------------------------------------------------
  MISC OPTIONS
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
  ISR Vector Table (SDCC, IAR)
  Notes:
    - IAR has an IRQ offset of +2 compared to datasheet and below numbers
    - Cosmic uses a separate, device specific file 'stm8_interrupt_vector.c'
    - different interrupt sources may share the same IRQ
-------------------------------------------------------------------------*/

// interrupt                                   IRQ
#define _TLI_VECTOR_                             0          
#define _FLASH_EOP_VECTOR_                       1          ///< FLASH_EOP interrupt vector: enable: FLASH_CR1.IE, pending: FLASH_IAPSR.EOP, priority: ITC_SPR1.VECT1SPR
#define _FLASH_WR_PG_DIS_VECTOR_                 1          ///< FLASH_WR_PG_DIS interrupt vector: enable: FLASH_CR1.IE, pending: FLASH_IAPSR.WR_PG_DIS, priority: ITC_SPR1.VECT1SPR
#define _DMA1_CH0_HT_VECTOR_                     2          ///< DMA1_CH0_HT interrupt vector: enable: DMA1_C0CR.HTIE, pending: DMA1_C0SPR.HTIF, priority: ITC_SPR1.VECT2SPR
#define _DMA1_CH0_TC_VECTOR_                     2          ///< DMA1_CH0_TC interrupt vector: enable: DMA1_C0CR.TCIE, pending: DMA1_C0SPR.TCIF, priority: ITC_SPR1.VECT2SPR
#define _DMA1_CH1_HT_VECTOR_                     2          ///< DMA1_CH1_HT interrupt vector: enable: DMA1_C1CR.HTIE, pending: DMA1_C1SPR.HTIF, priority: ITC_SPR1.VECT2SPR
#define _DMA1_CH1_TC_VECTOR_                     2          ///< DMA1_CH1_TC interrupt vector: enable: DMA1_C1CR.TCIE, pending: DMA1_C1SPR.TCIF, priority: ITC_SPR1.VECT2SPR
#define _DMA1_CH2_HT_VECTOR_                     3          ///< DMA1_CH2_HT interrupt vector: enable: DMA1_C2CR.HTIE, pending: DMA1_C2SPR.HTIF, priority: ITC_SPR1.VECT3SPR
#define _DMA1_CH2_TC_VECTOR_                     3          ///< DMA1_CH2_TC interrupt vector: enable: DMA1_C2CR.TCIE, pending: DMA1_C2SPR.TCIF, priority: ITC_SPR1.VECT3SPR
#define _DMA1_CH3_HT_VECTOR_                     3          ///< DMA1_CH3_HT interrupt vector: enable: DMA1_C3CR.HTIE, pending: DMA1_C3SPR.HTIF, priority: ITC_SPR1.VECT3SPR
#define _DMA1_CH3_TC_VECTOR_                     3          ///< DMA1_CH3_TC interrupt vector: enable: DMA1_C3CR.TCIE, pending: DMA1_C3SPR.TCIF, priority: ITC_SPR1.VECT3SPR
#define _LSE_CSS_VECTOR_                         4          ///< LSE_CSS interrupt vector: enable: CSS_LSE_CSR.CSSIE, pending: CSS_LSE_CSR.CSSF, priority: ITC_SPR2.VECT4SPR
#define _RTC_ALARM_VECTOR_                       4          ///< RTC_ALARM interrupt vector: enable: RTC_CR2.ALRAIE, pending: RTC_ISR2.ALRAF, priority: ITC_SPR2.VECT4SPR
#define _RTC_TAMP1_VECTOR_                       4          ///< RTC_TAMP1 interrupt vector: enable: RTC_TCR1.TAMPIE, pending: RTC_ISR2.TAMP1F, priority: ITC_SPR2.VECT4SPR
#define _RTC_TAMP2_VECTOR_                       4          ///< RTC_TAMP2 interrupt vector: enable: RTC_TCR1.TAMPIE, pending: RTC_ISR2.TAMP2F, priority: ITC_SPR2.VECT4SPR
#define _RTC_TAMP3_VECTOR_                       4          ///< RTC_TAMP3 interrupt vector: enable: RTC_TCR1.TAMPIE, pending: RTC_ISR2.TAMP3F, priority: ITC_SPR2.VECT4SPR
#define _RTC_WAKEUP_VECTOR_                      4          ///< RTC_WAKEUP interrupt vector: enable: RTC_CR2.WUTIE, pending: RTC_ISR2.WUTF, priority: ITC_SPR2.VECT4SPR
#define _EXTIE_VECTOR_                           5          ///< EXTIE interrupt vector: enable: EXTI_CR3.PEIS, pending: EXTI_SR2.PEF, priority: ITC_SPR2.VECT5SPR
#define _EXTIF_VECTOR_                           5          ///< EXTIF interrupt vector: enable: EXTI_CR3.PFIS, pending: EXTI_SR2.PFF, priority: ITC_SPR2.VECT5SPR
#define _EXTIPVD_VECTOR_                         5          ///< EXTIPVD interrupt vector: enable: PWR_CSR1.PVDE, pending: PWR_CSR1.PVDIF, priority: ITC_SPR2.VECT5SPR
#define _EXTIB_VECTOR_                           6          ///< EXTIB interrupt vector: enable: EXTI_CR3.PBIS, pending: EXTI_SR2.PBF, priority: ITC_SPR2.VECT6SPR
#define _EXTIG_VECTOR_                           6          ///< EXTIG interrupt vector: enable: EXTI_CR4.PGIS, pending: EXTI_SR2.PGF, priority: ITC_SPR2.VECT6SPR
#define _EXTID_VECTOR_                           7          ///< EXTID interrupt vector: enable: EXTI_CR3.PDIS, pending: EXTI_SR2.PDF, priority: ITC_SPR2.VECT7SPR
#define _EXTIH_VECTOR_                           7          ///< EXTIH interrupt vector: enable: EXTI_CR4.PHIS, pending: EXTI_SR2.PHF, priority: ITC_SPR2.VECT7SPR
#define _EXTI0_VECTOR_                           8          ///< EXTI0 interrupt vector: enable: EXTI_CR1.P0IS, pending: EXTI_SR1.P0F, priority: ITC_SPR3.VECT8SPR
#define _EXTI1_VECTOR_                           9          ///< EXTI1 interrupt vector: enable: EXTI_CR1.P1IS, pending: EXTI_SR1.P1F, priority: ITC_SPR3.VECT9SPR
#define _EXTI2_VECTOR_                           10         ///< EXTI2 interrupt vector: enable: EXTI_CR1.P2IS, pending: EXTI_SR1.P2F, priority: ITC_SPR3.VECT10SPR
#define _EXTI3_VECTOR_                           11         ///< EXTI3 interrupt vector: enable: EXTI_CR1.P3IS, pending: EXTI_SR1.P3F, priority: ITC_SPR3.VECT11SPR
#define _EXTI4_VECTOR_                           12         ///< EXTI4 interrupt vector: enable: EXTI_CR2.P4IS, pending: EXTI_SR1.P4F, priority: ITC_SPR4.VECT12SPR
#define _EXTI5_VECTOR_                           13         ///< EXTI5 interrupt vector: enable: EXTI_CR2.P5IS, pending: EXTI_SR1.P5F, priority: ITC_SPR4.VECT13SPR
#define _EXTI6_VECTOR_                           14         ///< EXTI6 interrupt vector: enable: EXTI_CR2.P6IS, pending: EXTI_SR1.P6F, priority: ITC_SPR4.VECT14SPR
#define _EXTI7_VECTOR_                           15         ///< EXTI7 interrupt vector: enable: EXTI_CR2.P7IS, pending: EXTI_SR1.P7F, priority: ITC_SPR4.VECT15SPR
#define _CLK_CSS_VECTOR_                         17         ///< CLK_CSS interrupt vector: enable: CLK_CSSR.CSSDIE, pending: CLK_CSSR.CSSD, priority: ITC_SPR4.VECT17SPR
#define _CLK_SWITCH_VECTOR_                      17         ///< CLK_SWITCH interrupt vector: enable: CLK_SWCR.SWIEN, pending: CLK_SWCR.SWIF, priority: ITC_SPR4.VECT17SPR
#define _DAC_DMAUDR1_VECTOR_                     17         ///< DAC_DMAUDR1 interrupt vector: enable: DAC_CH1CR2.DMAUDRIE, pending: DAC_SR.DMAUDR1, priority: ITC_SPR4.VECT17SPR
#define _DAC_DMAUDR2_VECTOR_                     17         ///< DAC_DMAUDR2 interrupt vector: enable: DAC_CH2CR2.DMAUDRIE, pending: DAC_SR.DMAUDR2, priority: ITC_SPR4.VECT17SPR
#define _TIM1_BIF_VECTOR_                        17         ///< TIM1_BIF interrupt vector: enable: TIM1_IER.BIE, pending: TIM1_SR1.BIF, priority: ITC_SPR4.VECT17SPR
#define _ADC1_AWD_VECTOR_                        18         ///< ADC1_AWD interrupt vector: enable: ADC1_CR1.AWDIE, pending: ADC1_SR.AWD, priority: ITC_SPR5.VECT18SPR
#define _ADC1_EOC_VECTOR_                        18         ///< ADC1_EOC interrupt vector: enable: ADC1_CR1.EOCIE, pending: ADC1_SR.EOC, priority: ITC_SPR5.VECT18SPR
#define _ADC1_OVER_VECTOR_                       18         ///< ADC1_OVER interrupt vector: enable: ADC1_CR1.OVERIE, pending: ADC1_SR.OVER, priority: ITC_SPR5.VECT18SPR
#define _COMP_EF1_VECTOR_                        18         ///< COMP_EF1 interrupt vector: enable: COMP_CSR1.IE1, pending: COMP_CSR1.EF1, priority: ITC_SPR5.VECT18SPR
#define _COMP_EF2_VECTOR_                        18         ///< COMP_EF2 interrupt vector: enable: COMP_CSR2.IE2, pending: COMP_CSR2.EF2, priority: ITC_SPR5.VECT18SPR
#define _TIM2_BIF_VECTOR_                        19         ///< TIM2_BIF interrupt vector: enable: TIM2_IER.BIE, pending: TIM2_SR1.BIF, priority: ITC_SPR5.VECT19SPR
#define _TIM2_OVR_UIF_VECTOR_                    19         ///< TIM2_OVR_UIF interrupt vector: enable: TIM2_IER.UIE, pending: TIM2_SR1.UIF, priority: ITC_SPR5.VECT19SPR
#define _TIM2_TIF_VECTOR_                        19         ///< TIM2_TIF interrupt vector: enable: TIM2_IER.TIE, pending: TIM2_SR1.TIF, priority: ITC_SPR5.VECT19SPR
#define _USART2_T_TC_VECTOR_                     19         ///< USART2_T_TC interrupt vector: enable: USART2_CR2.TCIEN, pending: USART2_SR.TC, priority: ITC_SPR5.VECT19SPR
#define _USART2_T_TXE_VECTOR_                    19         ///< USART2_T_TXE interrupt vector: enable: USART2_CR2.TIEN, pending: USART2_SR.TXE, priority: ITC_SPR5.VECT19SPR
#define _TIM2_CAPCOM_CC1IF_VECTOR_               20         ///< TIM2_CAPCOM_CC1IF interrupt vector: enable: TIM2_IER.CC1IE, pending: TIM2_SR1.CC1IF, priority: ITC_SPR6.VECT20SPR
#define _TIM2_CAPCOM_CC2IF_VECTOR_               20         ///< TIM2_CAPCOM_CC2IF interrupt vector: enable: TIM2_IER.CC2IE, pending: TIM2_SR1.CC2IF, priority: ITC_SPR6.VECT20SPR
#define _USART2_R_IDLE_VECTOR_                   20         ///< USART2_R_IDLE interrupt vector: enable: USART2_CR2.ILIEN, pending: USART2_SR.IDLE, priority: ITC_SPR6.VECT20SPR
#define _USART2_R_OR_VECTOR_                     20         ///< USART2_R_OR interrupt vector: enable: USART2_CR2.RIEN, pending: USART2_SR.OR, priority: ITC_SPR6.VECT20SPR
#define _USART2_R_PE_VECTOR_                     20         ///< USART2_R_PE interrupt vector: enable: USART2_CR1.PIEN, pending: USART2_SR.PE, priority: ITC_SPR6.VECT20SPR
#define _USART2_R_RXNE_VECTOR_                   20         ///< USART2_R_RXNE interrupt vector: enable: USART2_CR2.RIEN, pending: USART2_SR.RXNE, priority: ITC_SPR6.VECT20SPR
#define _TIM3_BIF_VECTOR_                        21         ///< TIM3_BIF interrupt vector: enable: TIM3_IER.BIE, pending: TIM3_SR1.BIF, priority: ITC_SPR5.VECT21SPR
#define _TIM3_OVR_UIF_VECTOR_                    21         ///< TIM3_OVR_UIF interrupt vector: enable: TIM3_IER.UIE, pending: TIM3_SR1.UIF, priority: ITC_SPR5.VECT21SPR
#define _TIM3_TIF_VECTOR_                        21         ///< TIM3_TIF interrupt vector: enable: TIM3_IER.TIE, pending: TIM3_SR1.TIF, priority: ITC_SPR5.VECT21SPR
#define _USART3_T_TC_VECTOR_                     21         ///< USART3_T_TC interrupt vector: enable: USART3_CR2.TCIEN, pending: USART3_SR.TC, priority: ITC_SPR5.VECT21SPR
#define _USART3_T_TXE_VECTOR_                    21         ///< USART3_T_TXE interrupt vector: enable: USART3_CR2.TIEN, pending: USART3_SR.TXE, priority: ITC_SPR5.VECT21SPR
#define _TIM3_CAPCOM_CC1IF_VECTOR_               22         ///< TIM3_CAPCOM_CC1IF interrupt vector: enable: TIM3_IER.CC1IE, pending: TIM3_SR1.CC1IF, priority: ITC_SPR6.VECT22SPR
#define _TIM3_CAPCOM_CC2IF_VECTOR_               22         ///< TIM3_CAPCOM_CC2IF interrupt vector: enable: TIM3_IER.CC2IE, pending: TIM3_SR1.CC2IF, priority: ITC_SPR6.VECT22SPR
#define _USART3_R_IDLE_VECTOR_                   22         ///< USART3_R_IDLE interrupt vector: enable: USART3_CR2.ILIEN, pending: USART3_SR.IDLE, priority: ITC_SPR6.VECT22SPR
#define _USART3_R_OR_VECTOR_                     22         ///< USART3_R_OR interrupt vector: enable: USART3_CR2.RIEN, pending: USART3_SR.OR, priority: ITC_SPR6.VECT22SPR
#define _USART3_R_PE_VECTOR_                     22         ///< USART3_R_PE interrupt vector: enable: USART3_CR1.PIEN, pending: USART3_SR.PE, priority: ITC_SPR6.VECT22SPR
#define _USART3_R_RXNE_VECTOR_                   22         ///< USART3_R_RXNE interrupt vector: enable: USART3_CR2.RIEN, pending: USART3_SR.RXNE, priority: ITC_SPR6.VECT22SPR
#define _TIM1_OVR_UIF_VECTOR_                    23         ///< TIM1_OVR_UIF interrupt vector: enable: TIM1_IER.UIE, pending: TIM1_SR1.UIF, priority: ITC_SPR6.VECT23SPR
#define _TIM1_CAPCOM_CC1IF_VECTOR_               24         ///< TIM1_CAPCOM_CC1IF interrupt vector: enable: TIM1_IER.CC1IE, pending: TIM1_SR1.CC1IF, priority: ITC_SPR6.VECT24SPR
#define _TIM1_CAPCOM_CC2IF_VECTOR_               24         ///< TIM1_CAPCOM_CC2IF interrupt vector: enable: TIM1_IER.CC2IE, pending: TIM1_SR1.CC2IF, priority: ITC_SPR6.VECT24SPR
#define _TIM1_CAPCOM_CC3IF_VECTOR_               24         ///< TIM1_CAPCOM_CC3IF interrupt vector: enable: TIM1_IER.CC3IE, pending: TIM1_SR1.CC2IF, priority: ITC_SPR6.VECT24SPR
#define _TIM1_CAPCOM_CC4IF_VECTOR_               24         ///< TIM1_CAPCOM_CC4IF interrupt vector: enable: TIM1_IER.CC4IE, pending: TIM1_SR1.CC4IF, priority: ITC_SPR6.VECT24SPR
#define _TIM1_CAPCOM_COMIF_VECTOR_               24         ///< TIM1_CAPCOM_COMIF interrupt vector: enable: TIM1_IER.COMIE, pending: TIM1_SR1.COMIF, priority: ITC_SPR6.VECT24SPR
#define _TIM1_CAPCOM_TIF_VECTOR_                 24         ///< TIM1_CAPCOM_TIF interrupt vector: enable: TIM1_IER.TIE, pending: TIM1_SR1.TIF, priority: ITC_SPR6.VECT24SPR
#define _TIM4_TIF_VECTOR_                        25         ///< TIM4_TIF interrupt vector: enable: TIM4_IER.TIE, pending: TIM4_SR1.TIF, priority: ITC_SPR7.VECT25SPR
#define _TIM4_UIF_VECTOR_                        25         ///< TIM4_UIF interrupt vector: enable: TIM4_IER.UIE, pending: TIM4_SR1.UIF, priority: ITC_SPR7.VECT25SPR
#define _SPI1_MODF_VECTOR_                       26         ///< SPI1_MODF interrupt vector: enable: SPI1_ICR.ERRIE, pending: SPI1_SR.MODF, priority: ITC_SPR7.VECT26SPR
#define _SPI1_OVR_VECTOR_                        26         ///< SPI1_OVR interrupt vector: enable: SPI1_ICR.ERRIE, pending: SPI1_SR.OVR, priority: ITC_SPR7.VECT26SPR
#define _SPI1_RXNE_VECTOR_                       26         ///< SPI1_RXNE interrupt vector: enable: SPI1_ICR.RXIE, pending: SPI1_SR.RXNE, priority: ITC_SPR7.VECT26SPR
#define _SPI1_TXE_VECTOR_                        26         ///< SPI1_TXE interrupt vector: enable: SPI1_ICR.TXIE, pending: SPI1_SR.TXE, priority: ITC_SPR7.VECT26SPR
#define _SPI1_WKUP_VECTOR_                       26         ///< SPI1_WKUP interrupt vector: enable: SPI1_ICR.WKIE, pending: SPI1_SR.WKUP, priority: ITC_SPR7.VECT26SPR
#define _TIM5_BIF_VECTOR_                        27         ///< TIM5_BIF interrupt vector: enable: TIM5_IER.BIE, pending: TIM5_SR1.BIF, priority: ITC_SPR7.VECT27SPR
#define _TIM5_OVR_UIF_VECTOR_                    27         ///< TIM5_OVR_UIF interrupt vector: enable: TIM5_IER.UIE, pending: TIM5_SR1.UIF, priority: ITC_SPR7.VECT27SPR
#define _TIM5_TIF_VECTOR_                        27         ///< TIM5_TIF interrupt vector: enable: TIM5_IER.TIE, pending: TIM5_SR1.TIF, priority: ITC_SPR7.VECT27SPR
#define _USART1_T_TC_VECTOR_                     27         ///< USART1_T_TC interrupt vector: enable: USART1_CR2.TCIEN, pending: USART1_SR.TC, priority: ITC_SPR7.VECT27SPR
#define _USART1_T_TXE_VECTOR_                    27         ///< USART1_T_TXE interrupt vector: enable: USART1_CR2.TIEN, pending: USART1_SR.TXE, priority: ITC_SPR7.VECT27SPR
#define _TIM5_CAPCOM_CC1IF_VECTOR_               28         ///< TIM5_CAPCOM_CC1IF interrupt vector: enable: TIM5_IER.CC1IE, pending: TIM5_SR1.CC1IF, priority: ITC_SPR7.VECT28SPR
#define _TIM5_CAPCOM_CC2IF_VECTOR_               28         ///< TIM5_CAPCOM_CC2IF interrupt vector: enable: TIM5_IER.CC2IE, pending: TIM5_SR1.CC2IF, priority: ITC_SPR7.VECT28SPR
#define _USART1_R_IDLE_VECTOR_                   28         ///< USART1_R_IDLE interrupt vector: enable: USART1_CR2.ILIEN, pending: USART1_SR.IDLE, priority: ITC_SPR7.VECT28SPR
#define _USART1_R_OR_VECTOR_                     28         ///< USART1_R_OR interrupt vector: enable: USART1_CR2.RIEN, pending: USART1_SR.OR, priority: ITC_SPR7.VECT28SPR
#define _USART1_R_PE_VECTOR_                     28         ///< USART1_R_PE interrupt vector: enable: USART1_CR1.PIEN, pending: USART1_SR.PE, priority: ITC_SPR7.VECT28SPR
#define _USART1_R_RXNE_VECTOR_                   28         ///< USART1_R_RXNE interrupt vector: enable: USART1_CR2.RIEN, pending: USART1_SR.RXNE, priority: ITC_SPR7.VECT28SPR
#define _I2C1_ADD10_VECTOR_                      29         ///< I2C1_ADD10 interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR1.ADD10, priority: ITC_SPR8.VECT29SPR
#define _I2C1_ADDR_VECTOR_                       29         ///< I2C1_ADDR interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR1.ADDR, priority: ITC_SPR8.VECT29SPR
#define _I2C1_AF_VECTOR_                         29         ///< I2C1_AF interrupt vector: enable: I2C1_ITR.ITERREN, pending: I2C1_SR2.AF, priority: ITC_SPR8.VECT29SPR
#define _I2C1_ARLO_VECTOR_                       29         ///< I2C1_ARLO interrupt vector: enable: I2C1_ITR.ITERREN, pending: I2C1_SR2.ARLO, priority: ITC_SPR8.VECT29SPR
#define _I2C1_BERR_VECTOR_                       29         ///< I2C1_BERR interrupt vector: enable: I2C1_ITR.ITERREN, pending: I2C1_SR2.BERR, priority: ITC_SPR8.VECT29SPR
#define _I2C1_BTF_VECTOR_                        29         ///< I2C1_BTF interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR1.BTF, priority: ITC_SPR8.VECT29SPR
#define _I2C1_OVR_VECTOR_                        29         ///< I2C1_OVR interrupt vector: enable: I2C1_ITR.ITERREN, pending: I2C1_SR2.OVR, priority: ITC_SPR8.VECT29SPR
#define _I2C1_RXNE_VECTOR_                       29         ///< I2C1_RXNE interrupt vector: enable: I2C1_ITR.ITBUFEN, pending: I2C1_SR1.RXNE, priority: ITC_SPR8.VECT29SPR
#define _I2C1_SB_VECTOR_                         29         ///< I2C1_SB interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR1.SB, priority: ITC_SPR8.VECT29SPR
#define _I2C1_STOPF_VECTOR_                      29         ///< I2C1_STOPF interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR1.STOPF, priority: ITC_SPR8.VECT29SPR
#define _I2C1_TXE_VECTOR_                        29         ///< I2C1_TXE interrupt vector: enable: I2C1_ITR.ITBUFEN, pending: I2C1_SR1.TXE, priority: ITC_SPR8.VECT29SPR
#define _I2C1_WUFH_VECTOR_                       29         ///< I2C1_WUFH interrupt vector: enable: I2C1_ITR.ITEVTEN, pending: I2C1_SR2.WUFH, priority: ITC_SPR8.VECT29SPR
#define _SPI2_MODF_VECTOR_                       29         ///< SPI2_MODF interrupt vector: enable: SPI2_ICR.ERRIE, pending: SPI2_SR.MODF, priority: ITC_SPR8.VECT29SPR
#define _SPI2_OVR_VECTOR_                        29         ///< SPI2_OVR interrupt vector: enable: SPI2_ICR.ERRIE, pending: SPI2_SR.OVR, priority: ITC_SPR8.VECT29SPR
#define _SPI2_RXNE_VECTOR_                       29         ///< SPI2_RXNE interrupt vector: enable: SPI2_ICR.RXIE, pending: SPI2_SR.RXNE, priority: ITC_SPR8.VECT29SPR
#define _SPI2_TXE_VECTOR_                        29         ///< SPI2_TXE interrupt vector: enable: SPI2_ICR.TXIE, pending: SPI2_SR.TXE, priority: ITC_SPR8.VECT29SPR
#define _SPI2_WKUP_VECTOR_                       29         ///< SPI2_WKUP interrupt vector: enable: SPI2_ICR.WKIE, pending: SPI2_SR.WKUP, priority: ITC_SPR8.VECT29SPR


/*-------------------------------------------------------------------------
  DEFINITION OF STM8 PERIPHERAL REGISTERS
-------------------------------------------------------------------------*/

//------------------------
// Module ADC1
//------------------------

/** struct containing ADC1 module registers */
typedef struct {

  /** ADC1 configuration register 1 (CR1 at 0x5340) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   ADON       : 1;      // bit 0
      BITFIELD_UINT   START      : 1;      // bit 1
      BITFIELD_UINT   CONT       : 1;      // bit 2
      BITFIELD_UINT   EOCIE      : 1;      // bit 3
      BITFIELD_UINT   AWDIE      : 1;      // bit 4
      BITFIELD_UINT   RES        : 2;      // bits 5-6
      BITFIELD_UINT   OVERIE     : 1;      // bit 7
    };  // CR1 bitfield

    /// register _ADC1_CR1 reset value
    #define sfr_ADC1_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** ADC1 configuration register 2 (CR2 at 0x5341) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SMTP1      : 3;      // bits 0-2
      BITFIELD_UINT   EXTSEL0    : 1;      // bit 3
      BITFIELD_UINT   EXTSEL1    : 1;      // bit 4
      BITFIELD_UINT   TRIG_EDGE0 : 1;      // bit 5
      BITFIELD_UINT   TRIG_EDGE1 : 1;      // bit 6
      BITFIELD_UINT   PRESC      : 1;      // bit 7
    };  // CR2 bitfield

    /// register _ADC1_CR2 reset value
    #define sfr_ADC1_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** ADC1 configuration register 3 (CR3 at 0x5342) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   CHSEL      : 5;      // bits 0-4
      BITFIELD_UINT   SMTP2      : 3;      // bits 5-7
    };  // CR3 bitfield

    /// register _ADC1_CR3 reset value
    #define sfr_ADC1_CR3_RESET_VALUE   ((uint8_t) 0x1F)

  } CR3;


  /** ADC1 status register (SR at 0x5343) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   EOC        : 1;      // bit 0
      BITFIELD_UINT   AWD        : 1;      // bit 1
      BITFIELD_UINT   OVER       : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // SR bitfield

    /// register _ADC1_SR reset value
    #define sfr_ADC1_SR_RESET_VALUE   ((uint8_t) 0x00)

  } SR;


  /** ADC1 data register high (DRH at 0x5344) */
  union {

    /// bytewise access to DRH
    uint8_t  byte;

    /// bitwise access to register DRH
    struct {
      BITFIELD_UINT   CONV_DATA8 : 1;      // bit 0
      BITFIELD_UINT   CONV_DATA9 : 1;      // bit 1
      BITFIELD_UINT   CONV_DATA10: 1;      // bit 2
      BITFIELD_UINT   CONV_DATA11: 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // DRH bitfield

    /// register _ADC1_DRH reset value
    #define sfr_ADC1_DRH_RESET_VALUE   ((uint8_t) 0x00)

  } DRH;


  /** ADC1 data register low (DRL at 0x5345) */
  union {

    /// bytewise access to DRL
    uint8_t  byte;

    /// bitwise access to register DRL
    struct {
      BITFIELD_UINT   CONV_DATA0 : 1;      // bit 0
      BITFIELD_UINT   CONV_DATA1 : 1;      // bit 1
      BITFIELD_UINT   CONV_DATA2 : 1;      // bit 2
      BITFIELD_UINT   CONV_DATA3 : 1;      // bit 3
      BITFIELD_UINT   CONV_DATA4 : 1;      // bit 4
      BITFIELD_UINT   CONV_DATA5 : 1;      // bit 5
      BITFIELD_UINT   CONV_DATA6 : 1;      // bit 6
      BITFIELD_UINT   CONV_DATA7 : 1;      // bit 7
    };  // DRL bitfield

    /// register _ADC1_DRL reset value
    #define sfr_ADC1_DRL_RESET_VALUE   ((uint8_t) 0x00)

  } DRL;


  /** ADC1 high threshold register high (HTRH at 0x5346) */
  union {

    /// bytewise access to HTRH
    uint8_t  byte;

    /// bitwise access to register HTRH
    struct {
      BITFIELD_UINT   HT8        : 1;      // bit 0
      BITFIELD_UINT   HT9        : 1;      // bit 1
      BITFIELD_UINT   HT10       : 1;      // bit 2
      BITFIELD_UINT   HT11       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // HTRH bitfield

    /// register _ADC1_HTRH reset value
    #define sfr_ADC1_HTRH_RESET_VALUE   ((uint8_t) 0x0F)

  } HTRH;


  /** ADC1 high threshold register low (HTRL at 0x5347) */
  union {

    /// bytewise access to HTRL
    uint8_t  byte;

    /// bitwise access to register HTRL
    struct {
      BITFIELD_UINT   HT0        : 1;      // bit 0
      BITFIELD_UINT   HT1        : 1;      // bit 1
      BITFIELD_UINT   HT2        : 1;      // bit 2
      BITFIELD_UINT   HT3        : 1;      // bit 3
      BITFIELD_UINT   HT4        : 1;      // bit 4
      BITFIELD_UINT   HT5        : 1;      // bit 5
      BITFIELD_UINT   HT6        : 1;      // bit 6
      BITFIELD_UINT   HT7        : 1;      // bit 7
    };  // HTRL bitfield

    /// register _ADC1_HTRL reset value
    #define sfr_ADC1_HTRL_RESET_VALUE   ((uint8_t) 0xFF)

  } HTRL;


  /** ADC1 low threshold register high (LTRH at 0x5348) */
  union {

    /// bytewise access to LTRH
    uint8_t  byte;

    /// bitwise access to register LTRH
    struct {
      BITFIELD_UINT   LT8        : 1;      // bit 0
      BITFIELD_UINT   LT9        : 1;      // bit 1
      BITFIELD_UINT   LT10       : 1;      // bit 2
      BITFIELD_UINT   LT11       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // LTRH bitfield

    /// register _ADC1_LTRH reset value
    #define sfr_ADC1_LTRH_RESET_VALUE   ((uint8_t) 0x00)

  } LTRH;


  /** ADC1 low threshold register low (LTRL at 0x5349) */
  union {

    /// bytewise access to LTRL
    uint8_t  byte;

    /// bitwise access to register LTRL
    struct {
      BITFIELD_UINT   LT0        : 1;      // bit 0
      BITFIELD_UINT   LT1        : 1;      // bit 1
      BITFIELD_UINT   LT2        : 1;      // bit 2
      BITFIELD_UINT   LT3        : 1;      // bit 3
      BITFIELD_UINT   LT4        : 1;      // bit 4
      BITFIELD_UINT   LT5        : 1;      // bit 5
      BITFIELD_UINT   LT6        : 1;      // bit 6
      BITFIELD_UINT   LT7        : 1;      // bit 7
    };  // LTRL bitfield

    /// register _ADC1_LTRL reset value
    #define sfr_ADC1_LTRL_RESET_VALUE   ((uint8_t) 0x00)

  } LTRL;


  /** ADC1 channel sequence 1 register (SQR1 at 0x534a) */
  union {

    /// bytewise access to SQR1
    uint8_t  byte;

    /// bitwise access to register SQR1
    struct {
      BITFIELD_UINT   CHSEL_S24  : 1;      // bit 0
      BITFIELD_UINT   CHSEL_S25  : 1;      // bit 1
      BITFIELD_UINT   CHSEL_S26  : 1;      // bit 2
      BITFIELD_UINT   CHSEL_S27  : 1;      // bit 3
      BITFIELD_UINT   CHSEL_S28  : 1;      // bit 4
      BITFIELD_UINT   CHSEL_S29  : 1;      // bit 5
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   DMAOFF     : 1;      // bit 7
    };  // SQR1 bitfield

    /// register _ADC1_SQR1 reset value
    #define sfr_ADC1_SQR1_RESET_VALUE   ((uint8_t) 0x00)

  } SQR1;


  /** ADC1 channel sequence 2 register (SQR2 at 0x534b) */
  union {

    /// bytewise access to SQR2
    uint8_t  byte;

    /// bitwise access to register SQR2
    struct {
      BITFIELD_UINT   CHSEL_S16  : 1;      // bit 0
      BITFIELD_UINT   CHSEL_S17  : 1;      // bit 1
      BITFIELD_UINT   CHSEL_S18  : 1;      // bit 2
      BITFIELD_UINT   CHSEL_S19  : 1;      // bit 3
      BITFIELD_UINT   CHSEL_S20  : 1;      // bit 4
      BITFIELD_UINT   CHSEL_S21  : 1;      // bit 5
      BITFIELD_UINT   CHSEL_S22  : 1;      // bit 6
      BITFIELD_UINT   CHSEL_S23  : 1;      // bit 7
    };  // SQR2 bitfield

    /// register _ADC1_SQR2 reset value
    #define sfr_ADC1_SQR2_RESET_VALUE   ((uint8_t) 0x00)

  } SQR2;


  /** ADC1 channel sequence 3 register (SQR3 at 0x534c) */
  union {

    /// bytewise access to SQR3
    uint8_t  byte;

    /// bitwise access to register SQR3
    struct {
      BITFIELD_UINT   CHSEL_S8   : 1;      // bit 0
      BITFIELD_UINT   CHSEL_S9   : 1;      // bit 1
      BITFIELD_UINT   CHSEL_S10  : 1;      // bit 2
      BITFIELD_UINT   CHSEL_S11  : 1;      // bit 3
      BITFIELD_UINT   CHSEL_S12  : 1;      // bit 4
      BITFIELD_UINT   CHSEL_S13  : 1;      // bit 5
      BITFIELD_UINT   CHSEL_S14  : 1;      // bit 6
      BITFIELD_UINT   CHSEL_S15  : 1;      // bit 7
    };  // SQR3 bitfield

    /// register _ADC1_SQR3 reset value
    #define sfr_ADC1_SQR3_RESET_VALUE   ((uint8_t) 0x00)

  } SQR3;


  /** ADC1 channel sequence 4 register (SQR4 at 0x534d) */
  union {

    /// bytewise access to SQR4
    uint8_t  byte;

    /// bitwise access to register SQR4
    struct {
      BITFIELD_UINT   CHSEL_S0   : 1;      // bit 0
      BITFIELD_UINT   CHSEL_S1   : 1;      // bit 1
      BITFIELD_UINT   CHSEL_S2   : 1;      // bit 2
      BITFIELD_UINT   CHSEL_S3   : 1;      // bit 3
      BITFIELD_UINT   CHSEL_S4   : 1;      // bit 4
      BITFIELD_UINT   CHSEL_S5   : 1;      // bit 5
      BITFIELD_UINT   CHSEL_S6   : 1;      // bit 6
      BITFIELD_UINT   CHSEL_S7   : 1;      // bit 7
    };  // SQR4 bitfield

    /// register _ADC1_SQR4 reset value
    #define sfr_ADC1_SQR4_RESET_VALUE   ((uint8_t) 0x00)

  } SQR4;


  /** ADC1 trigger disable 1 (TRIGR1 at 0x534e) */
  union {

    /// bytewise access to TRIGR1
    uint8_t  byte;

    /// bitwise access to register TRIGR1
    struct {
      BITFIELD_UINT   TRIG24     : 1;      // bit 0
      BITFIELD_UINT   TRIG25     : 1;      // bit 1
      BITFIELD_UINT   TRIG26     : 1;      // bit 2
      BITFIELD_UINT   TRIG27     : 1;      // bit 3
      BITFIELD_UINT   VREFINTON  : 1;      // bit 4
      BITFIELD_UINT   TSON       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // TRIGR1 bitfield

    /// register _ADC1_TRIGR1 reset value
    #define sfr_ADC1_TRIGR1_RESET_VALUE   ((uint8_t) 0x00)

  } TRIGR1;


  /** ADC1 trigger disable 2 (TRIGR2 at 0x534f) */
  union {

    /// bytewise access to TRIGR2
    uint8_t  byte;

    /// bitwise access to register TRIGR2
    struct {
      BITFIELD_UINT   TRIG16     : 1;      // bit 0
      BITFIELD_UINT   TRIG17     : 1;      // bit 1
      BITFIELD_UINT   TRIG18     : 1;      // bit 2
      BITFIELD_UINT   TRIG19     : 1;      // bit 3
      BITFIELD_UINT   TRIG20     : 1;      // bit 4
      BITFIELD_UINT   TRIG21     : 1;      // bit 5
      BITFIELD_UINT   TRIG22     : 1;      // bit 6
      BITFIELD_UINT   TRIG23     : 1;      // bit 7
    };  // TRIGR2 bitfield

    /// register _ADC1_TRIGR2 reset value
    #define sfr_ADC1_TRIGR2_RESET_VALUE   ((uint8_t) 0x00)

  } TRIGR2;


  /** ADC1 trigger disable 3 (TRIGR3 at 0x5350) */
  union {

    /// bytewise access to TRIGR3
    uint8_t  byte;

    /// bitwise access to register TRIGR3
    struct {
      BITFIELD_UINT   TRIG8      : 1;      // bit 0
      BITFIELD_UINT   TRIG9      : 1;      // bit 1
      BITFIELD_UINT   TRIG10     : 1;      // bit 2
      BITFIELD_UINT   TRIG11     : 1;      // bit 3
      BITFIELD_UINT   TRIG12     : 1;      // bit 4
      BITFIELD_UINT   TRIG13     : 1;      // bit 5
      BITFIELD_UINT   TRIG14     : 1;      // bit 6
      BITFIELD_UINT   TRIG15     : 1;      // bit 7
    };  // TRIGR3 bitfield

    /// register _ADC1_TRIGR3 reset value
    #define sfr_ADC1_TRIGR3_RESET_VALUE   ((uint8_t) 0x00)

  } TRIGR3;


  /** ADC1 trigger disable 4 (TRIGR4 at 0x5351) */
  union {

    /// bytewise access to TRIGR4
    uint8_t  byte;

    /// bitwise access to register TRIGR4
    struct {
      BITFIELD_UINT   TRIG0      : 1;      // bit 0
      BITFIELD_UINT   TRIG1      : 1;      // bit 1
      BITFIELD_UINT   TRIG2      : 1;      // bit 2
      BITFIELD_UINT   TRIG3      : 1;      // bit 3
      BITFIELD_UINT   TRIG4      : 1;      // bit 4
      BITFIELD_UINT   TRIG5      : 1;      // bit 5
      BITFIELD_UINT   TRIG6      : 1;      // bit 6
      BITFIELD_UINT   TRIG7      : 1;      // bit 7
    };  // TRIGR4 bitfield

    /// register _ADC1_TRIGR4 reset value
    #define sfr_ADC1_TRIGR4_RESET_VALUE   ((uint8_t) 0x00)

  } TRIGR4;

} ADC1_t;

/// access to ADC1 SFR registers
#define sfr_ADC1   (*((ADC1_t*) 0x5340))


//------------------------
// Module BEEP
//------------------------

/** struct containing BEEP module registers */
typedef struct {

  /** BEEP control/status register 1 (CSR1 at 0x50f0) */
  union {

    /// bytewise access to CSR1
    uint8_t  byte;

    /// bitwise access to register CSR1
    struct {
      BITFIELD_UINT   MSR        : 1;      // bit 0
      BITFIELD_UINT              : 7;      // 7 bits
    };  // CSR1 bitfield

    /// register _BEEP_CSR1 reset value
    #define sfr_BEEP_CSR1_RESET_VALUE   ((uint8_t) 0x00)

  } CSR1;


  /// Reserved register (2B)
  uint8_t     Reserved_1[2];


  /** BEEP control/status register 2 (CSR2 at 0x50f3) */
  union {

    /// bytewise access to CSR2
    uint8_t  byte;

    /// bitwise access to register CSR2
    struct {
      BITFIELD_UINT   BEEPDIV    : 5;      // bits 0-4
      BITFIELD_UINT   BEEPEN     : 1;      // bit 5
      BITFIELD_UINT   BEEPSEL    : 2;      // bits 6-7
    };  // CSR2 bitfield

    /// register _BEEP_CSR2 reset value
    #define sfr_BEEP_CSR2_RESET_VALUE   ((uint8_t) 0x1F)

  } CSR2;

} BEEP_t;

/// access to BEEP SFR registers
#define sfr_BEEP   (*((BEEP_t*) 0x50f0))


//------------------------
// Module CLK
//------------------------

/** struct containing CLK module registers */
typedef struct {

  /** Clock master divider register (CKDIVR at 0x50c0) */
  union {

    /// bytewise access to CKDIVR
    uint8_t  byte;

    /// bitwise access to register CKDIVR
    struct {
      BITFIELD_UINT   CKM        : 3;      // bits 0-2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // CKDIVR bitfield

    /// register _CLK_CKDIVR reset value
    #define sfr_CLK_CKDIVR_RESET_VALUE   ((uint8_t) 0x03)

  } CKDIVR;


  /** Clock RTC register (CRTCR at 0x50c1) */
  union {

    /// bytewise access to CRTCR
    uint8_t  byte;

    /// bitwise access to register CRTCR
    struct {
      BITFIELD_UINT   RTCSWBSY   : 1;      // bit 0
      BITFIELD_UINT   RTCSEL0    : 1;      // bit 1
      BITFIELD_UINT   RTCSEL1    : 1;      // bit 2
      BITFIELD_UINT   RTCSEL2    : 1;      // bit 3
      BITFIELD_UINT   RTCSEL3    : 1;      // bit 4
      BITFIELD_UINT   RTCDIV0    : 1;      // bit 5
      BITFIELD_UINT   RTCDIV1    : 1;      // bit 6
      BITFIELD_UINT   RTCDIV2    : 1;      // bit 7
    };  // CRTCR bitfield

    /// register _CLK_CRTCR reset value
    #define sfr_CLK_CRTCR_RESET_VALUE   ((uint8_t) 0x00)

  } CRTCR;


  /** Internal clock control register (ICKCR at 0x50c2) */
  union {

    /// bytewise access to ICKCR
    uint8_t  byte;

    /// bitwise access to register ICKCR
    struct {
      BITFIELD_UINT   HSION      : 1;      // bit 0
      BITFIELD_UINT   HSIRDY     : 1;      // bit 1
      BITFIELD_UINT   LSION      : 1;      // bit 2
      BITFIELD_UINT   LSIRDY     : 1;      // bit 3
      BITFIELD_UINT   SAHALT     : 1;      // bit 4
      BITFIELD_UINT   FHWU       : 1;      // bit 5
      BITFIELD_UINT   BEEPAHALT  : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // ICKCR bitfield

    /// register _CLK_ICKCR reset value
    #define sfr_CLK_ICKCR_RESET_VALUE   ((uint8_t) 0x11)

  } ICKCR;


  /** Peripheral clock gating register 1 (PCKENR1 at 0x50c3) */
  union {

    /// bytewise access to PCKENR1
    uint8_t  byte;

    /// bitwise access to register PCKENR1
    struct {
      BITFIELD_UINT   PCKEN10    : 1;      // bit 0
      BITFIELD_UINT   PCKEN11    : 1;      // bit 1
      BITFIELD_UINT   PCKEN12    : 1;      // bit 2
      BITFIELD_UINT   PCKEN13    : 1;      // bit 3
      BITFIELD_UINT   PCKEN14    : 1;      // bit 4
      BITFIELD_UINT   PCKEN15    : 1;      // bit 5
      BITFIELD_UINT   PCKEN16    : 1;      // bit 6
      BITFIELD_UINT   PCKEN17    : 1;      // bit 7
    };  // PCKENR1 bitfield

    /// register _CLK_PCKENR1 reset value
    #define sfr_CLK_PCKENR1_RESET_VALUE   ((uint8_t) 0x00)

  } PCKENR1;


  /** Peripheral clock gating register 2 (PCKENR2 at 0x50c4) */
  union {

    /// bytewise access to PCKENR2
    uint8_t  byte;

    /// bitwise access to register PCKENR2
    struct {
      BITFIELD_UINT   PCKEN20    : 1;      // bit 0
      BITFIELD_UINT   PCKEN21    : 1;      // bit 1
      BITFIELD_UINT   PCKEN22    : 1;      // bit 2
      BITFIELD_UINT   PCKEN23    : 1;      // bit 3
      BITFIELD_UINT   PCKEN24    : 1;      // bit 4
      BITFIELD_UINT   PCKEN25    : 1;      // bit 5
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   PCKEN27    : 1;      // bit 7
    };  // PCKENR2 bitfield

    /// register _CLK_PCKENR2 reset value
    #define sfr_CLK_PCKENR2_RESET_VALUE   ((uint8_t) 0x00)

  } PCKENR2;


  /** Configurable clock control register (CCOR at 0x50c5) */
  union {

    /// bytewise access to CCOR
    uint8_t  byte;

    /// bitwise access to register CCOR
    struct {
      BITFIELD_UINT   CCOSWBSY   : 1;      // bit 0
      BITFIELD_UINT   CCOSEL     : 4;      // bits 1-4
      BITFIELD_UINT   CCODIV     : 3;      // bits 5-7
    };  // CCOR bitfield

    /// register _CLK_CCOR reset value
    #define sfr_CLK_CCOR_RESET_VALUE   ((uint8_t) 0x00)

  } CCOR;


  /** External clock control register (ECKCR at 0x50c6) */
  union {

    /// bytewise access to ECKCR
    uint8_t  byte;

    /// bitwise access to register ECKCR
    struct {
      BITFIELD_UINT   HSEON      : 1;      // bit 0
      BITFIELD_UINT   HSERDY     : 1;      // bit 1
      BITFIELD_UINT   LSEON      : 1;      // bit 2
      BITFIELD_UINT   LSERDY     : 1;      // bit 3
      BITFIELD_UINT   HSEBYP     : 1;      // bit 4
      BITFIELD_UINT   LSEBYP     : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // ECKCR bitfield

    /// register _CLK_ECKCR reset value
    #define sfr_CLK_ECKCR_RESET_VALUE   ((uint8_t) 0x00)

  } ECKCR;


  /** System clock status register (SCSR at 0x50c7) */
  union {

    /// bytewise access to SCSR
    uint8_t  byte;

    /// bitwise access to register SCSR
    struct {
      BITFIELD_UINT   CKM        : 8;      // bits 0-7
    };  // SCSR bitfield

    /// register _CLK_SCSR reset value
    #define sfr_CLK_SCSR_RESET_VALUE   ((uint8_t) 0x01)

  } SCSR;


  /** System clock switch register (SWR at 0x50c8) */
  union {

    /// bytewise access to SWR
    uint8_t  byte;

    /// bitwise access to register SWR
    struct {
      BITFIELD_UINT   SWI        : 8;      // bits 0-7
    };  // SWR bitfield

    /// register _CLK_SWR reset value
    #define sfr_CLK_SWR_RESET_VALUE   ((uint8_t) 0x01)

  } SWR;


  /** Clock switch control register (SWCR at 0x50c9) */
  union {

    /// bytewise access to SWCR
    uint8_t  byte;

    /// bitwise access to register SWCR
    struct {
      BITFIELD_UINT   SWBSY      : 1;      // bit 0
      BITFIELD_UINT   SWEN       : 1;      // bit 1
      BITFIELD_UINT   SWIEN      : 1;      // bit 2
      BITFIELD_UINT   SWIF       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // SWCR bitfield

    /// register _CLK_SWCR reset value
    #define sfr_CLK_SWCR_RESET_VALUE   ((uint8_t) 0x00)

  } SWCR;


  /** Clock security system register (CSSR at 0x50ca) */
  union {

    /// bytewise access to CSSR
    uint8_t  byte;

    /// bitwise access to register CSSR
    struct {
      BITFIELD_UINT   CSSEN      : 1;      // bit 0
      BITFIELD_UINT   AUX        : 1;      // bit 1
      BITFIELD_UINT   CSSDIE     : 1;      // bit 2
      BITFIELD_UINT   CSSD       : 1;      // bit 3
      BITFIELD_UINT   CSSDGON    : 1;      // bit 4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // CSSR bitfield

    /// register _CLK_CSSR reset value
    #define sfr_CLK_CSSR_RESET_VALUE   ((uint8_t) 0x00)

  } CSSR;


  /** Clock BEEP register (CBEEPR at 0x50cb) */
  union {

    /// bytewise access to CBEEPR
    uint8_t  byte;

    /// bitwise access to register CBEEPR
    struct {
      BITFIELD_UINT   BEEPSWBSY  : 1;      // bit 0
      BITFIELD_UINT   CLKBEEPSEL0: 1;      // bit 1
      BITFIELD_UINT   CLKBEEPSEL1: 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // CBEEPR bitfield

    /// register _CLK_CBEEPR reset value
    #define sfr_CLK_CBEEPR_RESET_VALUE   ((uint8_t) 0x00)

  } CBEEPR;


  /** HSI calibration register (HSICALR at 0x50cc) */
  union {

    /// bytewise access to HSICALR
    uint8_t  byte;

    /// bitwise access to register HSICALR
    struct {
      BITFIELD_UINT   HSICAL     : 8;      // bits 0-7
    };  // HSICALR bitfield

    /// register _CLK_HSICALR reset value
    #define sfr_CLK_HSICALR_RESET_VALUE   ((uint8_t) 0x00)

  } HSICALR;


  /** HSI clock calibration trimming register (HSITRIMR at 0x50cd) */
  union {

    /// bytewise access to HSITRIMR
    uint8_t  byte;

    /// bitwise access to register HSITRIMR
    struct {
      BITFIELD_UINT   HSITRIM    : 8;      // bits 0-7
    };  // HSITRIMR bitfield

    /// register _CLK_HSITRIMR reset value
    #define sfr_CLK_HSITRIMR_RESET_VALUE   ((uint8_t) 0x00)

  } HSITRIMR;


  /** HSI unlock register (HSIUNLCKR at 0x50ce) */
  union {

    /// bytewise access to HSIUNLCKR
    uint8_t  byte;

    /// bitwise access to register HSIUNLCKR
    struct {
      BITFIELD_UINT   HSIUNLCK   : 8;      // bits 0-7
    };  // HSIUNLCKR bitfield

    /// register _CLK_HSIUNLCKR reset value
    #define sfr_CLK_HSIUNLCKR_RESET_VALUE   ((uint8_t) 0x00)

  } HSIUNLCKR;


  /** Main regulator control status register (REGCSR at 0x50cf) */
  union {

    /// bytewise access to REGCSR
    uint8_t  byte;

    /// bitwise access to register REGCSR
    struct {
      BITFIELD_UINT   REGREADY   : 1;      // bit 0
      BITFIELD_UINT   REGOFF     : 1;      // bit 1
      BITFIELD_UINT   HSIPD      : 1;      // bit 2
      BITFIELD_UINT   LSIPD      : 1;      // bit 3
      BITFIELD_UINT   HSEPD      : 1;      // bit 4
      BITFIELD_UINT   LSEPD      : 1;      // bit 5
      BITFIELD_UINT   EEBUSY     : 1;      // bit 6
      BITFIELD_UINT   EEREADY    : 1;      // bit 7
    };  // REGCSR bitfield

    /// register _CLK_REGCSR reset value
    #define sfr_CLK_REGCSR_RESET_VALUE   ((uint8_t) 0x38)

  } REGCSR;


  /** Peripheral clock gating register 3 (PCKENR3 at 0x50d0) */
  union {

    /// bytewise access to PCKENR3
    uint8_t  byte;

    /// bitwise access to register PCKENR3
    struct {
      BITFIELD_UINT   PCKEN30    : 1;      // bit 0
      BITFIELD_UINT   PCKEN31    : 1;      // bit 1
      BITFIELD_UINT   PCKEN32    : 1;      // bit 2
      BITFIELD_UINT   PCKEN33    : 1;      // bit 3
      BITFIELD_UINT   PCKEN34    : 1;      // bit 4
      BITFIELD_UINT   PCKEN35    : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // PCKENR3 bitfield

    /// register _CLK_PCKENR3 reset value
    #define sfr_CLK_PCKENR3_RESET_VALUE   ((uint8_t) 0x00)

  } PCKENR3;

} CLK_t;

/// access to CLK SFR registers
#define sfr_CLK   (*((CLK_t*) 0x50c0))


//------------------------
// Module COMP1_2
//------------------------

/** struct containing COMP1_2 module registers */
typedef struct {

  /** Comparator control and status register 1 (CSR1 at 0x5440) */
  union {

    /// bytewise access to CSR1
    uint8_t  byte;

    /// bitwise access to register CSR1
    struct {
      BITFIELD_UINT   CMP1       : 2;      // bits 0-1
      BITFIELD_UINT   STE        : 1;      // bit 2
      BITFIELD_UINT   CMP1OUT    : 1;      // bit 3
      BITFIELD_UINT   EF1        : 1;      // bit 4
      BITFIELD_UINT   IE1        : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CSR1 bitfield

    /// register _COMP1_2_CSR1 reset value
    #define sfr_COMP1_2_CSR1_RESET_VALUE   ((uint8_t) 0x00)

  } CSR1;


  /** Comparator control and status register 2 (CSR2 at 0x5441) */
  union {

    /// bytewise access to CSR2
    uint8_t  byte;

    /// bitwise access to register CSR2
    struct {
      BITFIELD_UINT   CMP2       : 2;      // bits 0-1
      BITFIELD_UINT   SPEED      : 1;      // bit 2
      BITFIELD_UINT   CMP2OUT    : 1;      // bit 3
      BITFIELD_UINT   EF2        : 1;      // bit 4
      BITFIELD_UINT   IE2        : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CSR2 bitfield

    /// register _COMP1_2_CSR2 reset value
    #define sfr_COMP1_2_CSR2_RESET_VALUE   ((uint8_t) 0x00)

  } CSR2;


  /** Comparator control and status register 3 (CSR3 at 0x5442) */
  union {

    /// bytewise access to CSR3
    uint8_t  byte;

    /// bitwise access to register CSR3
    struct {
      BITFIELD_UINT   VREFOUTEN  : 1;      // bit 0
      BITFIELD_UINT   WNDWE      : 1;      // bit 1
      BITFIELD_UINT   VREFEN     : 1;      // bit 2
      BITFIELD_UINT   INSEL      : 3;      // bits 3-5
      BITFIELD_UINT   OUTSEL     : 2;      // bits 6-7
    };  // CSR3 bitfield

    /// register _COMP1_2_CSR3 reset value
    #define sfr_COMP1_2_CSR3_RESET_VALUE   ((uint8_t) 0x00)

  } CSR3;


  /** Comparator control and status register 4 (CSR4 at 0x5443) */
  union {

    /// bytewise access to CSR4
    uint8_t  byte;

    /// bitwise access to register CSR4
    struct {
      BITFIELD_UINT   INVTRIG    : 3;      // bits 0-2
      BITFIELD_UINT   NINVTRIG   : 3;      // bits 3-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CSR4 bitfield

    /// register _COMP1_2_CSR4 reset value
    #define sfr_COMP1_2_CSR4_RESET_VALUE   ((uint8_t) 0x00)

  } CSR4;


  /** Comparator control and status register 5 (CSR5 at 0x5444) */
  union {

    /// bytewise access to CSR5
    uint8_t  byte;

    /// bitwise access to register CSR5
    struct {
      BITFIELD_UINT   VREFTRIG   : 3;      // bits 0-2
      BITFIELD_UINT   DACTRIG    : 3;      // bits 3-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CSR5 bitfield

    /// register _COMP1_2_CSR5 reset value
    #define sfr_COMP1_2_CSR5_RESET_VALUE   ((uint8_t) 0x00)

  } CSR5;

} COMP1_2_t;

/// access to COMP1_2 SFR registers
#define sfr_COMP1_2   (*((COMP1_2_t*) 0x5440))


//------------------------
// Module CPU
//------------------------

/** struct containing CPU module registers */
typedef struct {

  /** Accumulator (A at 0x7f00) */
  union {

    /// bytewise access to A
    uint8_t  byte;

    /// skip bitwise access to register A

    /// register _CPU_A reset value
    #define sfr_CPU_A_RESET_VALUE   ((uint8_t) 0x00)

  } A;


  /** Program counter extended (PCE at 0x7f01) */
  union {

    /// bytewise access to PCE
    uint8_t  byte;

    /// skip bitwise access to register PCE

    /// register _CPU_PCE reset value
    #define sfr_CPU_PCE_RESET_VALUE   ((uint8_t) 0x00)

  } PCE;


  /** Program counter high (PCH at 0x7f02) */
  union {

    /// bytewise access to PCH
    uint8_t  byte;

    /// skip bitwise access to register PCH

    /// register _CPU_PCH reset value
    #define sfr_CPU_PCH_RESET_VALUE   ((uint8_t) 0x00)

  } PCH;


  /** Program counter low (PCL at 0x7f03) */
  union {

    /// bytewise access to PCL
    uint8_t  byte;

    /// skip bitwise access to register PCL

    /// register _CPU_PCL reset value
    #define sfr_CPU_PCL_RESET_VALUE   ((uint8_t) 0x00)

  } PCL;


  /** X index register high (XH at 0x7f04) */
  union {

    /// bytewise access to XH
    uint8_t  byte;

    /// skip bitwise access to register XH

    /// register _CPU_XH reset value
    #define sfr_CPU_XH_RESET_VALUE   ((uint8_t) 0x00)

  } XH;


  /** X index register low (XL at 0x7f05) */
  union {

    /// bytewise access to XL
    uint8_t  byte;

    /// skip bitwise access to register XL

    /// register _CPU_XL reset value
    #define sfr_CPU_XL_RESET_VALUE   ((uint8_t) 0x00)

  } XL;


  /** Y index register high (YH at 0x7f06) */
  union {

    /// bytewise access to YH
    uint8_t  byte;

    /// skip bitwise access to register YH

    /// register _CPU_YH reset value
    #define sfr_CPU_YH_RESET_VALUE   ((uint8_t) 0x00)

  } YH;


  /** Y index register low (YL at 0x7f07) */
  union {

    /// bytewise access to YL
    uint8_t  byte;

    /// skip bitwise access to register YL

    /// register _CPU_YL reset value
    #define sfr_CPU_YL_RESET_VALUE   ((uint8_t) 0x00)

  } YL;


  /** Stack pointer high (SPH at 0x7f08) */
  union {

    /// bytewise access to SPH
    uint8_t  byte;

    /// skip bitwise access to register SPH

    /// register _CPU_SPH reset value
    #define sfr_CPU_SPH_RESET_VALUE   ((uint8_t) 0x03)

  } SPH;


  /** Stack pointer low (SPL at 0x7f09) */
  union {

    /// bytewise access to SPL
    uint8_t  byte;

    /// skip bitwise access to register SPL

    /// register _CPU_SPL reset value
    #define sfr_CPU_SPL_RESET_VALUE   ((uint8_t) 0xFF)

  } SPL;


  /** Condition code register (CCR at 0x7f0a) */
  union {

    /// bytewise access to CCR
    uint8_t  byte;

    /// bitwise access to register CCR
    struct {
      BITFIELD_UINT   C          : 1;      // bit 0
      BITFIELD_UINT   Z          : 1;      // bit 1
      BITFIELD_UINT   N          : 1;      // bit 2
      BITFIELD_UINT   I0         : 1;      // bit 3
      BITFIELD_UINT   H          : 1;      // bit 4
      BITFIELD_UINT   I1         : 1;      // bit 5
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   V          : 1;      // bit 7
    };  // CCR bitfield

    /// register _CPU_CCR reset value
    #define sfr_CPU_CCR_RESET_VALUE   ((uint8_t) 0x28)

  } CCR;


  /// Reserved register (85B)
  uint8_t     Reserved_1[85];


  /** Global configuration register (CFG_GCR at 0x7f60) */
  union {

    /// bytewise access to CFG_GCR
    uint8_t  byte;

    /// bitwise access to register CFG_GCR
    struct {
      BITFIELD_UINT   SWD        : 1;      // bit 0
      BITFIELD_UINT   AL         : 1;      // bit 1
      BITFIELD_UINT              : 6;      // 6 bits
    };  // CFG_GCR bitfield

    /// register _CPU_CFG_GCR reset value
    #define sfr_CPU_CFG_GCR_RESET_VALUE   ((uint8_t) 0x00)

  } CFG_GCR;

} CPU_t;

/// access to CPU SFR registers
#define sfr_CPU   (*((CPU_t*) 0x7f00))


//------------------------
// Module DAC
//------------------------

/** struct containing DAC module registers */
typedef struct {

  /** DAC channel 1 control register 1 (CH1CR1 at 0x5380) */
  union {

    /// bytewise access to CH1CR1
    uint8_t  byte;

    /// bitwise access to register CH1CR1
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   BOFF       : 1;      // bit 1
      BITFIELD_UINT   TEN        : 1;      // bit 2
      BITFIELD_UINT   TSEL       : 3;      // bits 3-5
      BITFIELD_UINT   WAVEN      : 2;      // bits 6-7
    };  // CH1CR1 bitfield

    /// register _DAC_CH1CR1 reset value
    #define sfr_DAC_CH1CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CH1CR1;


  /** DAC channel 1 control register 2 (CH1CR2 at 0x5381) */
  union {

    /// bytewise access to CH1CR2
    uint8_t  byte;

    /// bitwise access to register CH1CR2
    struct {
      BITFIELD_UINT   MAMP       : 4;      // bits 0-3
      BITFIELD_UINT   DMAEN      : 1;      // bit 4
      BITFIELD_UINT   DMAUDRIE   : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CH1CR2 bitfield

    /// register _DAC_CH1CR2 reset value
    #define sfr_DAC_CH1CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CH1CR2;


  /** DAC channel 2 control register 1 (CH2CR1 at 0x5382) */
  union {

    /// bytewise access to CH2CR1
    uint8_t  byte;

    /// bitwise access to register CH2CR1
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   BOFF       : 1;      // bit 1
      BITFIELD_UINT   TEN        : 1;      // bit 2
      BITFIELD_UINT   TSEL       : 3;      // bits 3-5
      BITFIELD_UINT   WAVEN      : 2;      // bits 6-7
    };  // CH2CR1 bitfield

    /// register _DAC_CH2CR1 reset value
    #define sfr_DAC_CH2CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CH2CR1;


  /** DAC channel 2 control register 2 (CH2CR2 at 0x5383) */
  union {

    /// bytewise access to CH2CR2
    uint8_t  byte;

    /// bitwise access to register CH2CR2
    struct {
      BITFIELD_UINT   MAMP       : 4;      // bits 0-3
      BITFIELD_UINT   DMAEN      : 1;      // bit 4
      BITFIELD_UINT   DMAUDRIE   : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CH2CR2 bitfield

    /// register _DAC_CH2CR2 reset value
    #define sfr_DAC_CH2CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CH2CR2;


  /** DAC software trigger register (SWTRIG at 0x5384) */
  union {

    /// bytewise access to SWTRIG
    uint8_t  byte;

    /// bitwise access to register SWTRIG
    struct {
      BITFIELD_UINT   SWTRIG1    : 1;      // bit 0
      BITFIELD_UINT   SWTRIG2    : 1;      // bit 1
      BITFIELD_UINT              : 6;      // 6 bits
    };  // SWTRIG bitfield

    /// register _DAC_SWTRIG reset value
    #define sfr_DAC_SWTRIG_RESET_VALUE   ((uint8_t) 0x00)

  } SWTRIG;


  /** DAC status register (SR at 0x5385) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   DMAUDR1    : 1;      // bit 0
      BITFIELD_UINT   DMAUDR2    : 1;      // bit 1
      BITFIELD_UINT              : 6;      // 6 bits
    };  // SR bitfield

    /// register _DAC_SR reset value
    #define sfr_DAC_SR_RESET_VALUE   ((uint8_t) 0x00)

  } SR;


  /// Reserved register (2B)
  uint8_t     Reserved_1[2];


  /** DAC channel 1 right aligned data holding register high (CH1RDHRH at 0x5388) */
  union {

    /// bytewise access to CH1RDHRH
    uint8_t  byte;

    /// bitwise access to register CH1RDHRH
    struct {
      BITFIELD_UINT   RDHRH00    : 1;      // bit 0
      BITFIELD_UINT   RDHRH10    : 1;      // bit 1
      BITFIELD_UINT   RDHRH20    : 1;      // bit 2
      BITFIELD_UINT   RDHRH30    : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CH1RDHRH bitfield

    /// register _DAC_CH1RDHRH reset value
    #define sfr_DAC_CH1RDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } CH1RDHRH;


  /** DAC channel 1 right aligned data holding register low (CH1RDHRL at 0x5389) */
  union {

    /// bytewise access to CH1RDHRL
    uint8_t  byte;

    /// bitwise access to register CH1RDHRL
    struct {
      BITFIELD_UINT   RDHRL00    : 1;      // bit 0
      BITFIELD_UINT   RDHRL10    : 1;      // bit 1
      BITFIELD_UINT   RDHRL20    : 1;      // bit 2
      BITFIELD_UINT   RDHRL30    : 1;      // bit 3
      BITFIELD_UINT   RDHRL40    : 1;      // bit 4
      BITFIELD_UINT   RDHRL50    : 1;      // bit 5
      BITFIELD_UINT   RDHRL60    : 1;      // bit 6
      BITFIELD_UINT   RDHRL70    : 1;      // bit 7
    };  // CH1RDHRL bitfield

    /// register _DAC_CH1RDHRL reset value
    #define sfr_DAC_CH1RDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } CH1RDHRL;


  /// Reserved register (2B)
  uint8_t     Reserved_2[2];


  /** DAC channel 1 left aligned data holding register high (CH1LDHRH at 0x538c) */
  union {

    /// bytewise access to CH1LDHRH
    uint8_t  byte;

    /// bitwise access to register CH1LDHRH
    struct {
      BITFIELD_UINT   LDHRH00    : 1;      // bit 0
      BITFIELD_UINT   LDHRH10    : 1;      // bit 1
      BITFIELD_UINT   LDHRH20    : 1;      // bit 2
      BITFIELD_UINT   LDHRH30    : 1;      // bit 3
      BITFIELD_UINT   LDHRH40    : 1;      // bit 4
      BITFIELD_UINT   LDHRH50    : 1;      // bit 5
      BITFIELD_UINT   LDHRH60    : 1;      // bit 6
      BITFIELD_UINT   LDHRH70    : 1;      // bit 7
    };  // CH1LDHRH bitfield

    /// register _DAC_CH1LDHRH reset value
    #define sfr_DAC_CH1LDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } CH1LDHRH;


  /** DAC channel 1 left aligned data holding register low (CH1LDHRL at 0x538d) */
  union {

    /// bytewise access to CH1LDHRL
    uint8_t  byte;

    /// bitwise access to register CH1LDHRL
    struct {
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   LDHRL4     : 1;      // bit 4
      BITFIELD_UINT   LDHRL5     : 1;      // bit 5
      BITFIELD_UINT   LDHRL6     : 1;      // bit 6
      BITFIELD_UINT   LDHRL7     : 1;      // bit 7
    };  // CH1LDHRL bitfield

    /// register _DAC_CH1LDHRL reset value
    #define sfr_DAC_CH1LDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } CH1LDHRL;


  /// Reserved register (2B)
  uint8_t     Reserved_3[2];


  /** DAC channel 1 8-bit data holding register (CH1DHR8 at 0x5390) */
  union {

    /// bytewise access to CH1DHR8
    uint8_t  byte;

    /// bitwise access to register CH1DHR8
    struct {
      BITFIELD_UINT   DHR00      : 1;      // bit 0
      BITFIELD_UINT   DHR10      : 1;      // bit 1
      BITFIELD_UINT   DHR20      : 1;      // bit 2
      BITFIELD_UINT   DHR30      : 1;      // bit 3
      BITFIELD_UINT   DHR40      : 1;      // bit 4
      BITFIELD_UINT   DHR50      : 1;      // bit 5
      BITFIELD_UINT   DHR60      : 1;      // bit 6
      BITFIELD_UINT   DHR70      : 1;      // bit 7
    };  // CH1DHR8 bitfield

    /// register _DAC_CH1DHR8 reset value
    #define sfr_DAC_CH1DHR8_RESET_VALUE   ((uint8_t) 0x00)

  } CH1DHR8;


  /// Reserved register (3B)
  uint8_t     Reserved_4[3];


  /** DAC channel 2 right aligned data holding register high (CH2RDHRH at 0x5394) */
  union {

    /// bytewise access to CH2RDHRH
    uint8_t  byte;

    /// bitwise access to register CH2RDHRH
    struct {
      BITFIELD_UINT   RDHRH00    : 1;      // bit 0
      BITFIELD_UINT   RDHRH10    : 1;      // bit 1
      BITFIELD_UINT   RDHRH20    : 1;      // bit 2
      BITFIELD_UINT   RDHRH30    : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CH2RDHRH bitfield

    /// register _DAC_CH2RDHRH reset value
    #define sfr_DAC_CH2RDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } CH2RDHRH;


  /** DAC channel 2 right aligned data holding register low (CH2RDHRL at 0x5395) */
  union {

    /// bytewise access to CH2RDHRL
    uint8_t  byte;

    /// bitwise access to register CH2RDHRL
    struct {
      BITFIELD_UINT   RDHRL00    : 1;      // bit 0
      BITFIELD_UINT   RDHRL10    : 1;      // bit 1
      BITFIELD_UINT   RDHRL20    : 1;      // bit 2
      BITFIELD_UINT   RDHRL30    : 1;      // bit 3
      BITFIELD_UINT   RDHRL40    : 1;      // bit 4
      BITFIELD_UINT   RDHRL50    : 1;      // bit 5
      BITFIELD_UINT   RDHRL60    : 1;      // bit 6
      BITFIELD_UINT   RDHRL70    : 1;      // bit 7
    };  // CH2RDHRL bitfield

    /// register _DAC_CH2RDHRL reset value
    #define sfr_DAC_CH2RDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } CH2RDHRL;


  /// Reserved register (2B)
  uint8_t     Reserved_5[2];


  /** DAC channel 2 left aligned data holding register high (CH2LDHRH at 0x5398) */
  union {

    /// bytewise access to CH2LDHRH
    uint8_t  byte;

    /// bitwise access to register CH2LDHRH
    struct {
      BITFIELD_UINT   LDHRH00    : 1;      // bit 0
      BITFIELD_UINT   LDHRH10    : 1;      // bit 1
      BITFIELD_UINT   LDHRH20    : 1;      // bit 2
      BITFIELD_UINT   LDHRH30    : 1;      // bit 3
      BITFIELD_UINT   LDHRH40    : 1;      // bit 4
      BITFIELD_UINT   LDHRH50    : 1;      // bit 5
      BITFIELD_UINT   LDHRH60    : 1;      // bit 6
      BITFIELD_UINT   LDHRH70    : 1;      // bit 7
    };  // CH2LDHRH bitfield

    /// register _DAC_CH2LDHRH reset value
    #define sfr_DAC_CH2LDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } CH2LDHRH;


  /** DAC channel 2 left aligned data holding register low (CH2LDHRL at 0x5399) */
  union {

    /// bytewise access to CH2LDHRL
    uint8_t  byte;

    /// bitwise access to register CH2LDHRL
    struct {
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   LDHRL4     : 1;      // bit 4
      BITFIELD_UINT   LDHRL5     : 1;      // bit 5
      BITFIELD_UINT   LDHRL6     : 1;      // bit 6
      BITFIELD_UINT   LDHRL7     : 1;      // bit 7
    };  // CH2LDHRL bitfield

    /// register _DAC_CH2LDHRL reset value
    #define sfr_DAC_CH2LDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } CH2LDHRL;


  /// Reserved register (2B)
  uint8_t     Reserved_6[2];


  /** DAC channel 2 8-bit data holding register (CH2DHR8 at 0x539c) */
  union {

    /// bytewise access to CH2DHR8
    uint8_t  byte;

    /// bitwise access to register CH2DHR8
    struct {
      BITFIELD_UINT   DHR00      : 1;      // bit 0
      BITFIELD_UINT   DHR10      : 1;      // bit 1
      BITFIELD_UINT   DHR20      : 1;      // bit 2
      BITFIELD_UINT   DHR30      : 1;      // bit 3
      BITFIELD_UINT   DHR40      : 1;      // bit 4
      BITFIELD_UINT   DHR50      : 1;      // bit 5
      BITFIELD_UINT   DHR60      : 1;      // bit 6
      BITFIELD_UINT   DHR70      : 1;      // bit 7
    };  // CH2DHR8 bitfield

    /// register _DAC_CH2DHR8 reset value
    #define sfr_DAC_CH2DHR8_RESET_VALUE   ((uint8_t) 0x00)

  } CH2DHR8;


  /// Reserved register (3B)
  uint8_t     Reserved_7[3];


  /** DAC channel 1 right aligned data holding register high (DCH1RDHRH at 0x53a0) */
  union {

    /// bytewise access to DCH1RDHRH
    uint8_t  byte;

    /// bitwise access to register DCH1RDHRH
    struct {
      BITFIELD_UINT   RDHRH00    : 1;      // bit 0
      BITFIELD_UINT   RDHRH10    : 1;      // bit 1
      BITFIELD_UINT   RDHRH20    : 1;      // bit 2
      BITFIELD_UINT   RDHRH30    : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // DCH1RDHRH bitfield

    /// register _DAC_DCH1RDHRH reset value
    #define sfr_DAC_DCH1RDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } DCH1RDHRH;


  /** DAC channel 1 right aligned data holding register low (DCH1RDHRL at 0x53a1) */
  union {

    /// bytewise access to DCH1RDHRL
    uint8_t  byte;

    /// bitwise access to register DCH1RDHRL
    struct {
      BITFIELD_UINT   RDHRL00    : 1;      // bit 0
      BITFIELD_UINT   RDHRL10    : 1;      // bit 1
      BITFIELD_UINT   RDHRL20    : 1;      // bit 2
      BITFIELD_UINT   RDHRL30    : 1;      // bit 3
      BITFIELD_UINT   RDHRL40    : 1;      // bit 4
      BITFIELD_UINT   RDHRL50    : 1;      // bit 5
      BITFIELD_UINT   RDHRL60    : 1;      // bit 6
      BITFIELD_UINT   RDHRL70    : 1;      // bit 7
    };  // DCH1RDHRL bitfield

    /// register _DAC_DCH1RDHRL reset value
    #define sfr_DAC_DCH1RDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } DCH1RDHRL;


  /** DAC channel 2 right aligned data holding register high (DCH2RDHRH at 0x53a2) */
  union {

    /// bytewise access to DCH2RDHRH
    uint8_t  byte;

    /// bitwise access to register DCH2RDHRH
    struct {
      BITFIELD_UINT   RDHRH00    : 1;      // bit 0
      BITFIELD_UINT   RDHRH10    : 1;      // bit 1
      BITFIELD_UINT   RDHRH20    : 1;      // bit 2
      BITFIELD_UINT   RDHRH30    : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // DCH2RDHRH bitfield

    /// register _DAC_DCH2RDHRH reset value
    #define sfr_DAC_DCH2RDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } DCH2RDHRH;


  /** DAC channel 2 right aligned data holding register low (DCH2RDHRL at 0x53a3) */
  union {

    /// bytewise access to DCH2RDHRL
    uint8_t  byte;

    /// bitwise access to register DCH2RDHRL
    struct {
      BITFIELD_UINT   RDHRL00    : 1;      // bit 0
      BITFIELD_UINT   RDHRL10    : 1;      // bit 1
      BITFIELD_UINT   RDHRL20    : 1;      // bit 2
      BITFIELD_UINT   RDHRL30    : 1;      // bit 3
      BITFIELD_UINT   RDHRL40    : 1;      // bit 4
      BITFIELD_UINT   RDHRL50    : 1;      // bit 5
      BITFIELD_UINT   RDHRL60    : 1;      // bit 6
      BITFIELD_UINT   RDHRL70    : 1;      // bit 7
    };  // DCH2RDHRL bitfield

    /// register _DAC_DCH2RDHRL reset value
    #define sfr_DAC_DCH2RDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } DCH2RDHRL;


  /** DAC channel 1left aligned data holding register high (DCH1LDHRH at 0x53a4) */
  union {

    /// bytewise access to DCH1LDHRH
    uint8_t  byte;

    /// bitwise access to register DCH1LDHRH
    struct {
      BITFIELD_UINT   LDHRH00    : 1;      // bit 0
      BITFIELD_UINT   LDHRH10    : 1;      // bit 1
      BITFIELD_UINT   LDHRH20    : 1;      // bit 2
      BITFIELD_UINT   LDHRH30    : 1;      // bit 3
      BITFIELD_UINT   LDHRH40    : 1;      // bit 4
      BITFIELD_UINT   LDHRH50    : 1;      // bit 5
      BITFIELD_UINT   LDHRH60    : 1;      // bit 6
      BITFIELD_UINT   LDHRH70    : 1;      // bit 7
    };  // DCH1LDHRH bitfield

    /// register _DAC_DCH1LDHRH reset value
    #define sfr_DAC_DCH1LDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } DCH1LDHRH;


  /** DAC channel 1left aligned data holding register low (DCH1LDHRL at 0x53a5) */
  union {

    /// bytewise access to DCH1LDHRL
    uint8_t  byte;

    /// bitwise access to register DCH1LDHRL
    struct {
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   LDHRL40    : 1;      // bit 4
      BITFIELD_UINT   LDHRL50    : 1;      // bit 5
      BITFIELD_UINT   LDHRL60    : 1;      // bit 6
      BITFIELD_UINT   LDHRL70    : 1;      // bit 7
    };  // DCH1LDHRL bitfield

    /// register _DAC_DCH1LDHRL reset value
    #define sfr_DAC_DCH1LDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } DCH1LDHRL;


  /** DAC channel 2 left aligned data holding register high (DCH2LDHRH at 0x53a6) */
  union {

    /// bytewise access to DCH2LDHRH
    uint8_t  byte;

    /// bitwise access to register DCH2LDHRH
    struct {
      BITFIELD_UINT   LDHRH00    : 1;      // bit 0
      BITFIELD_UINT   LDHRH10    : 1;      // bit 1
      BITFIELD_UINT   LDHRH20    : 1;      // bit 2
      BITFIELD_UINT   LDHRH30    : 1;      // bit 3
      BITFIELD_UINT   LDHRH40    : 1;      // bit 4
      BITFIELD_UINT   LDHRH50    : 1;      // bit 5
      BITFIELD_UINT   LDHRH60    : 1;      // bit 6
      BITFIELD_UINT   LDHRH70    : 1;      // bit 7
    };  // DCH2LDHRH bitfield

    /// register _DAC_DCH2LDHRH reset value
    #define sfr_DAC_DCH2LDHRH_RESET_VALUE   ((uint8_t) 0x00)

  } DCH2LDHRH;


  /** DAC channel 2 left aligned data holding register low (DCH2LDHRL at 0x53a7) */
  union {

    /// bytewise access to DCH2LDHRL
    uint8_t  byte;

    /// bitwise access to register DCH2LDHRL
    struct {
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   LDHRL40    : 1;      // bit 4
      BITFIELD_UINT   LDHRL50    : 1;      // bit 5
      BITFIELD_UINT   LDHRL60    : 1;      // bit 6
      BITFIELD_UINT   LDHRL70    : 1;      // bit 7
    };  // DCH2LDHRL bitfield

    /// register _DAC_DCH2LDHRL reset value
    #define sfr_DAC_DCH2LDHRL_RESET_VALUE   ((uint8_t) 0x00)

  } DCH2LDHRL;


  /** DAC channel 1 8-bit mode data holding register (DCH1DHR8 at 0x53a8) */
  union {

    /// bytewise access to DCH1DHR8
    uint8_t  byte;

    /// bitwise access to register DCH1DHR8
    struct {
      BITFIELD_UINT   DHR00      : 1;      // bit 0
      BITFIELD_UINT   DHR10      : 1;      // bit 1
      BITFIELD_UINT   DHR20      : 1;      // bit 2
      BITFIELD_UINT   DHR30      : 1;      // bit 3
      BITFIELD_UINT   DHR40      : 1;      // bit 4
      BITFIELD_UINT   DHR50      : 1;      // bit 5
      BITFIELD_UINT   DHR60      : 1;      // bit 6
      BITFIELD_UINT   DHR70      : 1;      // bit 7
    };  // DCH1DHR8 bitfield

    /// register _DAC_DCH1DHR8 reset value
    #define sfr_DAC_DCH1DHR8_RESET_VALUE   ((uint8_t) 0x00)

  } DCH1DHR8;


  /** DAC channel 2 8-bit mode data holding register (DCH2DHR8 at 0x53a9) */
  union {

    /// bytewise access to DCH2DHR8
    uint8_t  byte;

    /// bitwise access to register DCH2DHR8
    struct {
      BITFIELD_UINT   DHR00      : 1;      // bit 0
      BITFIELD_UINT   DHR10      : 1;      // bit 1
      BITFIELD_UINT   DHR20      : 1;      // bit 2
      BITFIELD_UINT   DHR30      : 1;      // bit 3
      BITFIELD_UINT   DHR40      : 1;      // bit 4
      BITFIELD_UINT   DHR50      : 1;      // bit 5
      BITFIELD_UINT   DHR60      : 1;      // bit 6
      BITFIELD_UINT   DHR70      : 1;      // bit 7
    };  // DCH2DHR8 bitfield

    /// register _DAC_DCH2DHR8 reset value
    #define sfr_DAC_DCH2DHR8_RESET_VALUE   ((uint8_t) 0x00)

  } DCH2DHR8;


  /// Reserved register (2B)
  uint8_t     Reserved_8[2];


  /** DAC data output register high (DORH_CH1DORH at 0x53ac) */
  union {

    /// bytewise access to DORH_CH1DORH
    uint8_t  byte;

    /// bitwise access to register DORH
    struct {
      BITFIELD_UINT   DORH       : 4;      // bits 0-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // DORH bitfield

    /// register _DAC_DORH reset value
    #define sfr_DAC_DORH_RESET_VALUE   ((uint8_t) 0x00)


    /// bitwise access to register CH1DORH
    struct {
      BITFIELD_UINT   DORH00     : 1;      // bit 0
      BITFIELD_UINT   DORH10     : 1;      // bit 1
      BITFIELD_UINT   DORH20     : 1;      // bit 2
      BITFIELD_UINT   DORH30     : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CH1DORH bitfield

    /// register _DAC_CH1DORH reset value
    #define sfr_DAC_CH1DORH_RESET_VALUE   ((uint8_t) 0x00)

  } DORH_CH1DORH;


  /** DAC data output register low (DORL_CH1DORL at 0x53ad) */
  union {

    /// bytewise access to DORL_CH1DORL
    uint8_t  byte;

    /// bitwise access to register DORL
    struct {
      BITFIELD_UINT   DORL       : 8;      // bits 0-7
    };  // DORL bitfield

    /// register _DAC_DORL reset value
    #define sfr_DAC_DORL_RESET_VALUE   ((uint8_t) 0x00)


    /// bitwise access to register CH1DORL
    struct {
      BITFIELD_UINT   DORL00     : 1;      // bit 0
      BITFIELD_UINT   DORL10     : 1;      // bit 1
      BITFIELD_UINT   DORL20     : 1;      // bit 2
      BITFIELD_UINT   DORL30     : 1;      // bit 3
      BITFIELD_UINT   DORL40     : 1;      // bit 4
      BITFIELD_UINT   DORL50     : 1;      // bit 5
      BITFIELD_UINT   DORL60     : 1;      // bit 6
      BITFIELD_UINT   DORL70     : 1;      // bit 7
    };  // CH1DORL bitfield

    /// register _DAC_CH1DORL reset value
    #define sfr_DAC_CH1DORL_RESET_VALUE   ((uint8_t) 0x00)

  } DORL_CH1DORL;


  /// Reserved register (2B)
  uint8_t     Reserved_9[2];


  /** DAC channel 2 data output register high (CH2DORH at 0x53b0) */
  union {

    /// bytewise access to CH2DORH
    uint8_t  byte;

    /// bitwise access to register CH2DORH
    struct {
      BITFIELD_UINT   DORH00     : 1;      // bit 0
      BITFIELD_UINT   DORH10     : 1;      // bit 1
      BITFIELD_UINT   DORH20     : 1;      // bit 2
      BITFIELD_UINT   DORH30     : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CH2DORH bitfield

    /// register _DAC_CH2DORH reset value
    #define sfr_DAC_CH2DORH_RESET_VALUE   ((uint8_t) 0x00)

  } CH2DORH;


  /** DAC channel 2 data output register low (CH2DORL at 0x53b1) */
  union {

    /// bytewise access to CH2DORL
    uint8_t  byte;

    /// bitwise access to register CH2DORL
    struct {
      BITFIELD_UINT   DORL00     : 1;      // bit 0
      BITFIELD_UINT   DORL10     : 1;      // bit 1
      BITFIELD_UINT   DORL20     : 1;      // bit 2
      BITFIELD_UINT   DORL30     : 1;      // bit 3
      BITFIELD_UINT   DORL40     : 1;      // bit 4
      BITFIELD_UINT   DORL50     : 1;      // bit 5
      BITFIELD_UINT   DORL60     : 1;      // bit 6
      BITFIELD_UINT   DORL70     : 1;      // bit 7
    };  // CH2DORL bitfield

    /// register _DAC_CH2DORL reset value
    #define sfr_DAC_CH2DORL_RESET_VALUE   ((uint8_t) 0x00)

  } CH2DORL;

} DAC_t;

/// access to DAC SFR registers
#define sfr_DAC   (*((DAC_t*) 0x5380))


//------------------------
// Module DM
//------------------------

/** struct containing DM module registers */
typedef struct {

  /** DM breakpoint 1 register extended byte (BK1RE at 0x7f90) */
  union {

    /// bytewise access to BK1RE
    uint8_t  byte;

    /// skip bitwise access to register BK1RE

    /// register _DM_BK1RE reset value
    #define sfr_DM_BK1RE_RESET_VALUE   ((uint8_t) 0xFF)

  } BK1RE;


  /** DM breakpoint 1 register high byte (BK1RH at 0x7f91) */
  union {

    /// bytewise access to BK1RH
    uint8_t  byte;

    /// skip bitwise access to register BK1RH

    /// register _DM_BK1RH reset value
    #define sfr_DM_BK1RH_RESET_VALUE   ((uint8_t) 0xFF)

  } BK1RH;


  /** DM breakpoint 1 register low byte (BK1RL at 0x7f92) */
  union {

    /// bytewise access to BK1RL
    uint8_t  byte;

    /// skip bitwise access to register BK1RL

    /// register _DM_BK1RL reset value
    #define sfr_DM_BK1RL_RESET_VALUE   ((uint8_t) 0xFF)

  } BK1RL;


  /** DM breakpoint 2 register extended byte (BK2RE at 0x7f93) */
  union {

    /// bytewise access to BK2RE
    uint8_t  byte;

    /// skip bitwise access to register BK2RE

    /// register _DM_BK2RE reset value
    #define sfr_DM_BK2RE_RESET_VALUE   ((uint8_t) 0xFF)

  } BK2RE;


  /** DM breakpoint 2 register high byte (BK2RH at 0x7f94) */
  union {

    /// bytewise access to BK2RH
    uint8_t  byte;

    /// skip bitwise access to register BK2RH

    /// register _DM_BK2RH reset value
    #define sfr_DM_BK2RH_RESET_VALUE   ((uint8_t) 0xFF)

  } BK2RH;


  /** DM breakpoint 2 register low byte (BK2RL at 0x7f95) */
  union {

    /// bytewise access to BK2RL
    uint8_t  byte;

    /// skip bitwise access to register BK2RL

    /// register _DM_BK2RL reset value
    #define sfr_DM_BK2RL_RESET_VALUE   ((uint8_t) 0xFF)

  } BK2RL;


  /** DM Debug module control register 1 (CR1 at 0x7f96) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// skip bitwise access to register CR1

    /// register _DM_CR1 reset value
    #define sfr_DM_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** DM Debug module control register 2 (CR2 at 0x7f97) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// skip bitwise access to register CR2

    /// register _DM_CR2 reset value
    #define sfr_DM_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** DM Debug module control/status register 1 (CSR1 at 0x7f98) */
  union {

    /// bytewise access to CSR1
    uint8_t  byte;

    /// skip bitwise access to register CSR1

    /// register _DM_CSR1 reset value
    #define sfr_DM_CSR1_RESET_VALUE   ((uint8_t) 0x10)

  } CSR1;


  /** DM Debug module control/status register 2 (CSR2 at 0x7f99) */
  union {

    /// bytewise access to CSR2
    uint8_t  byte;

    /// skip bitwise access to register CSR2

    /// register _DM_CSR2 reset value
    #define sfr_DM_CSR2_RESET_VALUE   ((uint8_t) 0x00)

  } CSR2;


  /** DM enable function register (ENFCTR at 0x7f9a) */
  union {

    /// bytewise access to ENFCTR
    uint8_t  byte;

    /// skip bitwise access to register ENFCTR

    /// register _DM_ENFCTR reset value
    #define sfr_DM_ENFCTR_RESET_VALUE   ((uint8_t) 0xFF)

  } ENFCTR;

} DM_t;

/// access to DM SFR registers
#define sfr_DM   (*((DM_t*) 0x7f90))


//------------------------
// Module DMA1
//------------------------

/** struct containing DMA1 module registers */
typedef struct {

  /** DMA1 global configuration & status register (GCSR at 0x5070) */
  union {

    /// bytewise access to GCSR
    uint8_t  byte;

    /// bitwise access to register GCSR
    struct {
      BITFIELD_UINT   GEN        : 1;      // bit 0
      BITFIELD_UINT   GB         : 1;      // bit 1
      BITFIELD_UINT   TO         : 6;      // bits 2-7
    };  // GCSR bitfield

    /// register _DMA1_GCSR reset value
    #define sfr_DMA1_GCSR_RESET_VALUE   ((uint8_t) 0xFC)

  } GCSR;


  /** DMA1 global interrupt register 1 (GIR1 at 0x5071) */
  union {

    /// bytewise access to GIR1
    uint8_t  byte;

    /// bitwise access to register GIR1
    struct {
      BITFIELD_UINT   IFC0       : 1;      // bit 0
      BITFIELD_UINT   IFC1       : 1;      // bit 1
      BITFIELD_UINT   IFC2       : 1;      // bit 2
      BITFIELD_UINT   IFC3       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // GIR1 bitfield

    /// register _DMA1_GIR1 reset value
    #define sfr_DMA1_GIR1_RESET_VALUE   ((uint8_t) 0x00)

  } GIR1;


  /// Reserved register (3B)
  uint8_t     Reserved_1[3];


  /** DMA1 channel 0 configuration register (C0CR at 0x5075) */
  union {

    /// bytewise access to C0CR
    uint8_t  byte;

    /// bitwise access to register C0CR
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   TCIE       : 1;      // bit 1
      BITFIELD_UINT   HTIE       : 1;      // bit 2
      BITFIELD_UINT   DIR        : 1;      // bit 3
      BITFIELD_UINT   CIRC       : 1;      // bit 4
      BITFIELD_UINT   MINCDEC    : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // C0CR bitfield

    /// register _DMA1_C0CR reset value
    #define sfr_DMA1_C0CR_RESET_VALUE   ((uint8_t) 0x00)

  } C0CR;


  /** DMA1 channel 0 status (C0SPR at 0x5076) */
  union {

    /// bytewise access to C0SPR
    uint8_t  byte;

    /// bitwise access to register C0SPR
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TCIF       : 1;      // bit 1
      BITFIELD_UINT   HTIF       : 1;      // bit 2
      BITFIELD_UINT   TSIZE      : 1;      // bit 3
      BITFIELD_UINT   PL0        : 1;      // bit 4
      BITFIELD_UINT   PL1        : 1;      // bit 5
      BITFIELD_UINT   PEND       : 1;      // bit 6
      BITFIELD_UINT   BUSY       : 1;      // bit 7
    };  // C0SPR bitfield

    /// register _DMA1_C0SPR reset value
    #define sfr_DMA1_C0SPR_RESET_VALUE   ((uint8_t) 0x00)

  } C0SPR;


  /** DMA1 number of data to transfer register (channel 0) (C0NDTR at 0x5077) */
  union {

    /// bytewise access to C0NDTR
    uint8_t  byte;

    /// bitwise access to register C0NDTR
    struct {
      BITFIELD_UINT   NDT0       : 1;      // bit 0
      BITFIELD_UINT   NDT1       : 1;      // bit 1
      BITFIELD_UINT   NDT2       : 1;      // bit 2
      BITFIELD_UINT   NDT3       : 1;      // bit 3
      BITFIELD_UINT   NDT4       : 1;      // bit 4
      BITFIELD_UINT   NDT5       : 1;      // bit 5
      BITFIELD_UINT   NDT6       : 1;      // bit 6
      BITFIELD_UINT   NDT7       : 1;      // bit 7
    };  // C0NDTR bitfield

    /// register _DMA1_C0NDTR reset value
    #define sfr_DMA1_C0NDTR_RESET_VALUE   ((uint8_t) 0x00)

  } C0NDTR;


  /** DMA1 peripheral address high register (channel 0) (C0PARH at 0x5078) */
  union {

    /// bytewise access to C0PARH
    uint8_t  byte;

    /// bitwise access to register C0PARH
    struct {
      BITFIELD_UINT   PA8        : 1;      // bit 0
      BITFIELD_UINT   PA9        : 1;      // bit 1
      BITFIELD_UINT   PA10       : 1;      // bit 2
      BITFIELD_UINT   PA11       : 1;      // bit 3
      BITFIELD_UINT   PA12       : 1;      // bit 4
      BITFIELD_UINT   PA13       : 1;      // bit 5
      BITFIELD_UINT   PA14       : 1;      // bit 6
      BITFIELD_UINT   PA15       : 1;      // bit 7
    };  // C0PARH bitfield

    /// register _DMA1_C0PARH reset value
    #define sfr_DMA1_C0PARH_RESET_VALUE   ((uint8_t) 0x52)

  } C0PARH;


  /** DMA1 peripheral address low register (channel 0) (C0PARL at 0x5079) */
  union {

    /// bytewise access to C0PARL
    uint8_t  byte;

    /// bitwise access to register C0PARL
    struct {
      BITFIELD_UINT   PA0        : 1;      // bit 0
      BITFIELD_UINT   PA1        : 1;      // bit 1
      BITFIELD_UINT   PA2        : 1;      // bit 2
      BITFIELD_UINT   PA3        : 1;      // bit 3
      BITFIELD_UINT   PA4        : 1;      // bit 4
      BITFIELD_UINT   PA5        : 1;      // bit 5
      BITFIELD_UINT   PA6        : 1;      // bit 6
      BITFIELD_UINT   PA7        : 1;      // bit 7
    };  // C0PARL bitfield

    /// register _DMA1_C0PARL reset value
    #define sfr_DMA1_C0PARL_RESET_VALUE   ((uint8_t) 0x00)

  } C0PARL;


  /// Reserved register (1B)
  uint8_t     Reserved_2[1];


  /** DMA1 memory 0 address high register (channel 0) (C0M0ARH at 0x507b) */
  union {

    /// bytewise access to C0M0ARH
    uint8_t  byte;

    /// bitwise access to register C0M0ARH
    struct {
      BITFIELD_UINT   M0A8       : 1;      // bit 0
      BITFIELD_UINT   M0A9       : 1;      // bit 1
      BITFIELD_UINT   M0A10      : 1;      // bit 2
      BITFIELD_UINT   M0A11      : 1;      // bit 3
      BITFIELD_UINT   M0A12      : 1;      // bit 4
      BITFIELD_UINT   M0A13      : 1;      // bit 5
      BITFIELD_UINT   M0A14      : 1;      // bit 6
      BITFIELD_UINT   M0A15      : 1;      // bit 7
    };  // C0M0ARH bitfield

    /// register _DMA1_C0M0ARH reset value
    #define sfr_DMA1_C0M0ARH_RESET_VALUE   ((uint8_t) 0x00)

  } C0M0ARH;


  /** DMA1 memory 0 address low register (channel 0) (C0M0ARL at 0x507c) */
  union {

    /// bytewise access to C0M0ARL
    uint8_t  byte;

    /// bitwise access to register C0M0ARL
    struct {
      BITFIELD_UINT   M0A0       : 1;      // bit 0
      BITFIELD_UINT   M0A1       : 1;      // bit 1
      BITFIELD_UINT   M0A2       : 1;      // bit 2
      BITFIELD_UINT   M0A3       : 1;      // bit 3
      BITFIELD_UINT   M0A4       : 1;      // bit 4
      BITFIELD_UINT   M0A5       : 1;      // bit 5
      BITFIELD_UINT   M0A6       : 1;      // bit 6
      BITFIELD_UINT   M0A7       : 1;      // bit 7
    };  // C0M0ARL bitfield

    /// register _DMA1_C0M0ARL reset value
    #define sfr_DMA1_C0M0ARL_RESET_VALUE   ((uint8_t) 0x00)

  } C0M0ARL;


  /// Reserved register (2B)
  uint8_t     Reserved_3[2];


  /** DMA1 channel 1 configuration register (C1CR at 0x507f) */
  union {

    /// bytewise access to C1CR
    uint8_t  byte;

    /// bitwise access to register C1CR
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   TCIE       : 1;      // bit 1
      BITFIELD_UINT   HTIE       : 1;      // bit 2
      BITFIELD_UINT   DIR        : 1;      // bit 3
      BITFIELD_UINT   CIRC       : 1;      // bit 4
      BITFIELD_UINT   MINCDEC    : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // C1CR bitfield

    /// register _DMA1_C1CR reset value
    #define sfr_DMA1_C1CR_RESET_VALUE   ((uint8_t) 0x00)

  } C1CR;


  /** DMA1 channel 1 status (C1SPR at 0x5080) */
  union {

    /// bytewise access to C1SPR
    uint8_t  byte;

    /// bitwise access to register C1SPR
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TCIF       : 1;      // bit 1
      BITFIELD_UINT   HTIF       : 1;      // bit 2
      BITFIELD_UINT   TSIZE      : 1;      // bit 3
      BITFIELD_UINT   PL0        : 1;      // bit 4
      BITFIELD_UINT   PL1        : 1;      // bit 5
      BITFIELD_UINT   PEND       : 1;      // bit 6
      BITFIELD_UINT   BUSY       : 1;      // bit 7
    };  // C1SPR bitfield

    /// register _DMA1_C1SPR reset value
    #define sfr_DMA1_C1SPR_RESET_VALUE   ((uint8_t) 0x00)

  } C1SPR;


  /** DMA1 number of data to transfer register (channel 1) (C1NDTR at 0x5081) */
  union {

    /// bytewise access to C1NDTR
    uint8_t  byte;

    /// bitwise access to register C1NDTR
    struct {
      BITFIELD_UINT   NDT0       : 1;      // bit 0
      BITFIELD_UINT   NDT1       : 1;      // bit 1
      BITFIELD_UINT   NDT2       : 1;      // bit 2
      BITFIELD_UINT   NDT3       : 1;      // bit 3
      BITFIELD_UINT   NDT4       : 1;      // bit 4
      BITFIELD_UINT   NDT5       : 1;      // bit 5
      BITFIELD_UINT   NDT6       : 1;      // bit 6
      BITFIELD_UINT   NDT7       : 1;      // bit 7
    };  // C1NDTR bitfield

    /// register _DMA1_C1NDTR reset value
    #define sfr_DMA1_C1NDTR_RESET_VALUE   ((uint8_t) 0x00)

  } C1NDTR;


  /** DMA1 peripheral address high register (channel 1) (C1PARH at 0x5082) */
  union {

    /// bytewise access to C1PARH
    uint8_t  byte;

    /// bitwise access to register C1PARH
    struct {
      BITFIELD_UINT   PA8        : 1;      // bit 0
      BITFIELD_UINT   PA9        : 1;      // bit 1
      BITFIELD_UINT   PA10       : 1;      // bit 2
      BITFIELD_UINT   PA11       : 1;      // bit 3
      BITFIELD_UINT   PA12       : 1;      // bit 4
      BITFIELD_UINT   PA13       : 1;      // bit 5
      BITFIELD_UINT   PA14       : 1;      // bit 6
      BITFIELD_UINT   PA15       : 1;      // bit 7
    };  // C1PARH bitfield

    /// register _DMA1_C1PARH reset value
    #define sfr_DMA1_C1PARH_RESET_VALUE   ((uint8_t) 0x52)

  } C1PARH;


  /** DMA1 peripheral address low register (channel 1) (C1PARL at 0x5083) */
  union {

    /// bytewise access to C1PARL
    uint8_t  byte;

    /// bitwise access to register C1PARL
    struct {
      BITFIELD_UINT   PA0        : 1;      // bit 0
      BITFIELD_UINT   PA1        : 1;      // bit 1
      BITFIELD_UINT   PA2        : 1;      // bit 2
      BITFIELD_UINT   PA3        : 1;      // bit 3
      BITFIELD_UINT   PA4        : 1;      // bit 4
      BITFIELD_UINT   PA5        : 1;      // bit 5
      BITFIELD_UINT   PA6        : 1;      // bit 6
      BITFIELD_UINT   PA7        : 1;      // bit 7
    };  // C1PARL bitfield

    /// register _DMA1_C1PARL reset value
    #define sfr_DMA1_C1PARL_RESET_VALUE   ((uint8_t) 0x00)

  } C1PARL;


  /// Reserved register (1B)
  uint8_t     Reserved_4[1];


  /** DMA1 memory 0 address high register (channel 1) (C1M0ARH at 0x5085) */
  union {

    /// bytewise access to C1M0ARH
    uint8_t  byte;

    /// bitwise access to register C1M0ARH
    struct {
      BITFIELD_UINT   M0A8       : 1;      // bit 0
      BITFIELD_UINT   M0A9       : 1;      // bit 1
      BITFIELD_UINT   M0A10      : 1;      // bit 2
      BITFIELD_UINT   M0A11      : 1;      // bit 3
      BITFIELD_UINT   M0A12      : 1;      // bit 4
      BITFIELD_UINT   M0A13      : 1;      // bit 5
      BITFIELD_UINT   M0A14      : 1;      // bit 6
      BITFIELD_UINT   M0A15      : 1;      // bit 7
    };  // C1M0ARH bitfield

    /// register _DMA1_C1M0ARH reset value
    #define sfr_DMA1_C1M0ARH_RESET_VALUE   ((uint8_t) 0x00)

  } C1M0ARH;


  /** DMA1 memory 0 address low register (channel 1) (C1M0ARL at 0x5086) */
  union {

    /// bytewise access to C1M0ARL
    uint8_t  byte;

    /// bitwise access to register C1M0ARL
    struct {
      BITFIELD_UINT   M0A0       : 1;      // bit 0
      BITFIELD_UINT   M0A1       : 1;      // bit 1
      BITFIELD_UINT   M0A2       : 1;      // bit 2
      BITFIELD_UINT   M0A3       : 1;      // bit 3
      BITFIELD_UINT   M0A4       : 1;      // bit 4
      BITFIELD_UINT   M0A5       : 1;      // bit 5
      BITFIELD_UINT   M0A6       : 1;      // bit 6
      BITFIELD_UINT   M0A7       : 1;      // bit 7
    };  // C1M0ARL bitfield

    /// register _DMA1_C1M0ARL reset value
    #define sfr_DMA1_C1M0ARL_RESET_VALUE   ((uint8_t) 0x00)

  } C1M0ARL;


  /// Reserved register (2B)
  uint8_t     Reserved_5[2];


  /** DMA1 channel 2 configuration register (C2CR at 0x5089) */
  union {

    /// bytewise access to C2CR
    uint8_t  byte;

    /// bitwise access to register C2CR
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   TCIE       : 1;      // bit 1
      BITFIELD_UINT   HTIE       : 1;      // bit 2
      BITFIELD_UINT   DIR        : 1;      // bit 3
      BITFIELD_UINT   CIRC       : 1;      // bit 4
      BITFIELD_UINT   MINCDEC    : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // C2CR bitfield

    /// register _DMA1_C2CR reset value
    #define sfr_DMA1_C2CR_RESET_VALUE   ((uint8_t) 0x00)

  } C2CR;


  /** DMA1 channel 2 status (C2SPR at 0x508a) */
  union {

    /// bytewise access to C2SPR
    uint8_t  byte;

    /// bitwise access to register C2SPR
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TCIF       : 1;      // bit 1
      BITFIELD_UINT   HTIF       : 1;      // bit 2
      BITFIELD_UINT   TSIZE      : 1;      // bit 3
      BITFIELD_UINT   PL0        : 1;      // bit 4
      BITFIELD_UINT   PL1        : 1;      // bit 5
      BITFIELD_UINT   PEND       : 1;      // bit 6
      BITFIELD_UINT   BUSY       : 1;      // bit 7
    };  // C2SPR bitfield

    /// register _DMA1_C2SPR reset value
    #define sfr_DMA1_C2SPR_RESET_VALUE   ((uint8_t) 0x00)

  } C2SPR;


  /** DMA1 number of data to transfer register (channel 2) (C2NDTR at 0x508b) */
  union {

    /// bytewise access to C2NDTR
    uint8_t  byte;

    /// bitwise access to register C2NDTR
    struct {
      BITFIELD_UINT   NDT0       : 1;      // bit 0
      BITFIELD_UINT   NDT1       : 1;      // bit 1
      BITFIELD_UINT   NDT2       : 1;      // bit 2
      BITFIELD_UINT   NDT3       : 1;      // bit 3
      BITFIELD_UINT   NDT4       : 1;      // bit 4
      BITFIELD_UINT   NDT5       : 1;      // bit 5
      BITFIELD_UINT   NDT6       : 1;      // bit 6
      BITFIELD_UINT   NDT7       : 1;      // bit 7
    };  // C2NDTR bitfield

    /// register _DMA1_C2NDTR reset value
    #define sfr_DMA1_C2NDTR_RESET_VALUE   ((uint8_t) 0x00)

  } C2NDTR;


  /** DMA1 peripheral address high register (channel 2) (C2PARH at 0x508c) */
  union {

    /// bytewise access to C2PARH
    uint8_t  byte;

    /// bitwise access to register C2PARH
    struct {
      BITFIELD_UINT   PA8        : 1;      // bit 0
      BITFIELD_UINT   PA9        : 1;      // bit 1
      BITFIELD_UINT   PA10       : 1;      // bit 2
      BITFIELD_UINT   PA11       : 1;      // bit 3
      BITFIELD_UINT   PA12       : 1;      // bit 4
      BITFIELD_UINT   PA13       : 1;      // bit 5
      BITFIELD_UINT   PA14       : 1;      // bit 6
      BITFIELD_UINT   PA15       : 1;      // bit 7
    };  // C2PARH bitfield

    /// register _DMA1_C2PARH reset value
    #define sfr_DMA1_C2PARH_RESET_VALUE   ((uint8_t) 0x52)

  } C2PARH;


  /** DMA1 peripheral address low register (channel 2) (C2PARL at 0x508d) */
  union {

    /// bytewise access to C2PARL
    uint8_t  byte;

    /// bitwise access to register C2PARL
    struct {
      BITFIELD_UINT   PA0        : 1;      // bit 0
      BITFIELD_UINT   PA1        : 1;      // bit 1
      BITFIELD_UINT   PA2        : 1;      // bit 2
      BITFIELD_UINT   PA3        : 1;      // bit 3
      BITFIELD_UINT   PA4        : 1;      // bit 4
      BITFIELD_UINT   PA5        : 1;      // bit 5
      BITFIELD_UINT   PA6        : 1;      // bit 6
      BITFIELD_UINT   PA7        : 1;      // bit 7
    };  // C2PARL bitfield

    /// register _DMA1_C2PARL reset value
    #define sfr_DMA1_C2PARL_RESET_VALUE   ((uint8_t) 0x00)

  } C2PARL;


  /// Reserved register (1B)
  uint8_t     Reserved_6[1];


  /** DMA1 memory 0 address high register (channel 2) (C2M0ARH at 0x508f) */
  union {

    /// bytewise access to C2M0ARH
    uint8_t  byte;

    /// bitwise access to register C2M0ARH
    struct {
      BITFIELD_UINT   M0A8       : 1;      // bit 0
      BITFIELD_UINT   M0A9       : 1;      // bit 1
      BITFIELD_UINT   M0A10      : 1;      // bit 2
      BITFIELD_UINT   M0A11      : 1;      // bit 3
      BITFIELD_UINT   M0A12      : 1;      // bit 4
      BITFIELD_UINT   M0A13      : 1;      // bit 5
      BITFIELD_UINT   M0A14      : 1;      // bit 6
      BITFIELD_UINT   M0A15      : 1;      // bit 7
    };  // C2M0ARH bitfield

    /// register _DMA1_C2M0ARH reset value
    #define sfr_DMA1_C2M0ARH_RESET_VALUE   ((uint8_t) 0x00)

  } C2M0ARH;


  /** DMA1 memory 0 address low register (channel 2) (C2M0ARL at 0x5090) */
  union {

    /// bytewise access to C2M0ARL
    uint8_t  byte;

    /// bitwise access to register C2M0ARL
    struct {
      BITFIELD_UINT   M0A0       : 1;      // bit 0
      BITFIELD_UINT   M0A1       : 1;      // bit 1
      BITFIELD_UINT   M0A2       : 1;      // bit 2
      BITFIELD_UINT   M0A3       : 1;      // bit 3
      BITFIELD_UINT   M0A4       : 1;      // bit 4
      BITFIELD_UINT   M0A5       : 1;      // bit 5
      BITFIELD_UINT   M0A6       : 1;      // bit 6
      BITFIELD_UINT   M0A7       : 1;      // bit 7
    };  // C2M0ARL bitfield

    /// register _DMA1_C2M0ARL reset value
    #define sfr_DMA1_C2M0ARL_RESET_VALUE   ((uint8_t) 0x00)

  } C2M0ARL;


  /// Reserved register (2B)
  uint8_t     Reserved_7[2];


  /** DMA1 channel 3 configuration register (C3CR at 0x5093) */
  union {

    /// bytewise access to C3CR
    uint8_t  byte;

    /// bitwise access to register C3CR
    struct {
      BITFIELD_UINT   EN         : 1;      // bit 0
      BITFIELD_UINT   TCIE       : 1;      // bit 1
      BITFIELD_UINT   HTIE       : 1;      // bit 2
      BITFIELD_UINT   DIR        : 1;      // bit 3
      BITFIELD_UINT   CIRC       : 1;      // bit 4
      BITFIELD_UINT   MINCDEC    : 1;      // bit 5
      BITFIELD_UINT   MEM        : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // C3CR bitfield

    /// register _DMA1_C3CR reset value
    #define sfr_DMA1_C3CR_RESET_VALUE   ((uint8_t) 0x00)

  } C3CR;


  /** DMA1 channel 3 status (C3SPR at 0x5094) */
  union {

    /// bytewise access to C3SPR
    uint8_t  byte;

    /// bitwise access to register C3SPR
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TCIF       : 1;      // bit 1
      BITFIELD_UINT   HTIF       : 1;      // bit 2
      BITFIELD_UINT   TSIZE      : 1;      // bit 3
      BITFIELD_UINT   PL0        : 1;      // bit 4
      BITFIELD_UINT   PL1        : 1;      // bit 5
      BITFIELD_UINT   PEND       : 1;      // bit 6
      BITFIELD_UINT   BUSY       : 1;      // bit 7
    };  // C3SPR bitfield

    /// register _DMA1_C3SPR reset value
    #define sfr_DMA1_C3SPR_RESET_VALUE   ((uint8_t) 0x00)

  } C3SPR;


  /** DMA1 number of data to transfer register (channel 3) (C3NDTR at 0x5095) */
  union {

    /// bytewise access to C3NDTR
    uint8_t  byte;

    /// bitwise access to register C3NDTR
    struct {
      BITFIELD_UINT   NDT0       : 1;      // bit 0
      BITFIELD_UINT   NDT1       : 1;      // bit 1
      BITFIELD_UINT   NDT2       : 1;      // bit 2
      BITFIELD_UINT   NDT3       : 1;      // bit 3
      BITFIELD_UINT   NDT4       : 1;      // bit 4
      BITFIELD_UINT   NDT5       : 1;      // bit 5
      BITFIELD_UINT   NDT6       : 1;      // bit 6
      BITFIELD_UINT   NDT7       : 1;      // bit 7
    };  // C3NDTR bitfield

    /// register _DMA1_C3NDTR reset value
    #define sfr_DMA1_C3NDTR_RESET_VALUE   ((uint8_t) 0x00)

  } C3NDTR;


  /** DMA1 peripheral address high register (channel 3) (C3PARH_C3M1ARH at 0x5096) */
  union {

    /// bytewise access to C3PARH_C3M1ARH
    uint8_t  byte;

    /// bitwise access to register C3PARH
    struct {
      BITFIELD_UINT   PA8        : 1;      // bit 0
      BITFIELD_UINT   PA9        : 1;      // bit 1
      BITFIELD_UINT   PA10       : 1;      // bit 2
      BITFIELD_UINT   PA11       : 1;      // bit 3
      BITFIELD_UINT   PA12       : 1;      // bit 4
      BITFIELD_UINT   PA13       : 1;      // bit 5
      BITFIELD_UINT   PA14       : 1;      // bit 6
      BITFIELD_UINT   PA15       : 1;      // bit 7
    };  // C3PARH bitfield

    /// register _DMA1_C3PARH reset value
    #define sfr_DMA1_C3PARH_RESET_VALUE   ((uint8_t) 0x40)


    /// bitwise access to register C3M1ARH
    struct {
      BITFIELD_UINT   M1A8       : 1;      // bit 0
      BITFIELD_UINT   M1A9       : 1;      // bit 1
      BITFIELD_UINT   M1A10      : 1;      // bit 2
      BITFIELD_UINT   M1A11      : 1;      // bit 3
      BITFIELD_UINT   M1A12      : 1;      // bit 4
      BITFIELD_UINT   M1A13      : 1;      // bit 5
      BITFIELD_UINT   M1A14      : 1;      // bit 6
      BITFIELD_UINT   M1A15      : 1;      // bit 7
    };  // C3M1ARH bitfield

    /// register _DMA1_C3M1ARH reset value
    #define sfr_DMA1_C3M1ARH_RESET_VALUE   ((uint8_t) 0x40)

  } C3PARH_C3M1ARH;


  /** DMA1 peripheral address low register (channel 3) (C3PARL_C3M1ARL at 0x5097) */
  union {

    /// bytewise access to C3PARL_C3M1ARL
    uint8_t  byte;

    /// bitwise access to register C3PARL
    struct {
      BITFIELD_UINT   PA0        : 1;      // bit 0
      BITFIELD_UINT   PA1        : 1;      // bit 1
      BITFIELD_UINT   PA2        : 1;      // bit 2
      BITFIELD_UINT   PA3        : 1;      // bit 3
      BITFIELD_UINT   PA4        : 1;      // bit 4
      BITFIELD_UINT   PA5        : 1;      // bit 5
      BITFIELD_UINT   PA6        : 1;      // bit 6
      BITFIELD_UINT   PA7        : 1;      // bit 7
    };  // C3PARL bitfield

    /// register _DMA1_C3PARL reset value
    #define sfr_DMA1_C3PARL_RESET_VALUE   ((uint8_t) 0x00)


    /// bitwise access to register C3M1ARL
    struct {
      BITFIELD_UINT   M1A0       : 1;      // bit 0
      BITFIELD_UINT   M1A1       : 1;      // bit 1
      BITFIELD_UINT   M1A2       : 1;      // bit 2
      BITFIELD_UINT   M1A3       : 1;      // bit 3
      BITFIELD_UINT   M1A4       : 1;      // bit 4
      BITFIELD_UINT   M1A5       : 1;      // bit 5
      BITFIELD_UINT   M1A6       : 1;      // bit 6
      BITFIELD_UINT   M1A7       : 1;      // bit 7
    };  // C3M1ARL bitfield

    /// register _DMA1_C3M1ARL reset value
    #define sfr_DMA1_C3M1ARL_RESET_VALUE   ((uint8_t) 0x00)

  } C3PARL_C3M1ARL;


  /** DMA channel 3 memory 0 extended address register (C3M0EAR at 0x5098) */
  union {

    /// bytewise access to C3M0EAR
    uint8_t  byte;

    /// bitwise access to register C3M0EAR
    struct {
      BITFIELD_UINT   M0A16      : 1;      // bit 0
      BITFIELD_UINT              : 7;      // 7 bits
    };  // C3M0EAR bitfield

    /// register _DMA1_C3M0EAR reset value
    #define sfr_DMA1_C3M0EAR_RESET_VALUE   ((uint8_t) 0x00)

  } C3M0EAR;


  /** DMA1 memory 0 address high register (channel 3) (C3M0ARH at 0x5099) */
  union {

    /// bytewise access to C3M0ARH
    uint8_t  byte;

    /// bitwise access to register C3M0ARH
    struct {
      BITFIELD_UINT   M0A8       : 1;      // bit 0
      BITFIELD_UINT   M0A9       : 1;      // bit 1
      BITFIELD_UINT   M0A10      : 1;      // bit 2
      BITFIELD_UINT   M0A11      : 1;      // bit 3
      BITFIELD_UINT   M0A12      : 1;      // bit 4
      BITFIELD_UINT   M0A13      : 1;      // bit 5
      BITFIELD_UINT   M0A14      : 1;      // bit 6
      BITFIELD_UINT   M0A15      : 1;      // bit 7
    };  // C3M0ARH bitfield

    /// register _DMA1_C3M0ARH reset value
    #define sfr_DMA1_C3M0ARH_RESET_VALUE   ((uint8_t) 0x00)

  } C3M0ARH;


  /** DMA1 memory 0 address low register (channel 3) (C3M0ARL at 0x509a) */
  union {

    /// bytewise access to C3M0ARL
    uint8_t  byte;

    /// bitwise access to register C3M0ARL
    struct {
      BITFIELD_UINT   M0A0       : 1;      // bit 0
      BITFIELD_UINT   M0A1       : 1;      // bit 1
      BITFIELD_UINT   M0A2       : 1;      // bit 2
      BITFIELD_UINT   M0A3       : 1;      // bit 3
      BITFIELD_UINT   M0A4       : 1;      // bit 4
      BITFIELD_UINT   M0A5       : 1;      // bit 5
      BITFIELD_UINT   M0A6       : 1;      // bit 6
      BITFIELD_UINT   M0A7       : 1;      // bit 7
    };  // C3M0ARL bitfield

    /// register _DMA1_C3M0ARL reset value
    #define sfr_DMA1_C3M0ARL_RESET_VALUE   ((uint8_t) 0x00)

  } C3M0ARL;

} DMA1_t;

/// access to DMA1 SFR registers
#define sfr_DMA1   (*((DMA1_t*) 0x5070))


//------------------------
// Module FLASH
//------------------------

/** struct containing FLASH module registers */
typedef struct {

  /** Flash control register 1 (CR1 at 0x5050) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   FIX        : 1;      // bit 0
      BITFIELD_UINT   IE         : 1;      // bit 1
      BITFIELD_UINT   WAITM      : 1;      // bit 2
      BITFIELD_UINT   EEPM       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CR1 bitfield

    /// register _FLASH_CR1 reset value
    #define sfr_FLASH_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** Flash control register 2 (CR2 at 0x5051) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   PRG        : 1;      // bit 0
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   FPRG       : 1;      // bit 4
      BITFIELD_UINT   ERASE      : 1;      // bit 5
      BITFIELD_UINT   WPRG       : 1;      // bit 6
      BITFIELD_UINT   OPT        : 1;      // bit 7
    };  // CR2 bitfield

    /// register _FLASH_CR2 reset value
    #define sfr_FLASH_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** Flash program memory unprotection key register (PUKR at 0x5052) */
  union {

    /// bytewise access to PUKR
    uint8_t  byte;

    /// bitwise access to register PUKR
    struct {
      BITFIELD_UINT   MASS_PRG   : 8;      // bits 0-7
    };  // PUKR bitfield

    /// register _FLASH_PUKR reset value
    #define sfr_FLASH_PUKR_RESET_VALUE   ((uint8_t) 0x00)

  } PUKR;


  /** Data EEPROM unprotection key register (DUKR at 0x5053) */
  union {

    /// bytewise access to DUKR
    uint8_t  byte;

    /// bitwise access to register DUKR
    struct {
      BITFIELD_UINT   MASS_DATA  : 8;      // bits 0-7
    };  // DUKR bitfield

    /// register _FLASH_DUKR reset value
    #define sfr_FLASH_DUKR_RESET_VALUE   ((uint8_t) 0x00)

  } DUKR;


  /** Flash in-application programming status register (IAPSR at 0x5054) */
  union {

    /// bytewise access to IAPSR
    uint8_t  byte;

    /// bitwise access to register IAPSR
    struct {
      BITFIELD_UINT   WR_PG_DIS  : 1;      // bit 0
      BITFIELD_UINT   PUL        : 1;      // bit 1
      BITFIELD_UINT   EOP        : 1;      // bit 2
      BITFIELD_UINT   DUL        : 1;      // bit 3
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   HVOFF      : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // IAPSR bitfield

    /// register _FLASH_IAPSR reset value
    #define sfr_FLASH_IAPSR_RESET_VALUE   ((uint8_t) 0x00)

  } IAPSR;

} FLASH_t;

/// access to FLASH SFR registers
#define sfr_FLASH   (*((FLASH_t*) 0x5050))


//------------------------
// Module I2C1
//------------------------

/** struct containing I2C1 module registers */
typedef struct {

  /** I2C1 control register 1 (CR1 at 0x5210) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   PE         : 1;      // bit 0
      BITFIELD_UINT   SMBUS      : 1;      // bit 1
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   SMBTYPE    : 1;      // bit 3
      BITFIELD_UINT   ENARP      : 1;      // bit 4
      BITFIELD_UINT   ENPEC      : 1;      // bit 5
      BITFIELD_UINT   ENGC       : 1;      // bit 6
      BITFIELD_UINT   NOSTRETCH  : 1;      // bit 7
    };  // CR1 bitfield

    /// register _I2C1_CR1 reset value
    #define sfr_I2C1_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** I2C1 control register 2 (CR2 at 0x5211) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   START      : 1;      // bit 0
      BITFIELD_UINT   STOP       : 1;      // bit 1
      BITFIELD_UINT   ACK        : 1;      // bit 2
      BITFIELD_UINT   POS        : 1;      // bit 3
      BITFIELD_UINT   PEC        : 1;      // bit 4
      BITFIELD_UINT   ALERT      : 1;      // bit 5
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   SWRST      : 1;      // bit 7
    };  // CR2 bitfield

    /// register _I2C1_CR2 reset value
    #define sfr_I2C1_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** I2C1 frequency register (FREQR at 0x5212) */
  union {

    /// bytewise access to FREQR
    uint8_t  byte;

    /// bitwise access to register FREQR
    struct {
      BITFIELD_UINT   FREQ       : 6;      // bits 0-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // FREQR bitfield

    /// register _I2C1_FREQR reset value
    #define sfr_I2C1_FREQR_RESET_VALUE   ((uint8_t) 0x00)

  } FREQR;


  /** I2C1 own address register low (OARL at 0x5213) */
  union {

    /// bytewise access to OARL
    uint8_t  byte;

    /// bitwise access to register OARL
    struct {
      BITFIELD_UINT   ADD0       : 1;      // bit 0
      BITFIELD_UINT   ADD1       : 1;      // bit 1
      BITFIELD_UINT   ADD2       : 1;      // bit 2
      BITFIELD_UINT   ADD3       : 1;      // bit 3
      BITFIELD_UINT   ADD4       : 1;      // bit 4
      BITFIELD_UINT   ADD5       : 1;      // bit 5
      BITFIELD_UINT   ADD6       : 1;      // bit 6
      BITFIELD_UINT   ADD7       : 1;      // bit 7
    };  // OARL bitfield

    /// register _I2C1_OARL reset value
    #define sfr_I2C1_OARL_RESET_VALUE   ((uint8_t) 0x00)

  } OARL;


  /** I2C1 own address register high (OARH at 0x5214) */
  union {

    /// bytewise access to OARH
    uint8_t  byte;

    /// bitwise access to register OARH
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   ADD8       : 1;      // bit 1
      BITFIELD_UINT   ADD9       : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   ADDCONF    : 1;      // bit 6
      BITFIELD_UINT   ADDMODE    : 1;      // bit 7
    };  // OARH bitfield

    /// register _I2C1_OARH reset value
    #define sfr_I2C1_OARH_RESET_VALUE   ((uint8_t) 0x00)

  } OARH;


  /** I2C1 own address register for dual mode (OAR2 at 0x5215) */
  union {

    /// bytewise access to OAR2
    uint8_t  byte;

    /// bitwise access to register OAR2
    struct {
      BITFIELD_UINT   ENDUAL     : 1;      // bit 0
      BITFIELD_UINT   ADD21      : 1;      // bit 1
      BITFIELD_UINT   ADD22      : 1;      // bit 2
      BITFIELD_UINT   ADD23      : 1;      // bit 3
      BITFIELD_UINT   ADD24      : 1;      // bit 4
      BITFIELD_UINT   ADD25      : 1;      // bit 5
      BITFIELD_UINT   ADD26      : 1;      // bit 6
      BITFIELD_UINT   ADD27      : 1;      // bit 7
    };  // OAR2 bitfield

    /// register _I2C1_OAR2 reset value
    #define sfr_I2C1_OAR2_RESET_VALUE   ((uint8_t) 0x00)

  } OAR2;


  /** I2C1 data register (DR at 0x5216) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _I2C1_DR reset value
    #define sfr_I2C1_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** I2C1 status register 1 (SR1 at 0x5217) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   SB         : 1;      // bit 0
      BITFIELD_UINT   ADDR       : 1;      // bit 1
      BITFIELD_UINT   BTF        : 1;      // bit 2
      BITFIELD_UINT   ADD10      : 1;      // bit 3
      BITFIELD_UINT   STOPF      : 1;      // bit 4
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   RXNE       : 1;      // bit 6
      BITFIELD_UINT   TXE        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _I2C1_SR1 reset value
    #define sfr_I2C1_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** I2C1 status register 2 (SR2 at 0x5218) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT   BERR       : 1;      // bit 0
      BITFIELD_UINT   ARLO       : 1;      // bit 1
      BITFIELD_UINT   AF         : 1;      // bit 2
      BITFIELD_UINT   OVR        : 1;      // bit 3
      BITFIELD_UINT   PECERR     : 1;      // bit 4
      BITFIELD_UINT   WUFH       : 1;      // bit 5
      BITFIELD_UINT   TIMEOUT    : 1;      // bit 6
      BITFIELD_UINT   SMBALERT   : 1;      // bit 7
    };  // SR2 bitfield

    /// register _I2C1_SR2 reset value
    #define sfr_I2C1_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** I2C1 status register 3 (SR3 at 0x5219) */
  union {

    /// bytewise access to SR3
    uint8_t  byte;

    /// bitwise access to register SR3
    struct {
      BITFIELD_UINT   MSL        : 1;      // bit 0
      BITFIELD_UINT   BUSY       : 1;      // bit 1
      BITFIELD_UINT   TRA        : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   GENCALL    : 1;      // bit 4
      BITFIELD_UINT   SMBDEFAULT : 1;      // bit 5
      BITFIELD_UINT   SMBHOST    : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // SR3 bitfield

    /// register _I2C1_SR3 reset value
    #define sfr_I2C1_SR3_RESET_VALUE   ((uint8_t) 0x00)

  } SR3;


  /** I2C1 interrupt control register (ITR at 0x521a) */
  union {

    /// bytewise access to ITR
    uint8_t  byte;

    /// bitwise access to register ITR
    struct {
      BITFIELD_UINT   ITERREN    : 1;      // bit 0
      BITFIELD_UINT   ITEVTEN    : 1;      // bit 1
      BITFIELD_UINT   ITBUFEN    : 1;      // bit 2
      BITFIELD_UINT   DMAEN      : 1;      // bit 3
      BITFIELD_UINT   LAST       : 1;      // bit 4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // ITR bitfield

    /// register _I2C1_ITR reset value
    #define sfr_I2C1_ITR_RESET_VALUE   ((uint8_t) 0x00)

  } ITR;


  /** I2C1 clock control register low (CCRL at 0x521b) */
  union {

    /// bytewise access to CCRL
    uint8_t  byte;

    /// bitwise access to register CCRL
    struct {
      BITFIELD_UINT   CCR0       : 1;      // bit 0
      BITFIELD_UINT   CCR1       : 1;      // bit 1
      BITFIELD_UINT   CCR2       : 1;      // bit 2
      BITFIELD_UINT   CCR3       : 1;      // bit 3
      BITFIELD_UINT   CCR4       : 1;      // bit 4
      BITFIELD_UINT   CCR5       : 1;      // bit 5
      BITFIELD_UINT   CCR6       : 1;      // bit 6
      BITFIELD_UINT   CCR7       : 1;      // bit 7
    };  // CCRL bitfield

    /// register _I2C1_CCRL reset value
    #define sfr_I2C1_CCRL_RESET_VALUE   ((uint8_t) 0x00)

  } CCRL;


  /** I2C1 clock control register high (CCRH at 0x521c) */
  union {

    /// bytewise access to CCRH
    uint8_t  byte;

    /// bitwise access to register CCRH
    struct {
      BITFIELD_UINT   CCR8       : 1;      // bit 0
      BITFIELD_UINT   CCR9       : 1;      // bit 1
      BITFIELD_UINT   CCR10      : 1;      // bit 2
      BITFIELD_UINT   CCR11      : 1;      // bit 3
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   DUTY       : 1;      // bit 6
      BITFIELD_UINT   F_S        : 1;      // bit 7
    };  // CCRH bitfield

    /// register _I2C1_CCRH reset value
    #define sfr_I2C1_CCRH_RESET_VALUE   ((uint8_t) 0x00)

  } CCRH;


  /** I2C1 TRISE register (TRISER at 0x521d) */
  union {

    /// bytewise access to TRISER
    uint8_t  byte;

    /// bitwise access to register TRISER
    struct {
      BITFIELD_UINT   TRISE      : 6;      // bits 0-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // TRISER bitfield

    /// register _I2C1_TRISER reset value
    #define sfr_I2C1_TRISER_RESET_VALUE   ((uint8_t) 0x02)

  } TRISER;


  /** I2C1 packet error checking register (PECR at 0x521e) */
  union {

    /// bytewise access to PECR
    uint8_t  byte;

    /// bitwise access to register PECR
    struct {
      BITFIELD_UINT   PEC        : 8;      // bits 0-7
    };  // PECR bitfield

    /// register _I2C1_PECR reset value
    #define sfr_I2C1_PECR_RESET_VALUE   ((uint8_t) 0x00)

  } PECR;

} I2C1_t;

/// access to I2C1 SFR registers
#define sfr_I2C1   (*((I2C1_t*) 0x5210))


//------------------------
// Module IRTIM
//------------------------

/** struct containing IRTIM module registers */
typedef struct {

  /** Infrared control register (CR at 0x52ff) */
  union {

    /// bytewise access to CR
    uint8_t  byte;

    /// bitwise access to register CR
    struct {
      BITFIELD_UINT   IR_EN      : 1;      // bit 0
      BITFIELD_UINT   HS_EN      : 1;      // bit 1
      BITFIELD_UINT              : 6;      // 6 bits
    };  // CR bitfield

    /// register _IRTIM_CR reset value
    #define sfr_IRTIM_CR_RESET_VALUE   ((uint8_t) 0x00)

  } CR;

} IRTIM_t;

/// access to IRTIM SFR registers
#define sfr_IRTIM   (*((IRTIM_t*) 0x52ff))


//------------------------
// Module ITC_EXTI
//------------------------

/** struct containing ITC_EXTI module registers */
typedef struct {

  /** External interrupt control register 1 (CR1 at 0x50a0) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   P0IS       : 2;      // bits 0-1
      BITFIELD_UINT   P1IS       : 2;      // bits 2-3
      BITFIELD_UINT   P2IS       : 2;      // bits 4-5
      BITFIELD_UINT   P3IS       : 2;      // bits 6-7
    };  // CR1 bitfield

    /// register _ITC_EXTI_CR1 reset value
    #define sfr_ITC_EXTI_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** External interrupt control register 2 (CR2 at 0x50a1) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   P4IS       : 2;      // bits 0-1
      BITFIELD_UINT   P5IS       : 2;      // bits 2-3
      BITFIELD_UINT   P6IS       : 2;      // bits 4-5
      BITFIELD_UINT   P7IS       : 2;      // bits 6-7
    };  // CR2 bitfield

    /// register _ITC_EXTI_CR2 reset value
    #define sfr_ITC_EXTI_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** External interrupt control register 3 (CR3 at 0x50a2) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   PBIS       : 2;      // bits 0-1
      BITFIELD_UINT   PDIS       : 2;      // bits 2-3
      BITFIELD_UINT   PEIS       : 2;      // bits 4-5
      BITFIELD_UINT   PFIS       : 2;      // bits 6-7
    };  // CR3 bitfield

    /// register _ITC_EXTI_CR3 reset value
    #define sfr_ITC_EXTI_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /** External interrupt status register 1 (SR1 at 0x50a3) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   P0F        : 1;      // bit 0
      BITFIELD_UINT   P1F        : 1;      // bit 1
      BITFIELD_UINT   P2F        : 1;      // bit 2
      BITFIELD_UINT   P3F        : 1;      // bit 3
      BITFIELD_UINT   P4F        : 1;      // bit 4
      BITFIELD_UINT   P5F        : 1;      // bit 5
      BITFIELD_UINT   P6F        : 1;      // bit 6
      BITFIELD_UINT   P7F        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _ITC_EXTI_SR1 reset value
    #define sfr_ITC_EXTI_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** External interrupt status register 2 (SR2 at 0x50a4) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT   PBF        : 1;      // bit 0
      BITFIELD_UINT   PDF        : 1;      // bit 1
      BITFIELD_UINT   PEF        : 1;      // bit 2
      BITFIELD_UINT   PFF        : 1;      // bit 3
      BITFIELD_UINT   PGF        : 1;      // bit 4
      BITFIELD_UINT   PHF        : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // SR2 bitfield

    /// register _ITC_EXTI_SR2 reset value
    #define sfr_ITC_EXTI_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** External interrupt port select register 1 (CONF1 at 0x50a5) */
  union {

    /// bytewise access to CONF1
    uint8_t  byte;

    /// bitwise access to register CONF1
    struct {
      BITFIELD_UINT   PBLIS      : 1;      // bit 0
      BITFIELD_UINT   PBHIS      : 1;      // bit 1
      BITFIELD_UINT   PDLIS      : 1;      // bit 2
      BITFIELD_UINT   PDHIS      : 1;      // bit 3
      BITFIELD_UINT   PELIS      : 1;      // bit 4
      BITFIELD_UINT   PEHIS      : 1;      // bit 5
      BITFIELD_UINT   PFLIS      : 1;      // bit 6
      BITFIELD_UINT   PFES       : 1;      // bit 7
    };  // CONF1 bitfield

    /// register _ITC_EXTI_CONF1 reset value
    #define sfr_ITC_EXTI_CONF1_RESET_VALUE   ((uint8_t) 0x00)

  } CONF1;


  /// Reserved register (4B)
  uint8_t     Reserved_1[4];


  /** External interrupt control register 4 (CR4 at 0x50aa) */
  union {

    /// bytewise access to CR4
    uint8_t  byte;

    /// bitwise access to register CR4
    struct {
      BITFIELD_UINT   PGIS       : 2;      // bits 0-1
      BITFIELD_UINT   PHIS       : 2;      // bits 2-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CR4 bitfield

    /// register _ITC_EXTI_CR4 reset value
    #define sfr_ITC_EXTI_CR4_RESET_VALUE   ((uint8_t) 0x00)

  } CR4;


  /** External interrupt port select register 2 (CONF2 at 0x50ab) */
  union {

    /// bytewise access to CONF2
    uint8_t  byte;

    /// bitwise access to register CONF2
    struct {
      BITFIELD_UINT   PFHIS      : 1;      // bit 0
      BITFIELD_UINT   PGLIS      : 1;      // bit 1
      BITFIELD_UINT   PGHIS      : 1;      // bit 2
      BITFIELD_UINT   PHLIS      : 1;      // bit 3
      BITFIELD_UINT   PHHIS      : 1;      // bit 4
      BITFIELD_UINT   PGBS       : 1;      // bit 5
      BITFIELD_UINT   PHDS       : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CONF2 bitfield

    /// register _ITC_EXTI_CONF2 reset value
    #define sfr_ITC_EXTI_CONF2_RESET_VALUE   ((uint8_t) 0x00)

  } CONF2;

} ITC_EXTI_t;

/// access to ITC_EXTI SFR registers
#define sfr_ITC_EXTI   (*((ITC_EXTI_t*) 0x50a0))


//------------------------
// Module ITC_SPR
//------------------------

/** struct containing ITC_SPR module registers */
typedef struct {

  /** Interrupt Software priority register 1 (SPR1 at 0x7f70) */
  union {

    /// bytewise access to SPR1
    uint8_t  byte;

    /// bitwise access to register SPR1
    struct {
      BITFIELD_UINT   VECT0SPR   : 2;      // bits 0-1
      BITFIELD_UINT   VECT1SPR   : 2;      // bits 2-3
      BITFIELD_UINT   VECT2SPR   : 2;      // bits 4-5
      BITFIELD_UINT   VECT3SPR   : 2;      // bits 6-7
    };  // SPR1 bitfield

    /// register _ITC_SPR_SPR1 reset value
    #define sfr_ITC_SPR_SPR1_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR1;


  /** Interrupt Software priority register 2 (SPR2 at 0x7f71) */
  union {

    /// bytewise access to SPR2
    uint8_t  byte;

    /// bitwise access to register SPR2
    struct {
      BITFIELD_UINT   VECT4SPR   : 2;      // bits 0-1
      BITFIELD_UINT   VECT5SPR   : 2;      // bits 2-3
      BITFIELD_UINT   VECT6SPR   : 2;      // bits 4-5
      BITFIELD_UINT   VECT7SPR   : 2;      // bits 6-7
    };  // SPR2 bitfield

    /// register _ITC_SPR_SPR2 reset value
    #define sfr_ITC_SPR_SPR2_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR2;


  /** Interrupt Software priority register 3 (SPR3 at 0x7f72) */
  union {

    /// bytewise access to SPR3
    uint8_t  byte;

    /// bitwise access to register SPR3
    struct {
      BITFIELD_UINT   VECT8SPR   : 2;      // bits 0-1
      BITFIELD_UINT   VECT9SPR   : 2;      // bits 2-3
      BITFIELD_UINT   VECT10SPR  : 2;      // bits 4-5
      BITFIELD_UINT   VECT11SPR  : 2;      // bits 6-7
    };  // SPR3 bitfield

    /// register _ITC_SPR_SPR3 reset value
    #define sfr_ITC_SPR_SPR3_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR3;


  /** Interrupt Software priority register 4 (SPR4 at 0x7f73) */
  union {

    /// bytewise access to SPR4
    uint8_t  byte;

    /// bitwise access to register SPR4
    struct {
      BITFIELD_UINT   VECT12SPR  : 2;      // bits 0-1
      BITFIELD_UINT   VECT13SPR  : 2;      // bits 2-3
      BITFIELD_UINT   VECT14SPR  : 2;      // bits 4-5
      BITFIELD_UINT   VECT15SPR  : 2;      // bits 6-7
    };  // SPR4 bitfield

    /// register _ITC_SPR_SPR4 reset value
    #define sfr_ITC_SPR_SPR4_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR4;


  /** Interrupt Software priority register 5 (SPR5 at 0x7f74) */
  union {

    /// bytewise access to SPR5
    uint8_t  byte;

    /// bitwise access to register SPR5
    struct {
      BITFIELD_UINT   VECT16SPR  : 2;      // bits 0-1
      BITFIELD_UINT   VECT17SPR  : 2;      // bits 2-3
      BITFIELD_UINT   VECT18SPR  : 2;      // bits 4-5
      BITFIELD_UINT   VECT19SPR  : 2;      // bits 6-7
    };  // SPR5 bitfield

    /// register _ITC_SPR_SPR5 reset value
    #define sfr_ITC_SPR_SPR5_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR5;


  /** Interrupt Software priority register 6 (SPR6 at 0x7f75) */
  union {

    /// bytewise access to SPR6
    uint8_t  byte;

    /// bitwise access to register SPR6
    struct {
      BITFIELD_UINT   VECT20SPR  : 2;      // bits 0-1
      BITFIELD_UINT   VECT21SPR  : 2;      // bits 2-3
      BITFIELD_UINT   VECT22SPR  : 2;      // bits 4-5
      BITFIELD_UINT   VECT23SPR  : 2;      // bits 6-7
    };  // SPR6 bitfield

    /// register _ITC_SPR_SPR6 reset value
    #define sfr_ITC_SPR_SPR6_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR6;


  /** Interrupt Software priority register 7 (SPR7 at 0x7f76) */
  union {

    /// bytewise access to SPR7
    uint8_t  byte;

    /// bitwise access to register SPR7
    struct {
      BITFIELD_UINT   VECT24SPR  : 2;      // bits 0-1
      BITFIELD_UINT   VECT25SPR  : 2;      // bits 2-3
      BITFIELD_UINT   VECT26SPR  : 2;      // bits 4-5
      BITFIELD_UINT   VECT27SPR  : 2;      // bits 6-7
    };  // SPR7 bitfield

    /// register _ITC_SPR_SPR7 reset value
    #define sfr_ITC_SPR_SPR7_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR7;


  /** Interrupt Software priority register 8 (SPR8 at 0x7f77) */
  union {

    /// bytewise access to SPR8
    uint8_t  byte;

    /// bitwise access to register SPR8
    struct {
      BITFIELD_UINT   VECT28SPR  : 2;      // bits 0-1
      BITFIELD_UINT   VECT29SPR  : 2;      // bits 2-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // SPR8 bitfield

    /// register _ITC_SPR_SPR8 reset value
    #define sfr_ITC_SPR_SPR8_RESET_VALUE   ((uint8_t) 0xFF)

  } SPR8;

} ITC_SPR_t;

/// access to ITC_SPR SFR registers
#define sfr_ITC_SPR   (*((ITC_SPR_t*) 0x7f70))


//------------------------
// Module IWDG
//------------------------

/** struct containing IWDG module registers */
typedef struct {

  /** IWDG key register (KR at 0x50e0) */
  union {

    /// bytewise access to KR
    uint8_t  byte;

    /// bitwise access to register KR
    struct {
      BITFIELD_UINT   KEY0       : 1;      // bit 0
      BITFIELD_UINT   KEY1       : 1;      // bit 1
      BITFIELD_UINT   KEY2       : 1;      // bit 2
      BITFIELD_UINT   KEY3       : 1;      // bit 3
      BITFIELD_UINT   KEY4       : 1;      // bit 4
      BITFIELD_UINT   KEY5       : 1;      // bit 5
      BITFIELD_UINT   KEY6       : 1;      // bit 6
      BITFIELD_UINT   KEY7       : 1;      // bit 7
    };  // KR bitfield

    /// register _IWDG_KR reset value
    #define sfr_IWDG_KR_RESET_VALUE   ((uint8_t) 0x00)

  } KR;


  /** IWDG prescaler register (PR at 0x50e1) */
  union {

    /// bytewise access to PR
    uint8_t  byte;

    /// bitwise access to register PR
    struct {
      BITFIELD_UINT   PR         : 3;      // bits 0-2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // PR bitfield

    /// register _IWDG_PR reset value
    #define sfr_IWDG_PR_RESET_VALUE   ((uint8_t) 0x00)

  } PR;


  /** IWDG reload register (RLR at 0x50e2) */
  union {

    /// bytewise access to RLR
    uint8_t  byte;

    /// bitwise access to register RLR
    struct {
      BITFIELD_UINT   RL         : 8;      // bits 0-7
    };  // RLR bitfield

    /// register _IWDG_RLR reset value
    #define sfr_IWDG_RLR_RESET_VALUE   ((uint8_t) 0xFF)

  } RLR;

} IWDG_t;

/// access to IWDG SFR registers
#define sfr_IWDG   (*((IWDG_t*) 0x50e0))


//------------------------
// Module OPT
//------------------------

/** struct containing OPT module registers */
typedef struct {

  /** Read-out protection (ROP) (OPT0 at 0x4800) */
  union {

    /// bytewise access to OPT0
    uint8_t  byte;

    /// skip bitwise access to register OPT0

    /// register _OPT_OPT0 reset value
    #define sfr_OPT_OPT0_RESET_VALUE   ((uint8_t) 0xAA)

  } OPT0;


  /// Reserved register (1B)
  uint8_t     Reserved_1[1];


  /** User boot code (UBC) (OPT1 at 0x4802) */
  union {

    /// bytewise access to OPT1
    uint8_t  byte;

    /// skip bitwise access to register OPT1

    /// register _OPT_OPT1 reset value
    #define sfr_OPT_OPT1_RESET_VALUE   ((uint8_t) 0x00)

  } OPT1;


  /// Reserved register (4B)
  uint8_t     Reserved_2[4];


  /** PCODESIZE (OPT2 at 0x4807) */
  union {

    /// bytewise access to OPT2
    uint8_t  byte;

    /// skip bitwise access to register OPT2

    /// register _OPT_OPT2 reset value
    #define sfr_OPT_OPT2_RESET_VALUE   ((uint8_t) 0x00)

  } OPT2;


  /** Watchdog option (OPT3 at 0x4808) */
  union {

    /// bytewise access to OPT3
    uint8_t  byte;

    /// skip bitwise access to register OPT3

    /// register _OPT_OPT3 reset value
    #define sfr_OPT_OPT3_RESET_VALUE   ((uint8_t) 0x00)

  } OPT3;


  /** Clock option (OPT4 at 0x4809) */
  union {

    /// bytewise access to OPT4
    uint8_t  byte;

    /// skip bitwise access to register OPT4

    /// register _OPT_OPT4 reset value
    #define sfr_OPT_OPT4_RESET_VALUE   ((uint8_t) 0x00)

  } OPT4;


  /** Brownout reset (BOR) (OPT5 at 0x480a) */
  union {

    /// bytewise access to OPT5
    uint8_t  byte;

    /// skip bitwise access to register OPT5

    /// register _OPT_OPT5 reset value
    #define sfr_OPT_OPT5_RESET_VALUE   ((uint8_t) 0x01)

  } OPT5;


  /** Bootloader (high byte) (OPTBL_H at 0x480b) */
  union {

    /// bytewise access to OPTBL_H
    uint8_t  byte;

    /// skip bitwise access to register OPTBL_H

    /// register _OPT_OPTBL_H reset value
    #define sfr_OPT_OPTBL_H_RESET_VALUE   ((uint8_t) 0x00)

  } OPTBL_H;


  /** Bootloader (low byte) (OPTBL_L at 0x480c) */
  union {

    /// bytewise access to OPTBL_L
    uint8_t  byte;

    /// skip bitwise access to register OPTBL_L

    /// register _OPT_OPTBL_L reset value
    #define sfr_OPT_OPTBL_L_RESET_VALUE   ((uint8_t) 0x00)

  } OPTBL_L;

} OPT_t;

/// access to OPT SFR registers
#define sfr_OPT   (*((OPT_t*) 0x4800))


//------------------------
// Module PWR
//------------------------

/** struct containing PWR module registers */
typedef struct {

  /** Power control and status register 1 (CSR1 at 0x50b2) */
  union {

    /// bytewise access to CSR1
    uint8_t  byte;

    /// bitwise access to register CSR1
    struct {
      BITFIELD_UINT   PVDE       : 1;      // bit 0
      BITFIELD_UINT   PLS        : 3;      // bits 1-3
      BITFIELD_UINT   PVDIEN     : 1;      // bit 4
      BITFIELD_UINT   PVDIF      : 1;      // bit 5
      BITFIELD_UINT   PVDOF      : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CSR1 bitfield

    /// register _PWR_CSR1 reset value
    #define sfr_PWR_CSR1_RESET_VALUE   ((uint8_t) 0x00)

  } CSR1;


  /** Power control and status register 2 (CSR2 at 0x50b3) */
  union {

    /// bytewise access to CSR2
    uint8_t  byte;

    /// bitwise access to register CSR2
    struct {
      BITFIELD_UINT   VREFINTF   : 1;      // bit 0
      BITFIELD_UINT   ULP        : 1;      // bit 1
      BITFIELD_UINT   FWU        : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // CSR2 bitfield

    /// register _PWR_CSR2 reset value
    #define sfr_PWR_CSR2_RESET_VALUE   ((uint8_t) 0x00)

  } CSR2;

} PWR_t;

/// access to PWR SFR registers
#define sfr_PWR   (*((PWR_t*) 0x50b2))


//------------------------
// Module PORT
//------------------------

/** struct containing PORTA module registers */
typedef struct {

  /** Port A data output latch register (ODR at 0x5000) */
  union {

    /// bytewise access to ODR
    uint8_t  byte;

    /// bitwise access to register ODR
    struct {
      BITFIELD_UINT   ODR0       : 1;      // bit 0
      BITFIELD_UINT   ODR1       : 1;      // bit 1
      BITFIELD_UINT   ODR2       : 1;      // bit 2
      BITFIELD_UINT   ODR3       : 1;      // bit 3
      BITFIELD_UINT   ODR4       : 1;      // bit 4
      BITFIELD_UINT   ODR5       : 1;      // bit 5
      BITFIELD_UINT   ODR6       : 1;      // bit 6
      BITFIELD_UINT   ODR7       : 1;      // bit 7
    };  // ODR bitfield

    /// register _PORT_ODR reset value
    #define sfr_PORT_ODR_RESET_VALUE   ((uint8_t) 0x00)

  } ODR;


  /** Port A input pin value register (IDR at 0x5001) */
  union {

    /// bytewise access to IDR
    uint8_t  byte;

    /// bitwise access to register IDR
    struct {
      BITFIELD_UINT   IDR0       : 1;      // bit 0
      BITFIELD_UINT   IDR1       : 1;      // bit 1
      BITFIELD_UINT   IDR2       : 1;      // bit 2
      BITFIELD_UINT   IDR3       : 1;      // bit 3
      BITFIELD_UINT   IDR4       : 1;      // bit 4
      BITFIELD_UINT   IDR5       : 1;      // bit 5
      BITFIELD_UINT   IDR6       : 1;      // bit 6
      BITFIELD_UINT   IDR7       : 1;      // bit 7
    };  // IDR bitfield

    /// register _PORT_IDR reset value
    #define sfr_PORT_IDR_RESET_VALUE   ((uint8_t) 0x00)

  } IDR;


  /** Port A data direction register (DDR at 0x5002) */
  union {

    /// bytewise access to DDR
    uint8_t  byte;

    /// bitwise access to register DDR
    struct {
      BITFIELD_UINT   DDR0       : 1;      // bit 0
      BITFIELD_UINT   DDR1       : 1;      // bit 1
      BITFIELD_UINT   DDR2       : 1;      // bit 2
      BITFIELD_UINT   DDR3       : 1;      // bit 3
      BITFIELD_UINT   DDR4       : 1;      // bit 4
      BITFIELD_UINT   DDR5       : 1;      // bit 5
      BITFIELD_UINT   DDR6       : 1;      // bit 6
      BITFIELD_UINT   DDR7       : 1;      // bit 7
    };  // DDR bitfield

    /// register _PORT_DDR reset value
    #define sfr_PORT_DDR_RESET_VALUE   ((uint8_t) 0x00)

  } DDR;


  /** Port A control register 1 (CR1 at 0x5003) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   C10        : 1;      // bit 0
      BITFIELD_UINT   C11        : 1;      // bit 1
      BITFIELD_UINT   C12        : 1;      // bit 2
      BITFIELD_UINT   C13        : 1;      // bit 3
      BITFIELD_UINT   C14        : 1;      // bit 4
      BITFIELD_UINT   C15        : 1;      // bit 5
      BITFIELD_UINT   C16        : 1;      // bit 6
      BITFIELD_UINT   C17        : 1;      // bit 7
    };  // CR1 bitfield

    /// register _PORT_CR1 reset value
    #define sfr_PORT_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** Port A control register 2 (CR2 at 0x5004) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   C20        : 1;      // bit 0
      BITFIELD_UINT   C21        : 1;      // bit 1
      BITFIELD_UINT   C22        : 1;      // bit 2
      BITFIELD_UINT   C23        : 1;      // bit 3
      BITFIELD_UINT   C24        : 1;      // bit 4
      BITFIELD_UINT   C25        : 1;      // bit 5
      BITFIELD_UINT   C26        : 1;      // bit 6
      BITFIELD_UINT   C27        : 1;      // bit 7
    };  // CR2 bitfield

    /// register _PORT_CR2 reset value
    #define sfr_PORT_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;

} PORT_t;

/// access to PORTA SFR registers
#define sfr_PORTA   (*((PORT_t*) 0x5000))


/// access to PORTB SFR registers
#define sfr_PORTB   (*((PORT_t*) 0x5005))


/// access to PORTC SFR registers
#define sfr_PORTC   (*((PORT_t*) 0x500a))


/// access to PORTD SFR registers
#define sfr_PORTD   (*((PORT_t*) 0x500f))


/// access to PORTE SFR registers
#define sfr_PORTE   (*((PORT_t*) 0x5014))


/// access to PORTF SFR registers
#define sfr_PORTF   (*((PORT_t*) 0x5019))


/// access to PORTG SFR registers
#define sfr_PORTG   (*((PORT_t*) 0x501e))


/// access to PORTH SFR registers
#define sfr_PORTH   (*((PORT_t*) 0x5023))


/// access to PORTI SFR registers
#define sfr_PORTI   (*((PORT_t*) 0x5028))


//------------------------
// Module RI
//------------------------

/** struct containing RI module registers */
typedef struct {

  /** Timer input capture routing register 1 (ICR1 at 0x5431) */
  union {

    /// bytewise access to ICR1
    uint8_t  byte;

    /// bitwise access to register ICR1
    struct {
      BITFIELD_UINT   IC2CS      : 5;      // bits 0-4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // ICR1 bitfield

    /// register _RI_ICR1 reset value
    #define sfr_RI_ICR1_RESET_VALUE   ((uint8_t) 0x00)

  } ICR1;


  /** Timer input capture routing register 2 (ICR2 at 0x5432) */
  union {

    /// bytewise access to ICR2
    uint8_t  byte;

    /// bitwise access to register ICR2
    struct {
      BITFIELD_UINT   IC3CS      : 5;      // bits 0-4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // ICR2 bitfield

    /// register _RI_ICR2 reset value
    #define sfr_RI_ICR2_RESET_VALUE   ((uint8_t) 0x00)

  } ICR2;


  /** I/O input register 1 (IOIR1 at 0x5433) */
  union {

    /// bytewise access to IOIR1
    uint8_t  byte;

    /// bitwise access to register IOIR1
    struct {
      BITFIELD_UINT   CH1I       : 1;      // bit 0
      BITFIELD_UINT   CH4I       : 1;      // bit 1
      BITFIELD_UINT   CH7I       : 1;      // bit 2
      BITFIELD_UINT   CH10I      : 1;      // bit 3
      BITFIELD_UINT   CH13I      : 1;      // bit 4
      BITFIELD_UINT   CH16I      : 1;      // bit 5
      BITFIELD_UINT   CH19I      : 1;      // bit 6
      BITFIELD_UINT   CH22I      : 1;      // bit 7
    };  // IOIR1 bitfield

    /// register _RI_IOIR1 reset value
    #define sfr_RI_IOIR1_RESET_VALUE   ((uint8_t) 0x00)

  } IOIR1;


  /** I/O input register 2 (IOIR2 at 0x5434) */
  union {

    /// bytewise access to IOIR2
    uint8_t  byte;

    /// bitwise access to register IOIR2
    struct {
      BITFIELD_UINT   CH2I       : 1;      // bit 0
      BITFIELD_UINT   CH5I       : 1;      // bit 1
      BITFIELD_UINT   CH8I       : 1;      // bit 2
      BITFIELD_UINT   CH11I      : 1;      // bit 3
      BITFIELD_UINT   CH14I      : 1;      // bit 4
      BITFIELD_UINT   CH17I      : 1;      // bit 5
      BITFIELD_UINT   CH20I      : 1;      // bit 6
      BITFIELD_UINT   CH23I      : 1;      // bit 7
    };  // IOIR2 bitfield

    /// register _RI_IOIR2 reset value
    #define sfr_RI_IOIR2_RESET_VALUE   ((uint8_t) 0x00)

  } IOIR2;


  /** I/O input register 3 (IOIR3 at 0x5435) */
  union {

    /// bytewise access to IOIR3
    uint8_t  byte;

    /// bitwise access to register IOIR3
    struct {
      BITFIELD_UINT   CH3I       : 1;      // bit 0
      BITFIELD_UINT   CH6I       : 1;      // bit 1
      BITFIELD_UINT   CH9I       : 1;      // bit 2
      BITFIELD_UINT   CH12I      : 1;      // bit 3
      BITFIELD_UINT   CH15I      : 1;      // bit 4
      BITFIELD_UINT   CH18I      : 1;      // bit 5
      BITFIELD_UINT   CH21I      : 1;      // bit 6
      BITFIELD_UINT   CH24I      : 1;      // bit 7
    };  // IOIR3 bitfield

    /// register _RI_IOIR3 reset value
    #define sfr_RI_IOIR3_RESET_VALUE   ((uint8_t) 0x00)

  } IOIR3;


  /** I/O control mode register 1 (IOCMR1 at 0x5436) */
  union {

    /// bytewise access to IOCMR1
    uint8_t  byte;

    /// bitwise access to register IOCMR1
    struct {
      BITFIELD_UINT   CH1M       : 1;      // bit 0
      BITFIELD_UINT   CH4M       : 1;      // bit 1
      BITFIELD_UINT   CH7M       : 1;      // bit 2
      BITFIELD_UINT   CH10M      : 1;      // bit 3
      BITFIELD_UINT   CH13M      : 1;      // bit 4
      BITFIELD_UINT   CH16M      : 1;      // bit 5
      BITFIELD_UINT   CH19M      : 1;      // bit 6
      BITFIELD_UINT   CH22M      : 1;      // bit 7
    };  // IOCMR1 bitfield

    /// register _RI_IOCMR1 reset value
    #define sfr_RI_IOCMR1_RESET_VALUE   ((uint8_t) 0x00)

  } IOCMR1;


  /** I/O control mode register 2 (IOCMR2 at 0x5437) */
  union {

    /// bytewise access to IOCMR2
    uint8_t  byte;

    /// bitwise access to register IOCMR2
    struct {
      BITFIELD_UINT   CH2M       : 1;      // bit 0
      BITFIELD_UINT   CH5M       : 1;      // bit 1
      BITFIELD_UINT   CH8M       : 1;      // bit 2
      BITFIELD_UINT   CH11M      : 1;      // bit 3
      BITFIELD_UINT   CH14M      : 1;      // bit 4
      BITFIELD_UINT   CH17M      : 1;      // bit 5
      BITFIELD_UINT   CH20M      : 1;      // bit 6
      BITFIELD_UINT   CH23M      : 1;      // bit 7
    };  // IOCMR2 bitfield

    /// register _RI_IOCMR2 reset value
    #define sfr_RI_IOCMR2_RESET_VALUE   ((uint8_t) 0x00)

  } IOCMR2;


  /** I/O control mode register 3 (IOCMR3 at 0x5438) */
  union {

    /// bytewise access to IOCMR3
    uint8_t  byte;

    /// bitwise access to register IOCMR3
    struct {
      BITFIELD_UINT   CH3M       : 1;      // bit 0
      BITFIELD_UINT   CH6M       : 1;      // bit 1
      BITFIELD_UINT   CH9M       : 1;      // bit 2
      BITFIELD_UINT   CH12M      : 1;      // bit 3
      BITFIELD_UINT   CH53M      : 1;      // bit 4
      BITFIELD_UINT   CH18M      : 1;      // bit 5
      BITFIELD_UINT   CH21M      : 1;      // bit 6
      BITFIELD_UINT   CH24M      : 1;      // bit 7
    };  // IOCMR3 bitfield

    /// register _RI_IOCMR3 reset value
    #define sfr_RI_IOCMR3_RESET_VALUE   ((uint8_t) 0x00)

  } IOCMR3;


  /** I/O switch register 1 (IOSR1 at 0x5439) */
  union {

    /// bytewise access to IOSR1
    uint8_t  byte;

    /// bitwise access to register IOSR1
    struct {
      BITFIELD_UINT   CH1E       : 1;      // bit 0
      BITFIELD_UINT   CH4E       : 1;      // bit 1
      BITFIELD_UINT   CH7E       : 1;      // bit 2
      BITFIELD_UINT   CH10E      : 1;      // bit 3
      BITFIELD_UINT   CH13E      : 1;      // bit 4
      BITFIELD_UINT   CH16E      : 1;      // bit 5
      BITFIELD_UINT   CH19E      : 1;      // bit 6
      BITFIELD_UINT   CH22E      : 1;      // bit 7
    };  // IOSR1 bitfield

    /// register _RI_IOSR1 reset value
    #define sfr_RI_IOSR1_RESET_VALUE   ((uint8_t) 0x00)

  } IOSR1;


  /** I/O switch register 2 (IOSR2 at 0x543a) */
  union {

    /// bytewise access to IOSR2
    uint8_t  byte;

    /// bitwise access to register IOSR2
    struct {
      BITFIELD_UINT   CH2E       : 1;      // bit 0
      BITFIELD_UINT   CH5E       : 1;      // bit 1
      BITFIELD_UINT   CH8E       : 1;      // bit 2
      BITFIELD_UINT   CH11E      : 1;      // bit 3
      BITFIELD_UINT   CH14E      : 1;      // bit 4
      BITFIELD_UINT   CH17E      : 1;      // bit 5
      BITFIELD_UINT   CH20E      : 1;      // bit 6
      BITFIELD_UINT   CH23E      : 1;      // bit 7
    };  // IOSR2 bitfield

    /// register _RI_IOSR2 reset value
    #define sfr_RI_IOSR2_RESET_VALUE   ((uint8_t) 0x00)

  } IOSR2;


  /** I/O switch register 3 (IOSR3 at 0x543b) */
  union {

    /// bytewise access to IOSR3
    uint8_t  byte;

    /// bitwise access to register IOSR3
    struct {
      BITFIELD_UINT   CH3E       : 1;      // bit 0
      BITFIELD_UINT   CH6E       : 1;      // bit 1
      BITFIELD_UINT   CH9E       : 1;      // bit 2
      BITFIELD_UINT   CH12E      : 1;      // bit 3
      BITFIELD_UINT   CH15E      : 1;      // bit 4
      BITFIELD_UINT   CH18E      : 1;      // bit 5
      BITFIELD_UINT   CH21E      : 1;      // bit 6
      BITFIELD_UINT   CH24E      : 1;      // bit 7
    };  // IOSR3 bitfield

    /// register _RI_IOSR3 reset value
    #define sfr_RI_IOSR3_RESET_VALUE   ((uint8_t) 0x00)

  } IOSR3;


  /** I/O group control register (IOGCR at 0x543c) */
  union {

    /// bytewise access to IOGCR
    uint8_t  byte;

    /// bitwise access to register IOGCR
    struct {
      BITFIELD_UINT   IOM1       : 2;      // bits 0-1
      BITFIELD_UINT   IOM2       : 2;      // bits 2-3
      BITFIELD_UINT   IOM3       : 2;      // bits 4-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // IOGCR bitfield

    /// register _RI_IOGCR reset value
    #define sfr_RI_IOGCR_RESET_VALUE   ((uint8_t) 0x3F)

  } IOGCR;


  /** Analog switch register 1 (ASCR1 at 0x543d) */
  union {

    /// bytewise access to ASCR1
    uint8_t  byte;

    /// bitwise access to register ASCR1
    struct {
      BITFIELD_UINT   AS0        : 1;      // bit 0
      BITFIELD_UINT   AS1        : 1;      // bit 1
      BITFIELD_UINT   AS2        : 1;      // bit 2
      BITFIELD_UINT   AS3        : 1;      // bit 3
      BITFIELD_UINT   AS4        : 1;      // bit 4
      BITFIELD_UINT   AS5        : 1;      // bit 5
      BITFIELD_UINT   AS6        : 1;      // bit 6
      BITFIELD_UINT   AS7        : 1;      // bit 7
    };  // ASCR1 bitfield

    /// register _RI_ASCR1 reset value
    #define sfr_RI_ASCR1_RESET_VALUE   ((uint8_t) 0x00)

  } ASCR1;


  /** Analog switch register 2 (ASCR2 at 0x543e) */
  union {

    /// bytewise access to ASCR2
    uint8_t  byte;

    /// bitwise access to register ASCR2
    struct {
      BITFIELD_UINT   AS8        : 1;      // bit 0
      BITFIELD_UINT              : 5;      // 5 bits
      BITFIELD_UINT   AS14       : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // ASCR2 bitfield

    /// register _RI_ASCR2 reset value
    #define sfr_RI_ASCR2_RESET_VALUE   ((uint8_t) 0x00)

  } ASCR2;


  /** Resistor control register 1 (RCR at 0x543f) */
  union {

    /// bytewise access to RCR
    uint8_t  byte;

    /// bitwise access to register RCR
    struct {
      BITFIELD_UINT   KPU10      : 1;      // bit 0
      BITFIELD_UINT   KPU400     : 1;      // bit 1
      BITFIELD_UINT   KPD10      : 1;      // bit 2
      BITFIELD_UINT   KPD400     : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // RCR bitfield

    /// register _RI_RCR reset value
    #define sfr_RI_RCR_RESET_VALUE   ((uint8_t) 0x00)

  } RCR;

} RI_t;

/// access to RI SFR registers
#define sfr_RI   (*((RI_t*) 0x5431))


//------------------------
// Module RST
//------------------------

/** struct containing RST module registers */
typedef struct {

  /** Reset control register (CR at 0x50b0) */
  union {

    /// bytewise access to CR
    uint8_t  byte;

    /// bitwise access to register CR
    struct {
      BITFIELD_UINT   RSTPIN_KEY : 8;      // bits 0-7
    };  // CR bitfield

    /// register _RST_CR reset value
    #define sfr_RST_CR_RESET_VALUE   ((uint8_t) 0x00)

  } CR;


  /** Reset status register (SR at 0x50b1) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   PORF       : 1;      // bit 0
      BITFIELD_UINT   IWDGF      : 1;      // bit 1
      BITFIELD_UINT   ILLOPF     : 1;      // bit 2
      BITFIELD_UINT   SWIMF      : 1;      // bit 3
      BITFIELD_UINT   WWDGF      : 1;      // bit 4
      BITFIELD_UINT   BORF       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // SR bitfield

    /// register _RST_SR reset value
    #define sfr_RST_SR_RESET_VALUE   ((uint8_t) 0x01)

  } SR;

} RST_t;

/// access to RST SFR registers
#define sfr_RST   (*((RST_t*) 0x50b0))


//------------------------
// Module RTC
//------------------------

/** struct containing RTC module registers */
typedef struct {

  /** Time register 1 (TR1 at 0x5140) */
  union {

    /// bytewise access to TR1
    uint8_t  byte;

    /// bitwise access to register TR1
    struct {
      BITFIELD_UINT   SU         : 4;      // bits 0-3
      BITFIELD_UINT   ST         : 4;      // bits 4-7
    };  // TR1 bitfield

    /// register _RTC_TR1 reset value
    #define sfr_RTC_TR1_RESET_VALUE   ((uint8_t) 0x00)

  } TR1;


  /** Time register 2 (TR2 at 0x5141) */
  union {

    /// bytewise access to TR2
    uint8_t  byte;

    /// bitwise access to register TR2
    struct {
      BITFIELD_UINT   MNU        : 4;      // bits 0-3
      BITFIELD_UINT   MNT        : 4;      // bits 4-7
    };  // TR2 bitfield

    /// register _RTC_TR2 reset value
    #define sfr_RTC_TR2_RESET_VALUE   ((uint8_t) 0x00)

  } TR2;


  /** Time register 3 (TR3 at 0x5142) */
  union {

    /// bytewise access to TR3
    uint8_t  byte;

    /// bitwise access to register TR3
    struct {
      BITFIELD_UINT   HU         : 4;      // bits 0-3
      BITFIELD_UINT   HT         : 2;      // bits 4-5
      BITFIELD_UINT   PM         : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // TR3 bitfield

    /// register _RTC_TR3 reset value
    #define sfr_RTC_TR3_RESET_VALUE   ((uint8_t) 0x00)

  } TR3;


  /// Reserved register (1B)
  uint8_t     Reserved_1[1];


  /** Date register 1 (DR1 at 0x5144) */
  union {

    /// bytewise access to DR1
    uint8_t  byte;

    /// bitwise access to register DR1
    struct {
      BITFIELD_UINT   DU         : 4;      // bits 0-3
      BITFIELD_UINT   DT         : 2;      // bits 4-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // DR1 bitfield

    /// register _RTC_DR1 reset value
    #define sfr_RTC_DR1_RESET_VALUE   ((uint8_t) 0x00)

  } DR1;


  /** Date register 2 (DR2 at 0x5145) */
  union {

    /// bytewise access to DR2
    uint8_t  byte;

    /// bitwise access to register DR2
    struct {
      BITFIELD_UINT   MU         : 4;      // bits 0-3
      BITFIELD_UINT   MT         : 1;      // bit 4
      BITFIELD_UINT   WDU        : 3;      // bits 5-7
    };  // DR2 bitfield

    /// register _RTC_DR2 reset value
    #define sfr_RTC_DR2_RESET_VALUE   ((uint8_t) 0x00)

  } DR2;


  /** Date register 3 (DR3 at 0x5146) */
  union {

    /// bytewise access to DR3
    uint8_t  byte;

    /// bitwise access to register DR3
    struct {
      BITFIELD_UINT   YU         : 4;      // bits 0-3
      BITFIELD_UINT   YT         : 4;      // bits 4-7
    };  // DR3 bitfield

    /// register _RTC_DR3 reset value
    #define sfr_RTC_DR3_RESET_VALUE   ((uint8_t) 0x00)

  } DR3;


  /// Reserved register (1B)
  uint8_t     Reserved_2[1];


  /** Control register 1 (CR1 at 0x5148) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   WUCKSEL    : 4;      // bits 0-3
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   RATIO      : 1;      // bit 5
      BITFIELD_UINT   FMT        : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CR1 bitfield

    /// register _RTC_CR1 reset value
    #define sfr_RTC_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** Control register 2 (CR2 at 0x5149) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   ALRAE      : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   WUTE       : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   ALRAIE     : 1;      // bit 4
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   WUTIE      : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CR2 bitfield

    /// register _RTC_CR2 reset value
    #define sfr_RTC_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** Control register 3 (CR3 at 0x514a) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   ADD1H      : 1;      // bit 0
      BITFIELD_UINT   SUB1H      : 1;      // bit 1
      BITFIELD_UINT   BCK        : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   POL        : 1;      // bit 4
      BITFIELD_UINT   OSEL       : 2;      // bits 5-6
      BITFIELD_UINT   COE        : 1;      // bit 7
    };  // CR3 bitfield

    /// register _RTC_CR3 reset value
    #define sfr_RTC_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /// Reserved register (1B)
  uint8_t     Reserved_3[1];


  /** Initialization and status register 1 (ISR1 at 0x514c) */
  union {

    /// bytewise access to ISR1
    uint8_t  byte;

    /// bitwise access to register ISR1
    struct {
      BITFIELD_UINT   ALRAWF     : 1;      // bit 0
      BITFIELD_UINT   RECALPF    : 1;      // bit 1
      BITFIELD_UINT   WUTWF      : 1;      // bit 2
      BITFIELD_UINT   SHPF       : 1;      // bit 3
      BITFIELD_UINT   INITS      : 1;      // bit 4
      BITFIELD_UINT   RSF        : 1;      // bit 5
      BITFIELD_UINT   INITF      : 1;      // bit 6
      BITFIELD_UINT   INIT       : 1;      // bit 7
    };  // ISR1 bitfield

    /// register _RTC_ISR1 reset value
    #define sfr_RTC_ISR1_RESET_VALUE   ((uint8_t) 0x00)

  } ISR1;


  /** Initialization and Status register 2 (ISR2 at 0x514d) */
  union {

    /// bytewise access to ISR2
    uint8_t  byte;

    /// bitwise access to register ISR2
    struct {
      BITFIELD_UINT   ALRAF      : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   WUTF       : 1;      // bit 2
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   TAMP1F     : 1;      // bit 5
      BITFIELD_UINT   TAMP2F     : 1;      // bit 6
      BITFIELD_UINT   TAMP3F     : 1;      // bit 7
    };  // ISR2 bitfield

    /// register _RTC_ISR2 reset value
    #define sfr_RTC_ISR2_RESET_VALUE   ((uint8_t) 0x00)

  } ISR2;


  /// Reserved register (2B)
  uint8_t     Reserved_4[2];


  /** Synchronous prescaler register high (SPRERH at 0x5150) */
  union {

    /// bytewise access to SPRERH
    uint8_t  byte;

    /// bitwise access to register SPRERH
    struct {
      BITFIELD_UINT   PREDIV_S8  : 1;      // bit 0
      BITFIELD_UINT   PREDIV_S9  : 1;      // bit 1
      BITFIELD_UINT   PREDIV_S10 : 1;      // bit 2
      BITFIELD_UINT   PREDIV_S11 : 1;      // bit 3
      BITFIELD_UINT   PREDIV_S12 : 1;      // bit 4
      BITFIELD_UINT   PREDIV_S13 : 1;      // bit 5
      BITFIELD_UINT   PREDIV_S14 : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // SPRERH bitfield

    /// register _RTC_SPRERH reset value
    #define sfr_RTC_SPRERH_RESET_VALUE   ((uint8_t) 0x00)

  } SPRERH;


  /** Synchronous prescaler register low (SPRERL at 0x5151) */
  union {

    /// bytewise access to SPRERL
    uint8_t  byte;

    /// bitwise access to register SPRERL
    struct {
      BITFIELD_UINT   PREDIV_S0  : 1;      // bit 0
      BITFIELD_UINT   PREDIV_S1  : 1;      // bit 1
      BITFIELD_UINT   PREDIV_S2  : 1;      // bit 2
      BITFIELD_UINT   PREDIV_S3  : 1;      // bit 3
      BITFIELD_UINT   PREDIV_S4  : 1;      // bit 4
      BITFIELD_UINT   PREDIV_S5  : 1;      // bit 5
      BITFIELD_UINT   PREDIV_S6  : 1;      // bit 6
      BITFIELD_UINT   PREDIV_S7  : 1;      // bit 7
    };  // SPRERL bitfield

    /// register _RTC_SPRERL reset value
    #define sfr_RTC_SPRERL_RESET_VALUE   ((uint8_t) 0x00)

  } SPRERL;


  /** Asynchronous prescaler register (APRER at 0x5152) */
  union {

    /// bytewise access to APRER
    uint8_t  byte;

    /// bitwise access to register APRER
    struct {
      BITFIELD_UINT   PREDIV_A   : 7;      // bits 0-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // APRER bitfield

    /// register _RTC_APRER reset value
    #define sfr_RTC_APRER_RESET_VALUE   ((uint8_t) 0x00)

  } APRER;


  /// Reserved register (1B)
  uint8_t     Reserved_5[1];


  /** Wakeup timer register high (WUTRH at 0x5154) */
  union {

    /// bytewise access to WUTRH
    uint8_t  byte;

    /// bitwise access to register WUTRH
    struct {
      BITFIELD_UINT   WUT8       : 1;      // bit 0
      BITFIELD_UINT   WUT9       : 1;      // bit 1
      BITFIELD_UINT   WUT10      : 1;      // bit 2
      BITFIELD_UINT   WUT11      : 1;      // bit 3
      BITFIELD_UINT   WUT12      : 1;      // bit 4
      BITFIELD_UINT   WUT13      : 1;      // bit 5
      BITFIELD_UINT   WUT14      : 1;      // bit 6
      BITFIELD_UINT   WUT15      : 1;      // bit 7
    };  // WUTRH bitfield

    /// register _RTC_WUTRH reset value
    #define sfr_RTC_WUTRH_RESET_VALUE   ((uint8_t) 0x00)

  } WUTRH;


  /** Wakeup timer register low (WUTRL at 0x5155) */
  union {

    /// bytewise access to WUTRL
    uint8_t  byte;

    /// bitwise access to register WUTRL
    struct {
      BITFIELD_UINT   WUT0       : 1;      // bit 0
      BITFIELD_UINT   WUT1       : 1;      // bit 1
      BITFIELD_UINT   WUT2       : 1;      // bit 2
      BITFIELD_UINT   WUT3       : 1;      // bit 3
      BITFIELD_UINT   WUT4       : 1;      // bit 4
      BITFIELD_UINT   WUT5       : 1;      // bit 5
      BITFIELD_UINT   WUT6       : 1;      // bit 6
      BITFIELD_UINT   WUT7       : 1;      // bit 7
    };  // WUTRL bitfield

    /// register _RTC_WUTRL reset value
    #define sfr_RTC_WUTRL_RESET_VALUE   ((uint8_t) 0x00)

  } WUTRL;


  /// Reserved register (1B)
  uint8_t     Reserved_6[1];


  /** Subsecond register low (SSRL at 0x5157) */
  union {

    /// bytewise access to SSRL
    uint8_t  byte;

    /// bitwise access to register SSRL
    struct {
      BITFIELD_UINT   SS0        : 1;      // bit 0
      BITFIELD_UINT   SS1        : 1;      // bit 1
      BITFIELD_UINT   SS2        : 1;      // bit 2
      BITFIELD_UINT   SS3        : 1;      // bit 3
      BITFIELD_UINT   SS4        : 1;      // bit 4
      BITFIELD_UINT   SS5        : 1;      // bit 5
      BITFIELD_UINT   SS6        : 1;      // bit 6
      BITFIELD_UINT   SS7        : 1;      // bit 7
    };  // SSRL bitfield

    /// register _RTC_SSRL reset value
    #define sfr_RTC_SSRL_RESET_VALUE   ((uint8_t) 0x00)

  } SSRL;


  /** Subsecond register high (SSRH at 0x5158) */
  union {

    /// bytewise access to SSRH
    uint8_t  byte;

    /// bitwise access to register SSRH
    struct {
      BITFIELD_UINT   SS8        : 1;      // bit 0
      BITFIELD_UINT   SS9        : 1;      // bit 1
      BITFIELD_UINT   SS10       : 1;      // bit 2
      BITFIELD_UINT   SS11       : 1;      // bit 3
      BITFIELD_UINT   SS12       : 1;      // bit 4
      BITFIELD_UINT   SS13       : 1;      // bit 5
      BITFIELD_UINT   SS14       : 1;      // bit 6
      BITFIELD_UINT   SS15       : 1;      // bit 7
    };  // SSRH bitfield

    /// register _RTC_SSRH reset value
    #define sfr_RTC_SSRH_RESET_VALUE   ((uint8_t) 0x00)

  } SSRH;


  /** Write protection register (WPR at 0x5159) */
  union {

    /// bytewise access to WPR
    uint8_t  byte;

    /// bitwise access to register WPR
    struct {
      BITFIELD_UINT   KEY0       : 1;      // bit 0
      BITFIELD_UINT   KEY1       : 1;      // bit 1
      BITFIELD_UINT   KEY2       : 1;      // bit 2
      BITFIELD_UINT   KEY3       : 1;      // bit 3
      BITFIELD_UINT   KEY4       : 1;      // bit 4
      BITFIELD_UINT   KEY5       : 1;      // bit 5
      BITFIELD_UINT   KEY6       : 1;      // bit 6
      BITFIELD_UINT   KEY7       : 1;      // bit 7
    };  // WPR bitfield

    /// register _RTC_WPR reset value
    #define sfr_RTC_WPR_RESET_VALUE   ((uint8_t) 0x00)

  } WPR;


  /** Shift register high (SHIFTRH at 0x515a) */
  union {

    /// bytewise access to SHIFTRH
    uint8_t  byte;

    /// bitwise access to register SHIFTRH
    struct {
      BITFIELD_UINT   SUBFS8     : 1;      // bit 0
      BITFIELD_UINT   SUBFS9     : 1;      // bit 1
      BITFIELD_UINT   SUBFS10    : 1;      // bit 2
      BITFIELD_UINT   SUBFS11    : 1;      // bit 3
      BITFIELD_UINT   SUBFS12    : 1;      // bit 4
      BITFIELD_UINT   SUBFS13    : 1;      // bit 5
      BITFIELD_UINT   SUBFS14    : 1;      // bit 6
      BITFIELD_UINT   ADDIS      : 1;      // bit 7
    };  // SHIFTRH bitfield

    /// register _RTC_SHIFTRH reset value
    #define sfr_RTC_SHIFTRH_RESET_VALUE   ((uint8_t) 0x00)

  } SHIFTRH;


  /** Shift register low (SHIFTRL at 0x515b) */
  union {

    /// bytewise access to SHIFTRL
    uint8_t  byte;

    /// bitwise access to register SHIFTRL
    struct {
      BITFIELD_UINT   SUBFS0     : 1;      // bit 0
      BITFIELD_UINT   SUBFS1     : 1;      // bit 1
      BITFIELD_UINT   SUBFS2     : 1;      // bit 2
      BITFIELD_UINT   SUBFS3     : 1;      // bit 3
      BITFIELD_UINT   SUBFS4     : 1;      // bit 4
      BITFIELD_UINT   SUBFS5     : 1;      // bit 5
      BITFIELD_UINT   SUBFS6     : 1;      // bit 6
      BITFIELD_UINT   SUBFS7     : 1;      // bit 7
    };  // SHIFTRL bitfield

    /// register _RTC_SHIFTRL reset value
    #define sfr_RTC_SHIFTRL_RESET_VALUE   ((uint8_t) 0x00)

  } SHIFTRL;


  /** Alarm A register 1 (ALRMAR1 at 0x515c) */
  union {

    /// bytewise access to ALRMAR1
    uint8_t  byte;

    /// bitwise access to register ALRMAR1
    struct {
      BITFIELD_UINT   ALSU       : 4;      // bits 0-3
      BITFIELD_UINT   ALST       : 3;      // bits 4-6
      BITFIELD_UINT   MSK1       : 1;      // bit 7
    };  // ALRMAR1 bitfield

    /// register _RTC_ALRMAR1 reset value
    #define sfr_RTC_ALRMAR1_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMAR1;


  /** Alarm A register 2 (ALRMAR2 at 0x515d) */
  union {

    /// bytewise access to ALRMAR2
    uint8_t  byte;

    /// bitwise access to register ALRMAR2
    struct {
      BITFIELD_UINT   ALMNU      : 4;      // bits 0-3
      BITFIELD_UINT   ALMNT      : 3;      // bits 4-6
      BITFIELD_UINT   MSK2       : 1;      // bit 7
    };  // ALRMAR2 bitfield

    /// register _RTC_ALRMAR2 reset value
    #define sfr_RTC_ALRMAR2_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMAR2;


  /** Alarm A register 3 (ALRMAR3 at 0x515e) */
  union {

    /// bytewise access to ALRMAR3
    uint8_t  byte;

    /// bitwise access to register ALRMAR3
    struct {
      BITFIELD_UINT   ALHU       : 4;      // bits 0-3
      BITFIELD_UINT   ALHT       : 2;      // bits 4-5
      BITFIELD_UINT   PM         : 1;      // bit 6
      BITFIELD_UINT   MSK3       : 1;      // bit 7
    };  // ALRMAR3 bitfield

    /// register _RTC_ALRMAR3 reset value
    #define sfr_RTC_ALRMAR3_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMAR3;


  /** Alarm A register 4 (ALRMAR4 at 0x515f) */
  union {

    /// bytewise access to ALRMAR4
    uint8_t  byte;

    /// bitwise access to register ALRMAR4
    struct {
      BITFIELD_UINT   ALDU       : 4;      // bits 0-3
      BITFIELD_UINT   ALDT       : 2;      // bits 4-5
      BITFIELD_UINT   WDSEL      : 1;      // bit 6
      BITFIELD_UINT   MSK4       : 1;      // bit 7
    };  // ALRMAR4 bitfield

    /// register _RTC_ALRMAR4 reset value
    #define sfr_RTC_ALRMAR4_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMAR4;


  /// Reserved register (4B)
  uint8_t     Reserved_7[4];


  /** Alarm A subsecond register high (ALRMASSRH at 0x5164) */
  union {

    /// bytewise access to ALRMASSRH
    uint8_t  byte;

    /// bitwise access to register ALRMASSRH
    struct {
      BITFIELD_UINT   ALSS8      : 1;      // bit 0
      BITFIELD_UINT   ALSS9      : 1;      // bit 1
      BITFIELD_UINT   ALSS10     : 1;      // bit 2
      BITFIELD_UINT   ALSS11     : 1;      // bit 3
      BITFIELD_UINT   ALSS12     : 1;      // bit 4
      BITFIELD_UINT   ALSS13     : 1;      // bit 5
      BITFIELD_UINT   ALSS14     : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // ALRMASSRH bitfield

    /// register _RTC_ALRMASSRH reset value
    #define sfr_RTC_ALRMASSRH_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMASSRH;


  /** Alarm A subsecond register low (ALRMASSRL at 0x5165) */
  union {

    /// bytewise access to ALRMASSRL
    uint8_t  byte;

    /// bitwise access to register ALRMASSRL
    struct {
      BITFIELD_UINT   ALSS0      : 1;      // bit 0
      BITFIELD_UINT   ALSS1      : 1;      // bit 1
      BITFIELD_UINT   ALSS2      : 1;      // bit 2
      BITFIELD_UINT   ALSS3      : 1;      // bit 3
      BITFIELD_UINT   ALSS4      : 1;      // bit 4
      BITFIELD_UINT   ALSS5      : 1;      // bit 5
      BITFIELD_UINT   ALSS6      : 1;      // bit 6
      BITFIELD_UINT   ALSS7      : 1;      // bit 7
    };  // ALRMASSRL bitfield

    /// register _RTC_ALRMASSRL reset value
    #define sfr_RTC_ALRMASSRL_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMASSRL;


  /** Alarm A masking register (ALRMASSMSKR at 0x5166) */
  union {

    /// bytewise access to ALRMASSMSKR
    uint8_t  byte;

    /// bitwise access to register ALRMASSMSKR
    struct {
      BITFIELD_UINT   MASKSS0    : 1;      // bit 0
      BITFIELD_UINT   MASKSS1    : 1;      // bit 1
      BITFIELD_UINT   MASKSS2    : 1;      // bit 2
      BITFIELD_UINT   MASKSS3    : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // ALRMASSMSKR bitfield

    /// register _RTC_ALRMASSMSKR reset value
    #define sfr_RTC_ALRMASSMSKR_RESET_VALUE   ((uint8_t) 0x00)

  } ALRMASSMSKR;


  /// Reserved register (3B)
  uint8_t     Reserved_8[3];


  /** Calibration register high (CALRH at 0x516a) */
  union {

    /// bytewise access to CALRH
    uint8_t  byte;

    /// bitwise access to register CALRH
    struct {
      BITFIELD_UINT   CALM8      : 1;      // bit 0
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   CALW16     : 1;      // bit 5
      BITFIELD_UINT   CALW8      : 1;      // bit 6
      BITFIELD_UINT   CALP       : 1;      // bit 7
    };  // CALRH bitfield

    /// register _RTC_CALRH reset value
    #define sfr_RTC_CALRH_RESET_VALUE   ((uint8_t) 0x00)

  } CALRH;


  /** Calibration register low (CALRL at 0x516b) */
  union {

    /// bytewise access to CALRL
    uint8_t  byte;

    /// bitwise access to register CALRL
    struct {
      BITFIELD_UINT   CALM0      : 1;      // bit 0
      BITFIELD_UINT   CALM1      : 1;      // bit 1
      BITFIELD_UINT   CALM2      : 1;      // bit 2
      BITFIELD_UINT   CALM3      : 1;      // bit 3
      BITFIELD_UINT   CALM4      : 1;      // bit 4
      BITFIELD_UINT   CALM5      : 1;      // bit 5
      BITFIELD_UINT   CALM6      : 1;      // bit 6
      BITFIELD_UINT   CALM7      : 1;      // bit 7
    };  // CALRL bitfield

    /// register _RTC_CALRL reset value
    #define sfr_RTC_CALRL_RESET_VALUE   ((uint8_t) 0x00)

  } CALRL;


  /** Tamper control register 1 (TCR1 at 0x516c) */
  union {

    /// bytewise access to TCR1
    uint8_t  byte;

    /// bitwise access to register TCR1
    struct {
      BITFIELD_UINT   TAMPIE     : 1;      // bit 0
      BITFIELD_UINT   TAMP1E     : 1;      // bit 1
      BITFIELD_UINT   TAMP1LEVEL : 1;      // bit 2
      BITFIELD_UINT   TAMP2E     : 1;      // bit 3
      BITFIELD_UINT   TAMP2LEVEL : 1;      // bit 4
      BITFIELD_UINT   TAMP3E     : 1;      // bit 5
      BITFIELD_UINT   TAMP3LEVEL : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // TCR1 bitfield

    /// register _RTC_TCR1 reset value
    #define sfr_RTC_TCR1_RESET_VALUE   ((uint8_t) 0x00)

  } TCR1;


  /** Tamper control register 2 (TCR2 at 0x516d) */
  union {

    /// bytewise access to TCR2
    uint8_t  byte;

    /// bitwise access to register TCR2
    struct {
      BITFIELD_UINT   TAMPFREQ   : 3;      // bits 0-2
      BITFIELD_UINT   TAMPFLT    : 2;      // bits 3-4
      BITFIELD_UINT   TAMPPRCH   : 2;      // bits 5-6
      BITFIELD_UINT   TAMPPUDIS  : 1;      // bit 7
    };  // TCR2 bitfield

    /// register _RTC_TCR2 reset value
    #define sfr_RTC_TCR2_RESET_VALUE   ((uint8_t) 0x00)

  } TCR2;


  /// Reserved register (34B)
  uint8_t     Reserved_9[34];


  /** CSS on LSE control and status register (CSS_LSE_CSR at 0x5190) */
  union {

    /// bytewise access to CSS_LSE_CSR
    uint8_t  byte;

    /// bitwise access to register CSS_LSE_CSR
    struct {
      BITFIELD_UINT   CSSEN      : 1;      // bit 0
      BITFIELD_UINT   SWITCHEN   : 1;      // bit 1
      BITFIELD_UINT   CSSIE      : 1;      // bit 2
      BITFIELD_UINT   CSSF       : 1;      // bit 3
      BITFIELD_UINT   SWITCHF    : 1;      // bit 4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // CSS_LSE_CSR bitfield

    /// register _RTC_CSS_LSE_CSR reset value
    #define sfr_RTC_CSS_LSE_CSR_RESET_VALUE   ((uint8_t) 0x00)

  } CSS_LSE_CSR;

} RTC_t;

/// access to RTC SFR registers
#define sfr_RTC   (*((RTC_t*) 0x5140))


//------------------------
// Module SPI1
//------------------------

/** struct containing SPI1 module registers */
typedef struct {

  /** SPI1 control register 1 (CR1 at 0x5200) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CPHA       : 1;      // bit 0
      BITFIELD_UINT   CPOL       : 1;      // bit 1
      BITFIELD_UINT   MSTR       : 1;      // bit 2
      BITFIELD_UINT   BR         : 3;      // bits 3-5
      BITFIELD_UINT   SPE        : 1;      // bit 6
      BITFIELD_UINT   LSBFIRST   : 1;      // bit 7
    };  // CR1 bitfield

    /// register _SPI1_CR1 reset value
    #define sfr_SPI1_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** SPI1 control register 2 (CR2 at 0x5201) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SSI        : 1;      // bit 0
      BITFIELD_UINT   SSM        : 1;      // bit 1
      BITFIELD_UINT   RXONLY     : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CRCNEXT    : 1;      // bit 4
      BITFIELD_UINT   CRCEN      : 1;      // bit 5
      BITFIELD_UINT   BDOE       : 1;      // bit 6
      BITFIELD_UINT   BDM        : 1;      // bit 7
    };  // CR2 bitfield

    /// register _SPI1_CR2 reset value
    #define sfr_SPI1_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** SPI1 interrupt control register (ICR at 0x5202) */
  union {

    /// bytewise access to ICR
    uint8_t  byte;

    /// bitwise access to register ICR
    struct {
      BITFIELD_UINT   RXDMAEN    : 1;      // bit 0
      BITFIELD_UINT   TXDMAEN    : 1;      // bit 1
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   WKIE       : 1;      // bit 4
      BITFIELD_UINT   ERRIE      : 1;      // bit 5
      BITFIELD_UINT   RXIE       : 1;      // bit 6
      BITFIELD_UINT   TXIE       : 1;      // bit 7
    };  // ICR bitfield

    /// register _SPI1_ICR reset value
    #define sfr_SPI1_ICR_RESET_VALUE   ((uint8_t) 0x00)

  } ICR;


  /** SPI1 status register (SR at 0x5203) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   RXNE       : 1;      // bit 0
      BITFIELD_UINT   TXE        : 1;      // bit 1
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   WKUP       : 1;      // bit 3
      BITFIELD_UINT   CRCERR     : 1;      // bit 4
      BITFIELD_UINT   MODF       : 1;      // bit 5
      BITFIELD_UINT   OVR        : 1;      // bit 6
      BITFIELD_UINT   BSY        : 1;      // bit 7
    };  // SR bitfield

    /// register _SPI1_SR reset value
    #define sfr_SPI1_SR_RESET_VALUE   ((uint8_t) 0x02)

  } SR;


  /** SPI1 data register (DR at 0x5204) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _SPI1_DR reset value
    #define sfr_SPI1_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** SPI1 CRC polynomial register (CRCPR at 0x5205) */
  union {

    /// bytewise access to CRCPR
    uint8_t  byte;

    /// bitwise access to register CRCPR
    struct {
      BITFIELD_UINT   CRCPOLY    : 8;      // bits 0-7
    };  // CRCPR bitfield

    /// register _SPI1_CRCPR reset value
    #define sfr_SPI1_CRCPR_RESET_VALUE   ((uint8_t) 0x07)

  } CRCPR;


  /** SPI1 Rx CRC register (RXCRCR at 0x5206) */
  union {

    /// bytewise access to RXCRCR
    uint8_t  byte;

    /// bitwise access to register RXCRCR
    struct {
      BITFIELD_UINT   RXCRC      : 8;      // bits 0-7
    };  // RXCRCR bitfield

    /// register _SPI1_RXCRCR reset value
    #define sfr_SPI1_RXCRCR_RESET_VALUE   ((uint8_t) 0x00)

  } RXCRCR;


  /** SPI1 Tx CRC register (TXCRCR at 0x5207) */
  union {

    /// bytewise access to TXCRCR
    uint8_t  byte;

    /// bitwise access to register TXCRCR
    struct {
      BITFIELD_UINT   TXCRC      : 7;      // bits 0-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // TXCRCR bitfield

    /// register _SPI1_TXCRCR reset value
    #define sfr_SPI1_TXCRCR_RESET_VALUE   ((uint8_t) 0x00)

  } TXCRCR;

} SPI1_t;

/// access to SPI1 SFR registers
#define sfr_SPI1   (*((SPI1_t*) 0x5200))


//------------------------
// Module SPI2
//------------------------

/** struct containing SPI2 module registers */
typedef struct {

  /** SPI2 control register 1 (CR1 at 0x53c0) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CPHA       : 1;      // bit 0
      BITFIELD_UINT   CPOL       : 1;      // bit 1
      BITFIELD_UINT   MSTR       : 1;      // bit 2
      BITFIELD_UINT   BR         : 3;      // bits 3-5
      BITFIELD_UINT   SPE        : 1;      // bit 6
      BITFIELD_UINT   LSBFIRST   : 1;      // bit 7
    };  // CR1 bitfield

    /// register _SPI2_CR1 reset value
    #define sfr_SPI2_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** SPI2 control register 2 (CR2 at 0x53c1) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SSI        : 1;      // bit 0
      BITFIELD_UINT   SSM        : 1;      // bit 1
      BITFIELD_UINT   RXONLY     : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CRCNEXT    : 1;      // bit 4
      BITFIELD_UINT   CRCEN      : 1;      // bit 5
      BITFIELD_UINT   BDOE       : 1;      // bit 6
      BITFIELD_UINT   BDM        : 1;      // bit 7
    };  // CR2 bitfield

    /// register _SPI2_CR2 reset value
    #define sfr_SPI2_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** SPI2 interrupt control register (ICR at 0x53c2) */
  union {

    /// bytewise access to ICR
    uint8_t  byte;

    /// bitwise access to register ICR
    struct {
      BITFIELD_UINT   RXDMAEN    : 1;      // bit 0
      BITFIELD_UINT   TXDMAEN    : 1;      // bit 1
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   WKIE       : 1;      // bit 4
      BITFIELD_UINT   ERRIE      : 1;      // bit 5
      BITFIELD_UINT   RXIE       : 1;      // bit 6
      BITFIELD_UINT   TXIE       : 1;      // bit 7
    };  // ICR bitfield

    /// register _SPI2_ICR reset value
    #define sfr_SPI2_ICR_RESET_VALUE   ((uint8_t) 0x00)

  } ICR;


  /** SPI2 status register (SR at 0x53c3) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   RXNE       : 1;      // bit 0
      BITFIELD_UINT   TXE        : 1;      // bit 1
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   WKUP       : 1;      // bit 3
      BITFIELD_UINT   CRCERR     : 1;      // bit 4
      BITFIELD_UINT   MODF       : 1;      // bit 5
      BITFIELD_UINT   OVR        : 1;      // bit 6
      BITFIELD_UINT   BSY        : 1;      // bit 7
    };  // SR bitfield

    /// register _SPI2_SR reset value
    #define sfr_SPI2_SR_RESET_VALUE   ((uint8_t) 0x02)

  } SR;


  /** SPI2 data register (DR at 0x53c4) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _SPI2_DR reset value
    #define sfr_SPI2_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** SPI2 CRC polynomial register (CRCPR at 0x53c5) */
  union {

    /// bytewise access to CRCPR
    uint8_t  byte;

    /// bitwise access to register CRCPR
    struct {
      BITFIELD_UINT   CRCPOLY    : 8;      // bits 0-7
    };  // CRCPR bitfield

    /// register _SPI2_CRCPR reset value
    #define sfr_SPI2_CRCPR_RESET_VALUE   ((uint8_t) 0x07)

  } CRCPR;


  /** SPI2 Rx CRC register (RXCRCR at 0x53c6) */
  union {

    /// bytewise access to RXCRCR
    uint8_t  byte;

    /// bitwise access to register RXCRCR
    struct {
      BITFIELD_UINT   RXCRC      : 8;      // bits 0-7
    };  // RXCRCR bitfield

    /// register _SPI2_RXCRCR reset value
    #define sfr_SPI2_RXCRCR_RESET_VALUE   ((uint8_t) 0x00)

  } RXCRCR;


  /** SPI2 Tx CRC register (TXCRCR at 0x53c7) */
  union {

    /// bytewise access to TXCRCR
    uint8_t  byte;

    /// bitwise access to register TXCRCR
    struct {
      BITFIELD_UINT   TXCRC      : 7;      // bits 0-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // TXCRCR bitfield

    /// register _SPI2_TXCRCR reset value
    #define sfr_SPI2_TXCRCR_RESET_VALUE   ((uint8_t) 0x00)

  } TXCRCR;

} SPI2_t;

/// access to SPI2 SFR registers
#define sfr_SPI2   (*((SPI2_t*) 0x53c0))


//------------------------
// Module SWIM
//------------------------

/** struct containing SWIM module registers */
typedef struct {

  /** SWIM control status register (CSR at 0x7f80) */
  union {

    /// bytewise access to CSR
    uint8_t  byte;

    /// skip bitwise access to register CSR

    /// register _SWIM_CSR reset value
    #define sfr_SWIM_CSR_RESET_VALUE   ((uint8_t) 0x00)

  } CSR;

} SWIM_t;

/// access to SWIM SFR registers
#define sfr_SWIM   (*((SWIM_t*) 0x7f80))


//------------------------
// Module SYSCFG
//------------------------

/** struct containing SYSCFG module registers */
typedef struct {

  /** Remapping register 3 (RMPCR3 at 0x509d) */
  union {

    /// bytewise access to RMPCR3
    uint8_t  byte;

    /// bitwise access to register RMPCR3
    struct {
      BITFIELD_UINT   SPI1_REMAP2: 1;      // bit 0
      BITFIELD_UINT   USART3TR_REMAP: 1;      // bit 1
      BITFIELD_UINT   USART3CK_REMAP: 1;      // bit 2
      BITFIELD_UINT   TIM3_CH1_REMAP: 1;      // bit 3
      BITFIELD_UINT   TIM3_CH2_REMAP: 1;      // bit 4
      BITFIELD_UINT   CCO_REMAP  : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // RMPCR3 bitfield

    /// register _SYSCFG_RMPCR3 reset value
    #define sfr_SYSCFG_RMPCR3_RESET_VALUE   ((uint8_t) 0x00)

  } RMPCR3;


  /** Remapping register 1 (RMPCR1 at 0x509e) */
  union {

    /// bytewise access to RMPCR1
    uint8_t  byte;

    /// bitwise access to register RMPCR1
    struct {
      BITFIELD_UINT   ADC1DMA_REMAP: 2;      // bits 0-1
      BITFIELD_UINT   TIM4DMA_REMAP: 2;      // bits 2-3
      BITFIELD_UINT   USART1TR_REMAP: 2;      // bits 4-5
      BITFIELD_UINT   USART1CK_REMAP: 1;      // bit 6
      BITFIELD_UINT   SPI1_REMAP : 1;      // bit 7
    };  // RMPCR1 bitfield

    /// register _SYSCFG_RMPCR1 reset value
    #define sfr_SYSCFG_RMPCR1_RESET_VALUE   ((uint8_t) 0x00)

  } RMPCR1;


  /** Remapping register 2 (RMPCR2 at 0x509f) */
  union {

    /// bytewise access to RMPCR2
    uint8_t  byte;

    /// bitwise access to register RMPCR2
    struct {
      BITFIELD_UINT   ADC1TRIG_REMAP: 1;      // bit 0
      BITFIELD_UINT   TIM2TRIG_REMAP: 1;      // bit 1
      BITFIELD_UINT   TIM3TRIG_REMAP: 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // RMPCR2 bitfield

    /// register _SYSCFG_RMPCR2 reset value
    #define sfr_SYSCFG_RMPCR2_RESET_VALUE   ((uint8_t) 0x00)

  } RMPCR2;

} SYSCFG_t;

/// access to SYSCFG SFR registers
#define sfr_SYSCFG   (*((SYSCFG_t*) 0x509d))


//------------------------
// Module TIM1
//------------------------

/** struct containing TIM1 module registers */
typedef struct {

  /** TIM1 control register 1 (CR1 at 0x52b0) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CEN        : 1;      // bit 0
      BITFIELD_UINT   UDIS       : 1;      // bit 1
      BITFIELD_UINT   URS        : 1;      // bit 2
      BITFIELD_UINT   OPM        : 1;      // bit 3
      BITFIELD_UINT   DIR        : 1;      // bit 4
      BITFIELD_UINT   CMS        : 2;      // bits 5-6
      BITFIELD_UINT   ARPE       : 1;      // bit 7
    };  // CR1 bitfield

    /// register _TIM1_CR1 reset value
    #define sfr_TIM1_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** TIM1 control register 2 (CR2 at 0x52b1) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   CCPC       : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   COMS       : 1;      // bit 2
      BITFIELD_UINT   CCDS       : 1;      // bit 3
      BITFIELD_UINT   MMS        : 3;      // bits 4-6
      BITFIELD_UINT   TI1S       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _TIM1_CR2 reset value
    #define sfr_TIM1_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** TIM1 Slave mode control register (SMCR at 0x52b2) */
  union {

    /// bytewise access to SMCR
    uint8_t  byte;

    /// bitwise access to register SMCR
    struct {
      BITFIELD_UINT   SMS        : 3;      // bits 0-2
      BITFIELD_UINT   OCCS       : 1;      // bit 3
      BITFIELD_UINT   TS         : 3;      // bits 4-6
      BITFIELD_UINT   MSM        : 1;      // bit 7
    };  // SMCR bitfield

    /// register _TIM1_SMCR reset value
    #define sfr_TIM1_SMCR_RESET_VALUE   ((uint8_t) 0x00)

  } SMCR;


  /** TIM1 external trigger register (ETR at 0x52b3) */
  union {

    /// bytewise access to ETR
    uint8_t  byte;

    /// bitwise access to register ETR
    struct {
      BITFIELD_UINT   ETF        : 4;      // bits 0-3
      BITFIELD_UINT   ETPS       : 2;      // bits 4-5
      BITFIELD_UINT   ECE        : 1;      // bit 6
      BITFIELD_UINT   ETP        : 1;      // bit 7
    };  // ETR bitfield

    /// register _TIM1_ETR reset value
    #define sfr_TIM1_ETR_RESET_VALUE   ((uint8_t) 0x00)

  } ETR;


  /** TIM1 DMA1 request enable register (DER at 0x52b4) */
  union {

    /// bytewise access to DER
    uint8_t  byte;

    /// bitwise access to register DER
    struct {
      BITFIELD_UINT   UDE        : 1;      // bit 0
      BITFIELD_UINT   CC1DE      : 1;      // bit 1
      BITFIELD_UINT   CC2DE      : 1;      // bit 2
      BITFIELD_UINT   CC3DE      : 1;      // bit 3
      BITFIELD_UINT   CC4DE      : 1;      // bit 4
      BITFIELD_UINT   COMDE      : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // DER bitfield

    /// register _TIM1_DER reset value
    #define sfr_TIM1_DER_RESET_VALUE   ((uint8_t) 0x00)

  } DER;


  /** TIM1 Interrupt enable register (IER at 0x52b5) */
  union {

    /// bytewise access to IER
    uint8_t  byte;

    /// bitwise access to register IER
    struct {
      BITFIELD_UINT   UIE        : 1;      // bit 0
      BITFIELD_UINT   CC1IE      : 1;      // bit 1
      BITFIELD_UINT   CC2IE      : 1;      // bit 2
      BITFIELD_UINT   CC3IE      : 1;      // bit 3
      BITFIELD_UINT   CC4IE      : 1;      // bit 4
      BITFIELD_UINT   COMIE      : 1;      // bit 5
      BITFIELD_UINT   TIE        : 1;      // bit 6
      BITFIELD_UINT   BIE        : 1;      // bit 7
    };  // IER bitfield

    /// register _TIM1_IER reset value
    #define sfr_TIM1_IER_RESET_VALUE   ((uint8_t) 0x00)

  } IER;


  /** TIM1 status register 1 (SR1 at 0x52b6) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   UIF        : 1;      // bit 0
      BITFIELD_UINT   CC1IF      : 1;      // bit 1
      BITFIELD_UINT   CC2IF      : 1;      // bit 2
      BITFIELD_UINT   CC3IF      : 1;      // bit 3
      BITFIELD_UINT   CC4IF      : 1;      // bit 4
      BITFIELD_UINT   COMIF      : 1;      // bit 5
      BITFIELD_UINT   TIF        : 1;      // bit 6
      BITFIELD_UINT   BIF        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _TIM1_SR1 reset value
    #define sfr_TIM1_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** TIM1 status register 2 (SR2 at 0x52b7) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CC1OF      : 1;      // bit 1
      BITFIELD_UINT   CC2OF      : 1;      // bit 2
      BITFIELD_UINT   CC3OF      : 1;      // bit 3
      BITFIELD_UINT   CC4OF      : 1;      // bit 4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // SR2 bitfield

    /// register _TIM1_SR2 reset value
    #define sfr_TIM1_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** TIM1 event generation register (EGR at 0x52b8) */
  union {

    /// bytewise access to EGR
    uint8_t  byte;

    /// bitwise access to register EGR
    struct {
      BITFIELD_UINT   UG         : 1;      // bit 0
      BITFIELD_UINT   CC1G       : 1;      // bit 1
      BITFIELD_UINT   CC2G       : 1;      // bit 2
      BITFIELD_UINT   CC3G       : 1;      // bit 3
      BITFIELD_UINT   CC4G       : 1;      // bit 4
      BITFIELD_UINT   COMG       : 1;      // bit 5
      BITFIELD_UINT   TG         : 1;      // bit 6
      BITFIELD_UINT   BG         : 1;      // bit 7
    };  // EGR bitfield

    /// register _TIM1_EGR reset value
    #define sfr_TIM1_EGR_RESET_VALUE   ((uint8_t) 0x00)

  } EGR;


  /** TIM1 Capture/Compare mode register 1 (CCMR1 at 0x52b9) */
  union {

    /// bytewise access to CCMR1
    uint8_t  byte;

    /// bitwise access to register CCMR1
    struct {
      BITFIELD_UINT   CC1S       : 2;      // bits 0-1
      BITFIELD_UINT   OC1FE      : 1;      // bit 2
      BITFIELD_UINT   OC1PE      : 1;      // bit 3
      BITFIELD_UINT   OC1M       : 3;      // bits 4-6
      BITFIELD_UINT   OC1CE      : 1;      // bit 7
    };  // CCMR1 bitfield

    /// register _TIM1_CCMR1 reset value
    #define sfr_TIM1_CCMR1_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR1;


  /** TIM1 Capture/Compare mode register 2 (CCMR2 at 0x52ba) */
  union {

    /// bytewise access to CCMR2
    uint8_t  byte;

    /// bitwise access to register CCMR2
    struct {
      BITFIELD_UINT   CC2S       : 2;      // bits 0-1
      BITFIELD_UINT   OC2FE      : 1;      // bit 2
      BITFIELD_UINT   OC2PE      : 1;      // bit 3
      BITFIELD_UINT   OC2M       : 3;      // bits 4-6
      BITFIELD_UINT   OC2CE      : 1;      // bit 7
    };  // CCMR2 bitfield

    /// register _TIM1_CCMR2 reset value
    #define sfr_TIM1_CCMR2_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR2;


  /** TIM1 Capture/Compare mode register 3 (CCMR3 at 0x52bb) */
  union {

    /// bytewise access to CCMR3
    uint8_t  byte;

    /// bitwise access to register CCMR3
    struct {
      BITFIELD_UINT   CC3S       : 2;      // bits 0-1
      BITFIELD_UINT   OC3FE      : 1;      // bit 2
      BITFIELD_UINT   OC3PE      : 1;      // bit 3
      BITFIELD_UINT   OC3M       : 3;      // bits 4-6
      BITFIELD_UINT   OC3CE      : 1;      // bit 7
    };  // CCMR3 bitfield

    /// register _TIM1_CCMR3 reset value
    #define sfr_TIM1_CCMR3_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR3;


  /** TIM1 Capture/Compare mode register 4 (CCMR4 at 0x52bc) */
  union {

    /// bytewise access to CCMR4
    uint8_t  byte;

    /// bitwise access to register CCMR4
    struct {
      BITFIELD_UINT   CC4S       : 2;      // bits 0-1
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   OC4PE      : 1;      // bit 3
      BITFIELD_UINT   OC4M       : 3;      // bits 4-6
      BITFIELD_UINT   OC4CE      : 1;      // bit 7
    };  // CCMR4 bitfield

    /// register _TIM1_CCMR4 reset value
    #define sfr_TIM1_CCMR4_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR4;


  /** TIM1 Capture/Compare enable register 1 (CCER1 at 0x52bd) */
  union {

    /// bytewise access to CCER1
    uint8_t  byte;

    /// bitwise access to register CCER1
    struct {
      BITFIELD_UINT   CC1E       : 1;      // bit 0
      BITFIELD_UINT   CC1P       : 1;      // bit 1
      BITFIELD_UINT   CC1NE      : 1;      // bit 2
      BITFIELD_UINT   CC1NP      : 1;      // bit 3
      BITFIELD_UINT   CC2E       : 1;      // bit 4
      BITFIELD_UINT   CC2P       : 1;      // bit 5
      BITFIELD_UINT   CC2NE      : 1;      // bit 6
      BITFIELD_UINT   CC2NP      : 1;      // bit 7
    };  // CCER1 bitfield

    /// register _TIM1_CCER1 reset value
    #define sfr_TIM1_CCER1_RESET_VALUE   ((uint8_t) 0x00)

  } CCER1;


  /** TIM1 Capture/Compare enable register 2 (CCER2 at 0x52be) */
  union {

    /// bytewise access to CCER2
    uint8_t  byte;

    /// bitwise access to register CCER2
    struct {
      BITFIELD_UINT   CC3E       : 1;      // bit 0
      BITFIELD_UINT   CC3P       : 1;      // bit 1
      BITFIELD_UINT   CC3NE      : 1;      // bit 2
      BITFIELD_UINT   CC3NP      : 1;      // bit 3
      BITFIELD_UINT   CC4E       : 1;      // bit 4
      BITFIELD_UINT   CC4P       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CCER2 bitfield

    /// register _TIM1_CCER2 reset value
    #define sfr_TIM1_CCER2_RESET_VALUE   ((uint8_t) 0x00)

  } CCER2;


  /** TIM1 counter high (CNTRH at 0x52bf) */
  union {

    /// bytewise access to CNTRH
    uint8_t  byte;

    /// bitwise access to register CNTRH
    struct {
      BITFIELD_UINT   CNT8       : 1;      // bit 0
      BITFIELD_UINT   CNT9       : 1;      // bit 1
      BITFIELD_UINT   CNT10      : 1;      // bit 2
      BITFIELD_UINT   CNT11      : 1;      // bit 3
      BITFIELD_UINT   CNT12      : 1;      // bit 4
      BITFIELD_UINT   CNT13      : 1;      // bit 5
      BITFIELD_UINT   CNT14      : 1;      // bit 6
      BITFIELD_UINT   CNT15      : 1;      // bit 7
    };  // CNTRH bitfield

    /// register _TIM1_CNTRH reset value
    #define sfr_TIM1_CNTRH_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRH;


  /** TIM1 counter low (CNTRL at 0x52c0) */
  union {

    /// bytewise access to CNTRL
    uint8_t  byte;

    /// bitwise access to register CNTRL
    struct {
      BITFIELD_UINT   CNT0       : 1;      // bit 0
      BITFIELD_UINT   CNT1       : 1;      // bit 1
      BITFIELD_UINT   CNT2       : 1;      // bit 2
      BITFIELD_UINT   CNT3       : 1;      // bit 3
      BITFIELD_UINT   CNT4       : 1;      // bit 4
      BITFIELD_UINT   CNT5       : 1;      // bit 5
      BITFIELD_UINT   CNT6       : 1;      // bit 6
      BITFIELD_UINT   CNT7       : 1;      // bit 7
    };  // CNTRL bitfield

    /// register _TIM1_CNTRL reset value
    #define sfr_TIM1_CNTRL_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRL;


  /** TIM1 prescaler register high (PSCRH at 0x52c1) */
  union {

    /// bytewise access to PSCRH
    uint8_t  byte;

    /// bitwise access to register PSCRH
    struct {
      BITFIELD_UINT   PSC8       : 1;      // bit 0
      BITFIELD_UINT   PSC9       : 1;      // bit 1
      BITFIELD_UINT   PSC10      : 1;      // bit 2
      BITFIELD_UINT   PSC11      : 1;      // bit 3
      BITFIELD_UINT   PSC12      : 1;      // bit 4
      BITFIELD_UINT   PSC13      : 1;      // bit 5
      BITFIELD_UINT   PSC14      : 1;      // bit 6
      BITFIELD_UINT   PSC15      : 1;      // bit 7
    };  // PSCRH bitfield

    /// register _TIM1_PSCRH reset value
    #define sfr_TIM1_PSCRH_RESET_VALUE   ((uint8_t) 0x00)

  } PSCRH;


  /** TIM1 prescaler register low (PSCRL at 0x52c2) */
  union {

    /// bytewise access to PSCRL
    uint8_t  byte;

    /// bitwise access to register PSCRL
    struct {
      BITFIELD_UINT   PSC0       : 1;      // bit 0
      BITFIELD_UINT   PSC1       : 1;      // bit 1
      BITFIELD_UINT   PSC2       : 1;      // bit 2
      BITFIELD_UINT   PSC3       : 1;      // bit 3
      BITFIELD_UINT   PSC4       : 1;      // bit 4
      BITFIELD_UINT   PSC5       : 1;      // bit 5
      BITFIELD_UINT   PSC6       : 1;      // bit 6
      BITFIELD_UINT   PSC7       : 1;      // bit 7
    };  // PSCRL bitfield

    /// register _TIM1_PSCRL reset value
    #define sfr_TIM1_PSCRL_RESET_VALUE   ((uint8_t) 0x00)

  } PSCRL;


  /** TIM1 Auto-reload register high (ARRH at 0x52c3) */
  union {

    /// bytewise access to ARRH
    uint8_t  byte;

    /// bitwise access to register ARRH
    struct {
      BITFIELD_UINT   ARR8       : 1;      // bit 0
      BITFIELD_UINT   ARR9       : 1;      // bit 1
      BITFIELD_UINT   ARR10      : 1;      // bit 2
      BITFIELD_UINT   ARR11      : 1;      // bit 3
      BITFIELD_UINT   ARR12      : 1;      // bit 4
      BITFIELD_UINT   ARR13      : 1;      // bit 5
      BITFIELD_UINT   ARR14      : 1;      // bit 6
      BITFIELD_UINT   ARR15      : 1;      // bit 7
    };  // ARRH bitfield

    /// register _TIM1_ARRH reset value
    #define sfr_TIM1_ARRH_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRH;


  /** TIM1 Auto-reload register low (ARRL at 0x52c4) */
  union {

    /// bytewise access to ARRL
    uint8_t  byte;

    /// bitwise access to register ARRL
    struct {
      BITFIELD_UINT   ARR0       : 1;      // bit 0
      BITFIELD_UINT   ARR1       : 1;      // bit 1
      BITFIELD_UINT   ARR2       : 1;      // bit 2
      BITFIELD_UINT   ARR3       : 1;      // bit 3
      BITFIELD_UINT   ARR4       : 1;      // bit 4
      BITFIELD_UINT   ARR5       : 1;      // bit 5
      BITFIELD_UINT   ARR6       : 1;      // bit 6
      BITFIELD_UINT   ARR7       : 1;      // bit 7
    };  // ARRL bitfield

    /// register _TIM1_ARRL reset value
    #define sfr_TIM1_ARRL_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRL;


  /** TIM1 Repetition counter register (RCR at 0x52c5) */
  union {

    /// bytewise access to RCR
    uint8_t  byte;

    /// bitwise access to register RCR
    struct {
      BITFIELD_UINT   REP        : 8;      // bits 0-7
    };  // RCR bitfield

    /// register _TIM1_RCR reset value
    #define sfr_TIM1_RCR_RESET_VALUE   ((uint8_t) 0x00)

  } RCR;


  /** TIM1 Capture/Compare register 1 high (CCR1H at 0x52c6) */
  union {

    /// bytewise access to CCR1H
    uint8_t  byte;

    /// bitwise access to register CCR1H
    struct {
      BITFIELD_UINT   CCR18      : 1;      // bit 0
      BITFIELD_UINT   CCR19      : 1;      // bit 1
      BITFIELD_UINT   CCR110     : 1;      // bit 2
      BITFIELD_UINT   CCR111     : 1;      // bit 3
      BITFIELD_UINT   CCR112     : 1;      // bit 4
      BITFIELD_UINT   CCR113     : 1;      // bit 5
      BITFIELD_UINT   CCR114     : 1;      // bit 6
      BITFIELD_UINT   CCR115     : 1;      // bit 7
    };  // CCR1H bitfield

    /// register _TIM1_CCR1H reset value
    #define sfr_TIM1_CCR1H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1H;


  /** TIM1 Capture/Compare register 1 low (CCR1L at 0x52c7) */
  union {

    /// bytewise access to CCR1L
    uint8_t  byte;

    /// bitwise access to register CCR1L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR1L bitfield

    /// register _TIM1_CCR1L reset value
    #define sfr_TIM1_CCR1L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1L;


  /** TIM1 Capture/Compare register 2 high (CCR2H at 0x52c8) */
  union {

    /// bytewise access to CCR2H
    uint8_t  byte;

    /// bitwise access to register CCR2H
    struct {
      BITFIELD_UINT   CCR28      : 1;      // bit 0
      BITFIELD_UINT   CCR29      : 1;      // bit 1
      BITFIELD_UINT   CCR210     : 1;      // bit 2
      BITFIELD_UINT   CCR211     : 1;      // bit 3
      BITFIELD_UINT   CCR212     : 1;      // bit 4
      BITFIELD_UINT   CCR213     : 1;      // bit 5
      BITFIELD_UINT   CCR214     : 1;      // bit 6
      BITFIELD_UINT   CCR215     : 1;      // bit 7
    };  // CCR2H bitfield

    /// register _TIM1_CCR2H reset value
    #define sfr_TIM1_CCR2H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2H;


  /** TIM1 Capture/Compare register 2 low (CCR2L at 0x52c9) */
  union {

    /// bytewise access to CCR2L
    uint8_t  byte;

    /// bitwise access to register CCR2L
    struct {
      BITFIELD_UINT   CCR20      : 1;      // bit 0
      BITFIELD_UINT   CCR21      : 1;      // bit 1
      BITFIELD_UINT   CCR22      : 1;      // bit 2
      BITFIELD_UINT   CCR23      : 1;      // bit 3
      BITFIELD_UINT   CCR24      : 1;      // bit 4
      BITFIELD_UINT   CCR25      : 1;      // bit 5
      BITFIELD_UINT   CCR26      : 1;      // bit 6
      BITFIELD_UINT   CCR27      : 1;      // bit 7
    };  // CCR2L bitfield

    /// register _TIM1_CCR2L reset value
    #define sfr_TIM1_CCR2L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2L;


  /** TIM1 Capture/Compare register 3 high (CCR3H at 0x52ca) */
  union {

    /// bytewise access to CCR3H
    uint8_t  byte;

    /// bitwise access to register CCR3H
    struct {
      BITFIELD_UINT   CCR38      : 1;      // bit 0
      BITFIELD_UINT   CCR39      : 1;      // bit 1
      BITFIELD_UINT   CCR310     : 1;      // bit 2
      BITFIELD_UINT   CCR311     : 1;      // bit 3
      BITFIELD_UINT   CCR312     : 1;      // bit 4
      BITFIELD_UINT   CCR313     : 1;      // bit 5
      BITFIELD_UINT   CCR314     : 1;      // bit 6
      BITFIELD_UINT   CCR315     : 1;      // bit 7
    };  // CCR3H bitfield

    /// register _TIM1_CCR3H reset value
    #define sfr_TIM1_CCR3H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR3H;


  /** TIM1 Capture/Compare register 3 low (CCR3L at 0x52cb) */
  union {

    /// bytewise access to CCR3L
    uint8_t  byte;

    /// bitwise access to register CCR3L
    struct {
      BITFIELD_UINT   CCR30      : 1;      // bit 0
      BITFIELD_UINT   CCR31      : 1;      // bit 1
      BITFIELD_UINT   CCR32      : 1;      // bit 2
      BITFIELD_UINT   CCR33      : 1;      // bit 3
      BITFIELD_UINT   CCR34      : 1;      // bit 4
      BITFIELD_UINT   CCR35      : 1;      // bit 5
      BITFIELD_UINT   CCR36      : 1;      // bit 6
      BITFIELD_UINT   CCR37      : 1;      // bit 7
    };  // CCR3L bitfield

    /// register _TIM1_CCR3L reset value
    #define sfr_TIM1_CCR3L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR3L;


  /** TIM1 Capture/Compare register 4 high (CCR4H at 0x52cc) */
  union {

    /// bytewise access to CCR4H
    uint8_t  byte;

    /// bitwise access to register CCR4H
    struct {
      BITFIELD_UINT   CCR48      : 1;      // bit 0
      BITFIELD_UINT   CCR49      : 1;      // bit 1
      BITFIELD_UINT   CCR410     : 1;      // bit 2
      BITFIELD_UINT   CCR411     : 1;      // bit 3
      BITFIELD_UINT   CCR412     : 1;      // bit 4
      BITFIELD_UINT   CCR413     : 1;      // bit 5
      BITFIELD_UINT   CCR414     : 1;      // bit 6
      BITFIELD_UINT   CCR415     : 1;      // bit 7
    };  // CCR4H bitfield

    /// register _TIM1_CCR4H reset value
    #define sfr_TIM1_CCR4H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR4H;


  /** TIM1 Capture/Compare register 4 low (CCR4L at 0x52cd) */
  union {

    /// bytewise access to CCR4L
    uint8_t  byte;

    /// bitwise access to register CCR4L
    struct {
      BITFIELD_UINT   CCR40      : 1;      // bit 0
      BITFIELD_UINT   CCR41      : 1;      // bit 1
      BITFIELD_UINT   CCR42      : 1;      // bit 2
      BITFIELD_UINT   CCR43      : 1;      // bit 3
      BITFIELD_UINT   CCR44      : 1;      // bit 4
      BITFIELD_UINT   CCR45      : 1;      // bit 5
      BITFIELD_UINT   CCR46      : 1;      // bit 6
      BITFIELD_UINT   CCR47      : 1;      // bit 7
    };  // CCR4L bitfield

    /// register _TIM1_CCR4L reset value
    #define sfr_TIM1_CCR4L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR4L;


  /** TIM1 break register (BKR at 0x52ce) */
  union {

    /// bytewise access to BKR
    uint8_t  byte;

    /// bitwise access to register BKR
    struct {
      BITFIELD_UINT   LOCK       : 2;      // bits 0-1
      BITFIELD_UINT   OSSI       : 1;      // bit 2
      BITFIELD_UINT   OSSR       : 1;      // bit 3
      BITFIELD_UINT   BKE        : 1;      // bit 4
      BITFIELD_UINT   BKP        : 1;      // bit 5
      BITFIELD_UINT   AOE        : 1;      // bit 6
      BITFIELD_UINT   MOE        : 1;      // bit 7
    };  // BKR bitfield

    /// register _TIM1_BKR reset value
    #define sfr_TIM1_BKR_RESET_VALUE   ((uint8_t) 0x00)

  } BKR;


  /** TIM1 dead-time register (DTR at 0x52cf) */
  union {

    /// bytewise access to DTR
    uint8_t  byte;

    /// bitwise access to register DTR
    struct {
      BITFIELD_UINT   DTG        : 8;      // bits 0-7
    };  // DTR bitfield

    /// register _TIM1_DTR reset value
    #define sfr_TIM1_DTR_RESET_VALUE   ((uint8_t) 0x00)

  } DTR;


  /** TIM1 output idle state register (OISR at 0x52d0) */
  union {

    /// bytewise access to OISR
    uint8_t  byte;

    /// bitwise access to register OISR
    struct {
      BITFIELD_UINT   OIS1       : 1;      // bit 0
      BITFIELD_UINT   OIS1N      : 1;      // bit 1
      BITFIELD_UINT   OIS2       : 1;      // bit 2
      BITFIELD_UINT   OIS2N      : 1;      // bit 3
      BITFIELD_UINT   OIS3       : 1;      // bit 4
      BITFIELD_UINT   OIS3N      : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // OISR bitfield

    /// register _TIM1_OISR reset value
    #define sfr_TIM1_OISR_RESET_VALUE   ((uint8_t) 0x00)

  } OISR;


  /** DMA1 control register 1 (DCR1 at 0x52d1) */
  union {

    /// bytewise access to DCR1
    uint8_t  byte;

    /// bitwise access to register DCR1
    struct {
      BITFIELD_UINT   DBA        : 5;      // bits 0-4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // DCR1 bitfield

    /// register _TIM1_DCR1 reset value
    #define sfr_TIM1_DCR1_RESET_VALUE   ((uint8_t) 0x00)

  } DCR1;


  /** TIM1 DMA1 control register 2 (DCR2 at 0x52d2) */
  union {

    /// bytewise access to DCR2
    uint8_t  byte;

    /// bitwise access to register DCR2
    struct {
      BITFIELD_UINT   DBL        : 5;      // bits 0-4
      BITFIELD_UINT              : 3;      // 3 bits
    };  // DCR2 bitfield

    /// register _TIM1_DCR2 reset value
    #define sfr_TIM1_DCR2_RESET_VALUE   ((uint8_t) 0x00)

  } DCR2;


  /** TIM1 DMA1 address for burst mode (DMA1R at 0x52d3) */
  union {

    /// bytewise access to DMA1R
    uint8_t  byte;

    /// bitwise access to register DMA1R
    struct {
      BITFIELD_UINT   DMAB       : 8;      // bits 0-7
    };  // DMA1R bitfield

    /// register _TIM1_DMA1R reset value
    #define sfr_TIM1_DMA1R_RESET_VALUE   ((uint8_t) 0x00)

  } DMA1R;

} TIM1_t;

/// access to TIM1 SFR registers
#define sfr_TIM1   (*((TIM1_t*) 0x52b0))


//------------------------
// Module TIM2
//------------------------

/** struct containing TIM2 module registers */
typedef struct {

  /** TIM2 control register 1 (CR1 at 0x5250) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CEN        : 1;      // bit 0
      BITFIELD_UINT   UDIS       : 1;      // bit 1
      BITFIELD_UINT   URS        : 1;      // bit 2
      BITFIELD_UINT   OPM        : 1;      // bit 3
      BITFIELD_UINT   DIR        : 1;      // bit 4
      BITFIELD_UINT   CMS        : 2;      // bits 5-6
      BITFIELD_UINT   ARPE       : 1;      // bit 7
    };  // CR1 bitfield

    /// register _TIM2_CR1 reset value
    #define sfr_TIM2_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** TIM2 control register 2 (CR2 at 0x5251) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   CCDS       : 1;      // bit 3
      BITFIELD_UINT   MMS        : 3;      // bits 4-6
      BITFIELD_UINT   TI1S       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _TIM2_CR2 reset value
    #define sfr_TIM2_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** TIM2 Slave mode control register (SMCR at 0x5252) */
  union {

    /// bytewise access to SMCR
    uint8_t  byte;

    /// bitwise access to register SMCR
    struct {
      BITFIELD_UINT   SMS        : 3;      // bits 0-2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TS         : 3;      // bits 4-6
      BITFIELD_UINT   MSM        : 1;      // bit 7
    };  // SMCR bitfield

    /// register _TIM2_SMCR reset value
    #define sfr_TIM2_SMCR_RESET_VALUE   ((uint8_t) 0x00)

  } SMCR;


  /** TIM2 external trigger register (ETR at 0x5253) */
  union {

    /// bytewise access to ETR
    uint8_t  byte;

    /// bitwise access to register ETR
    struct {
      BITFIELD_UINT   ETF        : 4;      // bits 0-3
      BITFIELD_UINT   ETPS       : 2;      // bits 4-5
      BITFIELD_UINT   ECE        : 1;      // bit 6
      BITFIELD_UINT   ETP        : 1;      // bit 7
    };  // ETR bitfield

    /// register _TIM2_ETR reset value
    #define sfr_TIM2_ETR_RESET_VALUE   ((uint8_t) 0x00)

  } ETR;


  /** TIM2 DMA1 request enable register (DER at 0x5254) */
  union {

    /// bytewise access to DER
    uint8_t  byte;

    /// bitwise access to register DER
    struct {
      BITFIELD_UINT   UDE        : 1;      // bit 0
      BITFIELD_UINT   CC1DE      : 1;      // bit 1
      BITFIELD_UINT   CC2DE      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // DER bitfield

    /// register _TIM2_DER reset value
    #define sfr_TIM2_DER_RESET_VALUE   ((uint8_t) 0x00)

  } DER;


  /** TIM2 interrupt enable register (IER at 0x5255) */
  union {

    /// bytewise access to IER
    uint8_t  byte;

    /// bitwise access to register IER
    struct {
      BITFIELD_UINT   UIE        : 1;      // bit 0
      BITFIELD_UINT   CC1IE      : 1;      // bit 1
      BITFIELD_UINT   CC2IE      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIE        : 1;      // bit 6
      BITFIELD_UINT   BIE        : 1;      // bit 7
    };  // IER bitfield

    /// register _TIM2_IER reset value
    #define sfr_TIM2_IER_RESET_VALUE   ((uint8_t) 0x00)

  } IER;


  /** TIM2 status register 1 (SR1 at 0x5256) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   UIF        : 1;      // bit 0
      BITFIELD_UINT   CC1IF      : 1;      // bit 1
      BITFIELD_UINT   CC2IF      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIF        : 1;      // bit 6
      BITFIELD_UINT   BIF        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _TIM2_SR1 reset value
    #define sfr_TIM2_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** TIM2 status register 2 (SR2 at 0x5257) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CC1OF      : 1;      // bit 1
      BITFIELD_UINT   CC2OF      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // SR2 bitfield

    /// register _TIM2_SR2 reset value
    #define sfr_TIM2_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** TIM2 event generation register (EGR at 0x5258) */
  union {

    /// bytewise access to EGR
    uint8_t  byte;

    /// bitwise access to register EGR
    struct {
      BITFIELD_UINT   UG         : 1;      // bit 0
      BITFIELD_UINT   CC1G       : 1;      // bit 1
      BITFIELD_UINT   CC2G       : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TG         : 1;      // bit 6
      BITFIELD_UINT   BG         : 1;      // bit 7
    };  // EGR bitfield

    /// register _TIM2_EGR reset value
    #define sfr_TIM2_EGR_RESET_VALUE   ((uint8_t) 0x00)

  } EGR;


  /** TIM2 capture/compare mode register 1 (CCMR1 at 0x5259) */
  union {

    /// bytewise access to CCMR1
    uint8_t  byte;

    /// bitwise access to register CCMR1
    struct {
      BITFIELD_UINT   CC1S       : 2;      // bits 0-1
      BITFIELD_UINT   OC1FE      : 1;      // bit 2
      BITFIELD_UINT   OC1PE      : 1;      // bit 3
      BITFIELD_UINT   OC1M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR1 bitfield

    /// register _TIM2_CCMR1 reset value
    #define sfr_TIM2_CCMR1_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR1;


  /** TIM2 capture/compare mode register 2 (CCMR2 at 0x525a) */
  union {

    /// bytewise access to CCMR2
    uint8_t  byte;

    /// bitwise access to register CCMR2
    struct {
      BITFIELD_UINT   CC2S       : 2;      // bits 0-1
      BITFIELD_UINT   OC2FE      : 1;      // bit 2
      BITFIELD_UINT   OC2PE      : 1;      // bit 3
      BITFIELD_UINT   OC2M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR2 bitfield

    /// register _TIM2_CCMR2 reset value
    #define sfr_TIM2_CCMR2_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR2;


  /** TIM2 capture/compare enable register 1 (CCER1 at 0x525b) */
  union {

    /// bytewise access to CCER1
    uint8_t  byte;

    /// bitwise access to register CCER1
    struct {
      BITFIELD_UINT   CC1E       : 1;      // bit 0
      BITFIELD_UINT   CC1P       : 1;      // bit 1
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   CC2E       : 1;      // bit 4
      BITFIELD_UINT   CC2P       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CCER1 bitfield

    /// register _TIM2_CCER1 reset value
    #define sfr_TIM2_CCER1_RESET_VALUE   ((uint8_t) 0x00)

  } CCER1;


  /** TIM2 counter high (CNTRH at 0x525c) */
  union {

    /// bytewise access to CNTRH
    uint8_t  byte;

    /// bitwise access to register CNTRH
    struct {
      BITFIELD_UINT   CNT8       : 1;      // bit 0
      BITFIELD_UINT   CNT9       : 1;      // bit 1
      BITFIELD_UINT   CNT10      : 1;      // bit 2
      BITFIELD_UINT   CNT11      : 1;      // bit 3
      BITFIELD_UINT   CNT12      : 1;      // bit 4
      BITFIELD_UINT   CNT13      : 1;      // bit 5
      BITFIELD_UINT   CNT14      : 1;      // bit 6
      BITFIELD_UINT   CNT15      : 1;      // bit 7
    };  // CNTRH bitfield

    /// register _TIM2_CNTRH reset value
    #define sfr_TIM2_CNTRH_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRH;


  /** TIM2 counter low (CNTRL at 0x525d) */
  union {

    /// bytewise access to CNTRL
    uint8_t  byte;

    /// bitwise access to register CNTRL
    struct {
      BITFIELD_UINT   CNT0       : 1;      // bit 0
      BITFIELD_UINT   CNT1       : 1;      // bit 1
      BITFIELD_UINT   CNT2       : 1;      // bit 2
      BITFIELD_UINT   CNT3       : 1;      // bit 3
      BITFIELD_UINT   CNT4       : 1;      // bit 4
      BITFIELD_UINT   CNT5       : 1;      // bit 5
      BITFIELD_UINT   CNT6       : 1;      // bit 6
      BITFIELD_UINT   CNT7       : 1;      // bit 7
    };  // CNTRL bitfield

    /// register _TIM2_CNTRL reset value
    #define sfr_TIM2_CNTRL_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRL;


  /** TIM2 prescaler register (PSCR at 0x525e) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 3;      // bits 0-2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // PSCR bitfield

    /// register _TIM2_PSCR reset value
    #define sfr_TIM2_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;


  /** TIM2 auto-reload register high (ARRH at 0x525f) */
  union {

    /// bytewise access to ARRH
    uint8_t  byte;

    /// bitwise access to register ARRH
    struct {
      BITFIELD_UINT   ARR8       : 1;      // bit 0
      BITFIELD_UINT   ARR9       : 1;      // bit 1
      BITFIELD_UINT   ARR10      : 1;      // bit 2
      BITFIELD_UINT   ARR11      : 1;      // bit 3
      BITFIELD_UINT   ARR12      : 1;      // bit 4
      BITFIELD_UINT   ARR13      : 1;      // bit 5
      BITFIELD_UINT   ARR14      : 1;      // bit 6
      BITFIELD_UINT   ARR15      : 1;      // bit 7
    };  // ARRH bitfield

    /// register _TIM2_ARRH reset value
    #define sfr_TIM2_ARRH_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRH;


  /** TIM2 auto-reload register low (ARRL at 0x5260) */
  union {

    /// bytewise access to ARRL
    uint8_t  byte;

    /// bitwise access to register ARRL
    struct {
      BITFIELD_UINT   ARR0       : 1;      // bit 0
      BITFIELD_UINT   ARR1       : 1;      // bit 1
      BITFIELD_UINT   ARR2       : 1;      // bit 2
      BITFIELD_UINT   ARR3       : 1;      // bit 3
      BITFIELD_UINT   ARR4       : 1;      // bit 4
      BITFIELD_UINT   ARR5       : 1;      // bit 5
      BITFIELD_UINT   ARR6       : 1;      // bit 6
      BITFIELD_UINT   ARR7       : 1;      // bit 7
    };  // ARRL bitfield

    /// register _TIM2_ARRL reset value
    #define sfr_TIM2_ARRL_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRL;


  /** TIM2 capture/compare register 1 high (CCR1H at 0x5261) */
  union {

    /// bytewise access to CCR1H
    uint8_t  byte;

    /// bitwise access to register CCR1H
    struct {
      BITFIELD_UINT   CCR18      : 1;      // bit 0
      BITFIELD_UINT   CCR19      : 1;      // bit 1
      BITFIELD_UINT   CCR110     : 1;      // bit 2
      BITFIELD_UINT   CCR111     : 1;      // bit 3
      BITFIELD_UINT   CCR112     : 1;      // bit 4
      BITFIELD_UINT   CCR113     : 1;      // bit 5
      BITFIELD_UINT   CCR114     : 1;      // bit 6
      BITFIELD_UINT   CCR115     : 1;      // bit 7
    };  // CCR1H bitfield

    /// register _TIM2_CCR1H reset value
    #define sfr_TIM2_CCR1H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1H;


  /** TIM2 capture/compare register 1 low (CCR1L at 0x5262) */
  union {

    /// bytewise access to CCR1L
    uint8_t  byte;

    /// bitwise access to register CCR1L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR1L bitfield

    /// register _TIM2_CCR1L reset value
    #define sfr_TIM2_CCR1L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1L;


  /** TIM2 capture/compare register 2 high (CCR2H at 0x5263) */
  union {

    /// bytewise access to CCR2H
    uint8_t  byte;

    /// bitwise access to register CCR2H
    struct {
      BITFIELD_UINT   CCR28      : 1;      // bit 0
      BITFIELD_UINT   CCR29      : 1;      // bit 1
      BITFIELD_UINT   CCR210     : 1;      // bit 2
      BITFIELD_UINT   CCR211     : 1;      // bit 3
      BITFIELD_UINT   CCR212     : 1;      // bit 4
      BITFIELD_UINT   CCR213     : 1;      // bit 5
      BITFIELD_UINT   CCR214     : 1;      // bit 6
      BITFIELD_UINT   CCR215     : 1;      // bit 7
    };  // CCR2H bitfield

    /// register _TIM2_CCR2H reset value
    #define sfr_TIM2_CCR2H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2H;


  /** TIM2 capture/compare register 2 low (CCR2L at 0x5264) */
  union {

    /// bytewise access to CCR2L
    uint8_t  byte;

    /// bitwise access to register CCR2L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR2L bitfield

    /// register _TIM2_CCR2L reset value
    #define sfr_TIM2_CCR2L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2L;


  /** TIM2 break register (BKR at 0x5265) */
  union {

    /// bytewise access to BKR
    uint8_t  byte;

    /// bitwise access to register BKR
    struct {
      BITFIELD_UINT   LOCK       : 2;      // bits 0-1
      BITFIELD_UINT   OSSI       : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   BKE        : 1;      // bit 4
      BITFIELD_UINT   BKP        : 1;      // bit 5
      BITFIELD_UINT   AOE        : 1;      // bit 6
      BITFIELD_UINT   MOE        : 1;      // bit 7
    };  // BKR bitfield

    /// register _TIM2_BKR reset value
    #define sfr_TIM2_BKR_RESET_VALUE   ((uint8_t) 0x00)

  } BKR;


  /** TIM2 output idle state register (OISR at 0x5266) */
  union {

    /// bytewise access to OISR
    uint8_t  byte;

    /// bitwise access to register OISR
    struct {
      BITFIELD_UINT   OIS1       : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   OIS2       : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // OISR bitfield

    /// register _TIM2_OISR reset value
    #define sfr_TIM2_OISR_RESET_VALUE   ((uint8_t) 0x00)

  } OISR;

} TIM2_t;

/// access to TIM2 SFR registers
#define sfr_TIM2   (*((TIM2_t*) 0x5250))


//------------------------
// Module TIM3
//------------------------

/** struct containing TIM3 module registers */
typedef struct {

  /** TIM3 control register 1 (CR1 at 0x5280) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CEN        : 1;      // bit 0
      BITFIELD_UINT   UDIS       : 1;      // bit 1
      BITFIELD_UINT   URS        : 1;      // bit 2
      BITFIELD_UINT   OPM        : 1;      // bit 3
      BITFIELD_UINT   DIR        : 1;      // bit 4
      BITFIELD_UINT   CMS        : 2;      // bits 5-6
      BITFIELD_UINT   ARPE       : 1;      // bit 7
    };  // CR1 bitfield

    /// register _TIM3_CR1 reset value
    #define sfr_TIM3_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** TIM3 control register 2 (CR2 at 0x5281) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   CCDS       : 1;      // bit 3
      BITFIELD_UINT   MMS        : 3;      // bits 4-6
      BITFIELD_UINT   TI1S       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _TIM3_CR2 reset value
    #define sfr_TIM3_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** TIM3 Slave mode control register (SMCR at 0x5282) */
  union {

    /// bytewise access to SMCR
    uint8_t  byte;

    /// bitwise access to register SMCR
    struct {
      BITFIELD_UINT   SMS        : 3;      // bits 0-2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TS         : 3;      // bits 4-6
      BITFIELD_UINT   MSM        : 1;      // bit 7
    };  // SMCR bitfield

    /// register _TIM3_SMCR reset value
    #define sfr_TIM3_SMCR_RESET_VALUE   ((uint8_t) 0x00)

  } SMCR;


  /** TIM3 external trigger register (ETR at 0x5283) */
  union {

    /// bytewise access to ETR
    uint8_t  byte;

    /// bitwise access to register ETR
    struct {
      BITFIELD_UINT   ETF        : 4;      // bits 0-3
      BITFIELD_UINT   ETPS       : 2;      // bits 4-5
      BITFIELD_UINT   ECE        : 1;      // bit 6
      BITFIELD_UINT   ETP        : 1;      // bit 7
    };  // ETR bitfield

    /// register _TIM3_ETR reset value
    #define sfr_TIM3_ETR_RESET_VALUE   ((uint8_t) 0x00)

  } ETR;


  /** TIM3 DMA1 request enable register (DER at 0x5284) */
  union {

    /// bytewise access to DER
    uint8_t  byte;

    /// bitwise access to register DER
    struct {
      BITFIELD_UINT   UDE        : 1;      // bit 0
      BITFIELD_UINT   CC1DE      : 1;      // bit 1
      BITFIELD_UINT   CC2DE      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // DER bitfield

    /// register _TIM3_DER reset value
    #define sfr_TIM3_DER_RESET_VALUE   ((uint8_t) 0x00)

  } DER;


  /** TIM3 interrupt enable register (IER at 0x5285) */
  union {

    /// bytewise access to IER
    uint8_t  byte;

    /// bitwise access to register IER
    struct {
      BITFIELD_UINT   UIE        : 1;      // bit 0
      BITFIELD_UINT   CC1IE      : 1;      // bit 1
      BITFIELD_UINT   CC2IE      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIE        : 1;      // bit 6
      BITFIELD_UINT   BIE        : 1;      // bit 7
    };  // IER bitfield

    /// register _TIM3_IER reset value
    #define sfr_TIM3_IER_RESET_VALUE   ((uint8_t) 0x00)

  } IER;


  /** TIM3 status register 1 (SR1 at 0x5286) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   UIF        : 1;      // bit 0
      BITFIELD_UINT   CC1IF      : 1;      // bit 1
      BITFIELD_UINT   CC2IF      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIF        : 1;      // bit 6
      BITFIELD_UINT   BIF        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _TIM3_SR1 reset value
    #define sfr_TIM3_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** TIM3 status register 2 (SR2 at 0x5287) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CC1OF      : 1;      // bit 1
      BITFIELD_UINT   CC2OF      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // SR2 bitfield

    /// register _TIM3_SR2 reset value
    #define sfr_TIM3_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** TIM3 event generation register (EGR at 0x5288) */
  union {

    /// bytewise access to EGR
    uint8_t  byte;

    /// bitwise access to register EGR
    struct {
      BITFIELD_UINT   UG         : 1;      // bit 0
      BITFIELD_UINT   CC1G       : 1;      // bit 1
      BITFIELD_UINT   CC2G       : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TG         : 1;      // bit 6
      BITFIELD_UINT   BG         : 1;      // bit 7
    };  // EGR bitfield

    /// register _TIM3_EGR reset value
    #define sfr_TIM3_EGR_RESET_VALUE   ((uint8_t) 0x00)

  } EGR;


  /** TIM3 Capture/Compare mode register 1 (CCMR1 at 0x5289) */
  union {

    /// bytewise access to CCMR1
    uint8_t  byte;

    /// bitwise access to register CCMR1
    struct {
      BITFIELD_UINT   CC1S       : 2;      // bits 0-1
      BITFIELD_UINT   OC1FE      : 1;      // bit 2
      BITFIELD_UINT   OC1PE      : 1;      // bit 3
      BITFIELD_UINT   OC1M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR1 bitfield

    /// register _TIM3_CCMR1 reset value
    #define sfr_TIM3_CCMR1_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR1;


  /** TIM3 Capture/Compare mode register 2 (CCMR2 at 0x528a) */
  union {

    /// bytewise access to CCMR2
    uint8_t  byte;

    /// bitwise access to register CCMR2
    struct {
      BITFIELD_UINT   CC2S       : 2;      // bits 0-1
      BITFIELD_UINT   OC2FE      : 1;      // bit 2
      BITFIELD_UINT   OC2PE      : 1;      // bit 3
      BITFIELD_UINT   OC2M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR2 bitfield

    /// register _TIM3_CCMR2 reset value
    #define sfr_TIM3_CCMR2_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR2;


  /** TIM3 Capture/Compare enable register 1 (CCER1 at 0x528b) */
  union {

    /// bytewise access to CCER1
    uint8_t  byte;

    /// bitwise access to register CCER1
    struct {
      BITFIELD_UINT   CC1E       : 1;      // bit 0
      BITFIELD_UINT   CC1P       : 1;      // bit 1
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   CC2E       : 1;      // bit 4
      BITFIELD_UINT   CC2P       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CCER1 bitfield

    /// register _TIM3_CCER1 reset value
    #define sfr_TIM3_CCER1_RESET_VALUE   ((uint8_t) 0x00)

  } CCER1;


  /** TIM3 counter high (CNTRH at 0x528c) */
  union {

    /// bytewise access to CNTRH
    uint8_t  byte;

    /// bitwise access to register CNTRH
    struct {
      BITFIELD_UINT   CNT8       : 1;      // bit 0
      BITFIELD_UINT   CNT9       : 1;      // bit 1
      BITFIELD_UINT   CNT10      : 1;      // bit 2
      BITFIELD_UINT   CNT11      : 1;      // bit 3
      BITFIELD_UINT   CNT12      : 1;      // bit 4
      BITFIELD_UINT   CNT13      : 1;      // bit 5
      BITFIELD_UINT   CNT14      : 1;      // bit 6
      BITFIELD_UINT   CNT15      : 1;      // bit 7
    };  // CNTRH bitfield

    /// register _TIM3_CNTRH reset value
    #define sfr_TIM3_CNTRH_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRH;


  /** TIM3 counter low (CNTRL at 0x528d) */
  union {

    /// bytewise access to CNTRL
    uint8_t  byte;

    /// bitwise access to register CNTRL
    struct {
      BITFIELD_UINT   CNT0       : 1;      // bit 0
      BITFIELD_UINT   CNT1       : 1;      // bit 1
      BITFIELD_UINT   CNT2       : 1;      // bit 2
      BITFIELD_UINT   CNT3       : 1;      // bit 3
      BITFIELD_UINT   CNT4       : 1;      // bit 4
      BITFIELD_UINT   CNT5       : 1;      // bit 5
      BITFIELD_UINT   CNT6       : 1;      // bit 6
      BITFIELD_UINT   CNT7       : 1;      // bit 7
    };  // CNTRL bitfield

    /// register _TIM3_CNTRL reset value
    #define sfr_TIM3_CNTRL_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRL;


  /** TIM3 prescaler register (PSCR at 0x528e) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 3;      // bits 0-2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // PSCR bitfield

    /// register _TIM3_PSCR reset value
    #define sfr_TIM3_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;


  /** TIM3 Auto-reload register high (ARRH at 0x528f) */
  union {

    /// bytewise access to ARRH
    uint8_t  byte;

    /// bitwise access to register ARRH
    struct {
      BITFIELD_UINT   ARR8       : 1;      // bit 0
      BITFIELD_UINT   ARR9       : 1;      // bit 1
      BITFIELD_UINT   ARR10      : 1;      // bit 2
      BITFIELD_UINT   ARR11      : 1;      // bit 3
      BITFIELD_UINT   ARR12      : 1;      // bit 4
      BITFIELD_UINT   ARR13      : 1;      // bit 5
      BITFIELD_UINT   ARR14      : 1;      // bit 6
      BITFIELD_UINT   ARR15      : 1;      // bit 7
    };  // ARRH bitfield

    /// register _TIM3_ARRH reset value
    #define sfr_TIM3_ARRH_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRH;


  /** TIM3 Auto-reload register low (ARRL at 0x5290) */
  union {

    /// bytewise access to ARRL
    uint8_t  byte;

    /// bitwise access to register ARRL
    struct {
      BITFIELD_UINT   ARR0       : 1;      // bit 0
      BITFIELD_UINT   ARR1       : 1;      // bit 1
      BITFIELD_UINT   ARR2       : 1;      // bit 2
      BITFIELD_UINT   ARR3       : 1;      // bit 3
      BITFIELD_UINT   ARR4       : 1;      // bit 4
      BITFIELD_UINT   ARR5       : 1;      // bit 5
      BITFIELD_UINT   ARR6       : 1;      // bit 6
      BITFIELD_UINT   ARR7       : 1;      // bit 7
    };  // ARRL bitfield

    /// register _TIM3_ARRL reset value
    #define sfr_TIM3_ARRL_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRL;


  /** TIM3 Capture/Compare register 1 high (CCR1H at 0x5291) */
  union {

    /// bytewise access to CCR1H
    uint8_t  byte;

    /// bitwise access to register CCR1H
    struct {
      BITFIELD_UINT   CCR18      : 1;      // bit 0
      BITFIELD_UINT   CCR19      : 1;      // bit 1
      BITFIELD_UINT   CCR110     : 1;      // bit 2
      BITFIELD_UINT   CCR111     : 1;      // bit 3
      BITFIELD_UINT   CCR112     : 1;      // bit 4
      BITFIELD_UINT   CCR113     : 1;      // bit 5
      BITFIELD_UINT   CCR114     : 1;      // bit 6
      BITFIELD_UINT   CCR115     : 1;      // bit 7
    };  // CCR1H bitfield

    /// register _TIM3_CCR1H reset value
    #define sfr_TIM3_CCR1H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1H;


  /** TIM3 Capture/Compare register 1 low (CCR1L at 0x5292) */
  union {

    /// bytewise access to CCR1L
    uint8_t  byte;

    /// bitwise access to register CCR1L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR1L bitfield

    /// register _TIM3_CCR1L reset value
    #define sfr_TIM3_CCR1L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1L;


  /** TIM3 Capture/Compare register 2 high (CCR2H at 0x5293) */
  union {

    /// bytewise access to CCR2H
    uint8_t  byte;

    /// bitwise access to register CCR2H
    struct {
      BITFIELD_UINT   CCR28      : 1;      // bit 0
      BITFIELD_UINT   CCR29      : 1;      // bit 1
      BITFIELD_UINT   CCR210     : 1;      // bit 2
      BITFIELD_UINT   CCR211     : 1;      // bit 3
      BITFIELD_UINT   CCR212     : 1;      // bit 4
      BITFIELD_UINT   CCR213     : 1;      // bit 5
      BITFIELD_UINT   CCR214     : 1;      // bit 6
      BITFIELD_UINT   CCR215     : 1;      // bit 7
    };  // CCR2H bitfield

    /// register _TIM3_CCR2H reset value
    #define sfr_TIM3_CCR2H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2H;


  /** TIM3 Capture/Compare register 2 low (CCR2L at 0x5294) */
  union {

    /// bytewise access to CCR2L
    uint8_t  byte;

    /// bitwise access to register CCR2L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR2L bitfield

    /// register _TIM3_CCR2L reset value
    #define sfr_TIM3_CCR2L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2L;


  /** TIM3 break register (BKR at 0x5295) */
  union {

    /// bytewise access to BKR
    uint8_t  byte;

    /// bitwise access to register BKR
    struct {
      BITFIELD_UINT   LOCK       : 2;      // bits 0-1
      BITFIELD_UINT   OSSI       : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   BKE        : 1;      // bit 4
      BITFIELD_UINT   BKP        : 1;      // bit 5
      BITFIELD_UINT   AOE        : 1;      // bit 6
      BITFIELD_UINT   MOE        : 1;      // bit 7
    };  // BKR bitfield

    /// register _TIM3_BKR reset value
    #define sfr_TIM3_BKR_RESET_VALUE   ((uint8_t) 0x00)

  } BKR;


  /** TIM3 output idle state register (OISR at 0x5296) */
  union {

    /// bytewise access to OISR
    uint8_t  byte;

    /// bitwise access to register OISR
    struct {
      BITFIELD_UINT   OIS1       : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   OIS2       : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // OISR bitfield

    /// register _TIM3_OISR reset value
    #define sfr_TIM3_OISR_RESET_VALUE   ((uint8_t) 0x00)

  } OISR;

} TIM3_t;

/// access to TIM3 SFR registers
#define sfr_TIM3   (*((TIM3_t*) 0x5280))


//------------------------
// Module TIM4
//------------------------

/** struct containing TIM4 module registers */
typedef struct {

  /** TIM4 control register 1 (CR1 at 0x52e0) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CEN        : 1;      // bit 0
      BITFIELD_UINT   UDIS       : 1;      // bit 1
      BITFIELD_UINT   URS        : 1;      // bit 2
      BITFIELD_UINT   OPM        : 1;      // bit 3
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   ARPE       : 1;      // bit 7
    };  // CR1 bitfield

    /// register _TIM4_CR1 reset value
    #define sfr_TIM4_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** TIM4 control register 2 (CR2 at 0x52e1) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT              : 4;      // 4 bits
      BITFIELD_UINT   MMS        : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CR2 bitfield

    /// register _TIM4_CR2 reset value
    #define sfr_TIM4_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** TIM4 Slave mode control register (SMCR at 0x52e2) */
  union {

    /// bytewise access to SMCR
    uint8_t  byte;

    /// bitwise access to register SMCR
    struct {
      BITFIELD_UINT   SMS        : 3;      // bits 0-2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TS         : 3;      // bits 4-6
      BITFIELD_UINT   MSM        : 1;      // bit 7
    };  // SMCR bitfield

    /// register _TIM4_SMCR reset value
    #define sfr_TIM4_SMCR_RESET_VALUE   ((uint8_t) 0x00)

  } SMCR;


  /** TIM4 DMA1 request enable register (DER at 0x52e3) */
  union {

    /// bytewise access to DER
    uint8_t  byte;

    /// bitwise access to register DER
    struct {
      BITFIELD_UINT   UDE        : 1;      // bit 0
      BITFIELD_UINT              : 7;      // 7 bits
    };  // DER bitfield

    /// register _TIM4_DER reset value
    #define sfr_TIM4_DER_RESET_VALUE   ((uint8_t) 0x00)

  } DER;


  /** TIM4 Interrupt enable register (IER at 0x52e4) */
  union {

    /// bytewise access to IER
    uint8_t  byte;

    /// bitwise access to register IER
    struct {
      BITFIELD_UINT   UIE        : 1;      // bit 0
      BITFIELD_UINT              : 5;      // 5 bits
      BITFIELD_UINT   TIE        : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // IER bitfield

    /// register _TIM4_IER reset value
    #define sfr_TIM4_IER_RESET_VALUE   ((uint8_t) 0x00)

  } IER;


  /** TIM4 status register 1 (SR1 at 0x52e5) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   UIF        : 1;      // bit 0
      BITFIELD_UINT              : 5;      // 5 bits
      BITFIELD_UINT   TIF        : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // SR1 bitfield

    /// register _TIM4_SR1 reset value
    #define sfr_TIM4_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** TIM4 Event generation register (EGR at 0x52e6) */
  union {

    /// bytewise access to EGR
    uint8_t  byte;

    /// bitwise access to register EGR
    struct {
      BITFIELD_UINT   UG         : 1;      // bit 0
      BITFIELD_UINT              : 5;      // 5 bits
      BITFIELD_UINT   TG         : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // EGR bitfield

    /// register _TIM4_EGR reset value
    #define sfr_TIM4_EGR_RESET_VALUE   ((uint8_t) 0x00)

  } EGR;


  /** TIM4 counter (CNTR at 0x52e7) */
  union {

    /// bytewise access to CNTR
    uint8_t  byte;

    /// bitwise access to register CNTR
    struct {
      BITFIELD_UINT   CNT        : 8;      // bits 0-7
    };  // CNTR bitfield

    /// register _TIM4_CNTR reset value
    #define sfr_TIM4_CNTR_RESET_VALUE   ((uint8_t) 0x00)

  } CNTR;


  /** TIM4 prescaler register (PSCR at 0x52e8) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 4;      // bits 0-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // PSCR bitfield

    /// register _TIM4_PSCR reset value
    #define sfr_TIM4_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;


  /** TIM4 Auto-reload register (ARR at 0x52e9) */
  union {

    /// bytewise access to ARR
    uint8_t  byte;

    /// bitwise access to register ARR
    struct {
      BITFIELD_UINT   ARR        : 8;      // bits 0-7
    };  // ARR bitfield

    /// register _TIM4_ARR reset value
    #define sfr_TIM4_ARR_RESET_VALUE   ((uint8_t) 0x00)

  } ARR;

} TIM4_t;

/// access to TIM4 SFR registers
#define sfr_TIM4   (*((TIM4_t*) 0x52e0))


//------------------------
// Module TIM5
//------------------------

/** struct containing TIM5 module registers */
typedef struct {

  /** TIM5 control register 1 (CR1 at 0x5300) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   CEN        : 1;      // bit 0
      BITFIELD_UINT   UDIS       : 1;      // bit 1
      BITFIELD_UINT   URS        : 1;      // bit 2
      BITFIELD_UINT   OPM        : 1;      // bit 3
      BITFIELD_UINT   DIR        : 1;      // bit 4
      BITFIELD_UINT   CMS        : 2;      // bits 5-6
      BITFIELD_UINT   ARPE       : 1;      // bit 7
    };  // CR1 bitfield

    /// register _TIM5_CR1 reset value
    #define sfr_TIM5_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** TIM5 control register 2 (CR2 at 0x5301) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   CCDS       : 1;      // bit 3
      BITFIELD_UINT   MMS        : 3;      // bits 4-6
      BITFIELD_UINT   TI1S       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _TIM5_CR2 reset value
    #define sfr_TIM5_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** TIM5 Slave mode control register (SMCR at 0x5302) */
  union {

    /// bytewise access to SMCR
    uint8_t  byte;

    /// bitwise access to register SMCR
    struct {
      BITFIELD_UINT   SMS        : 3;      // bits 0-2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   TS         : 3;      // bits 4-6
      BITFIELD_UINT   MSM        : 1;      // bit 7
    };  // SMCR bitfield

    /// register _TIM5_SMCR reset value
    #define sfr_TIM5_SMCR_RESET_VALUE   ((uint8_t) 0x00)

  } SMCR;


  /** TIM5 external trigger register (ETR at 0x5303) */
  union {

    /// bytewise access to ETR
    uint8_t  byte;

    /// bitwise access to register ETR
    struct {
      BITFIELD_UINT   ETF        : 4;      // bits 0-3
      BITFIELD_UINT   ETPS       : 2;      // bits 4-5
      BITFIELD_UINT   ECE        : 1;      // bit 6
      BITFIELD_UINT   ETP        : 1;      // bit 7
    };  // ETR bitfield

    /// register _TIM5_ETR reset value
    #define sfr_TIM5_ETR_RESET_VALUE   ((uint8_t) 0x00)

  } ETR;


  /** TIM5 DMA1 request enable register (DER at 0x5304) */
  union {

    /// bytewise access to DER
    uint8_t  byte;

    /// bitwise access to register DER
    struct {
      BITFIELD_UINT   UDE        : 1;      // bit 0
      BITFIELD_UINT   CC1DE      : 1;      // bit 1
      BITFIELD_UINT   CC2DE      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // DER bitfield

    /// register _TIM5_DER reset value
    #define sfr_TIM5_DER_RESET_VALUE   ((uint8_t) 0x00)

  } DER;


  /** TIM5 interrupt enable register (IER at 0x5305) */
  union {

    /// bytewise access to IER
    uint8_t  byte;

    /// bitwise access to register IER
    struct {
      BITFIELD_UINT   UIE        : 1;      // bit 0
      BITFIELD_UINT   CC1IE      : 1;      // bit 1
      BITFIELD_UINT   CC2IE      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIE        : 1;      // bit 6
      BITFIELD_UINT   BIE        : 1;      // bit 7
    };  // IER bitfield

    /// register _TIM5_IER reset value
    #define sfr_TIM5_IER_RESET_VALUE   ((uint8_t) 0x00)

  } IER;


  /** TIM5 status register 1 (SR1 at 0x5306) */
  union {

    /// bytewise access to SR1
    uint8_t  byte;

    /// bitwise access to register SR1
    struct {
      BITFIELD_UINT   UIF        : 1;      // bit 0
      BITFIELD_UINT   CC1IF      : 1;      // bit 1
      BITFIELD_UINT   CC2IF      : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TIF        : 1;      // bit 6
      BITFIELD_UINT   BIF        : 1;      // bit 7
    };  // SR1 bitfield

    /// register _TIM5_SR1 reset value
    #define sfr_TIM5_SR1_RESET_VALUE   ((uint8_t) 0x00)

  } SR1;


  /** TIM5 status register 2 (SR2 at 0x5307) */
  union {

    /// bytewise access to SR2
    uint8_t  byte;

    /// bitwise access to register SR2
    struct {
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   CC1OF      : 1;      // bit 1
      BITFIELD_UINT   CC2OF      : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // SR2 bitfield

    /// register _TIM5_SR2 reset value
    #define sfr_TIM5_SR2_RESET_VALUE   ((uint8_t) 0x00)

  } SR2;


  /** TIM5 event generation register (EGR at 0x5308) */
  union {

    /// bytewise access to EGR
    uint8_t  byte;

    /// bitwise access to register EGR
    struct {
      BITFIELD_UINT   UG         : 1;      // bit 0
      BITFIELD_UINT   CC1G       : 1;      // bit 1
      BITFIELD_UINT   CC2G       : 1;      // bit 2
      BITFIELD_UINT              : 3;      // 3 bits
      BITFIELD_UINT   TG         : 1;      // bit 6
      BITFIELD_UINT   BG         : 1;      // bit 7
    };  // EGR bitfield

    /// register _TIM5_EGR reset value
    #define sfr_TIM5_EGR_RESET_VALUE   ((uint8_t) 0x00)

  } EGR;


  /** TIM5 Capture/Compare mode register 1 (CCMR1 at 0x5309) */
  union {

    /// bytewise access to CCMR1
    uint8_t  byte;

    /// bitwise access to register CCMR1
    struct {
      BITFIELD_UINT   CC1S       : 2;      // bits 0-1
      BITFIELD_UINT   OC1FE      : 1;      // bit 2
      BITFIELD_UINT   OC1PE      : 1;      // bit 3
      BITFIELD_UINT   OC1M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR1 bitfield

    /// register _TIM5_CCMR1 reset value
    #define sfr_TIM5_CCMR1_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR1;


  /** TIM5 Capture/Compare mode register 2 (CCMR2 at 0x530a) */
  union {

    /// bytewise access to CCMR2
    uint8_t  byte;

    /// bitwise access to register CCMR2
    struct {
      BITFIELD_UINT   CC2S       : 2;      // bits 0-1
      BITFIELD_UINT   OC2FE      : 1;      // bit 2
      BITFIELD_UINT   OC2PE      : 1;      // bit 3
      BITFIELD_UINT   OC2M       : 3;      // bits 4-6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CCMR2 bitfield

    /// register _TIM5_CCMR2 reset value
    #define sfr_TIM5_CCMR2_RESET_VALUE   ((uint8_t) 0x00)

  } CCMR2;


  /** TIM5 Capture/Compare enable register 1 (CCER1 at 0x530b) */
  union {

    /// bytewise access to CCER1
    uint8_t  byte;

    /// bitwise access to register CCER1
    struct {
      BITFIELD_UINT   CC1E       : 1;      // bit 0
      BITFIELD_UINT   CC1P       : 1;      // bit 1
      BITFIELD_UINT              : 2;      // 2 bits
      BITFIELD_UINT   CC2E       : 1;      // bit 4
      BITFIELD_UINT   CC2P       : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CCER1 bitfield

    /// register _TIM5_CCER1 reset value
    #define sfr_TIM5_CCER1_RESET_VALUE   ((uint8_t) 0x00)

  } CCER1;


  /** TIM5 counter high (CNTRH at 0x530c) */
  union {

    /// bytewise access to CNTRH
    uint8_t  byte;

    /// bitwise access to register CNTRH
    struct {
      BITFIELD_UINT   CNT8       : 1;      // bit 0
      BITFIELD_UINT   CNT9       : 1;      // bit 1
      BITFIELD_UINT   CNT10      : 1;      // bit 2
      BITFIELD_UINT   CNT11      : 1;      // bit 3
      BITFIELD_UINT   CNT12      : 1;      // bit 4
      BITFIELD_UINT   CNT13      : 1;      // bit 5
      BITFIELD_UINT   CNT14      : 1;      // bit 6
      BITFIELD_UINT   CNT15      : 1;      // bit 7
    };  // CNTRH bitfield

    /// register _TIM5_CNTRH reset value
    #define sfr_TIM5_CNTRH_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRH;


  /** TIM5 counter low (CNTRL at 0x530d) */
  union {

    /// bytewise access to CNTRL
    uint8_t  byte;

    /// bitwise access to register CNTRL
    struct {
      BITFIELD_UINT   CNT0       : 1;      // bit 0
      BITFIELD_UINT   CNT1       : 1;      // bit 1
      BITFIELD_UINT   CNT2       : 1;      // bit 2
      BITFIELD_UINT   CNT3       : 1;      // bit 3
      BITFIELD_UINT   CNT4       : 1;      // bit 4
      BITFIELD_UINT   CNT5       : 1;      // bit 5
      BITFIELD_UINT   CNT6       : 1;      // bit 6
      BITFIELD_UINT   CNT7       : 1;      // bit 7
    };  // CNTRL bitfield

    /// register _TIM5_CNTRL reset value
    #define sfr_TIM5_CNTRL_RESET_VALUE   ((uint8_t) 0x00)

  } CNTRL;


  /** TIM5 prescaler register (PSCR at 0x530e) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 3;      // bits 0-2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // PSCR bitfield

    /// register _TIM5_PSCR reset value
    #define sfr_TIM5_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;


  /** TIM5 Auto-reload register high (ARRH at 0x530f) */
  union {

    /// bytewise access to ARRH
    uint8_t  byte;

    /// bitwise access to register ARRH
    struct {
      BITFIELD_UINT   ARR8       : 1;      // bit 0
      BITFIELD_UINT   ARR9       : 1;      // bit 1
      BITFIELD_UINT   ARR10      : 1;      // bit 2
      BITFIELD_UINT   ARR11      : 1;      // bit 3
      BITFIELD_UINT   ARR12      : 1;      // bit 4
      BITFIELD_UINT   ARR13      : 1;      // bit 5
      BITFIELD_UINT   ARR14      : 1;      // bit 6
      BITFIELD_UINT   ARR15      : 1;      // bit 7
    };  // ARRH bitfield

    /// register _TIM5_ARRH reset value
    #define sfr_TIM5_ARRH_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRH;


  /** TIM5 Auto-reload register low (ARRL at 0x5310) */
  union {

    /// bytewise access to ARRL
    uint8_t  byte;

    /// bitwise access to register ARRL
    struct {
      BITFIELD_UINT   ARR0       : 1;      // bit 0
      BITFIELD_UINT   ARR1       : 1;      // bit 1
      BITFIELD_UINT   ARR2       : 1;      // bit 2
      BITFIELD_UINT   ARR3       : 1;      // bit 3
      BITFIELD_UINT   ARR4       : 1;      // bit 4
      BITFIELD_UINT   ARR5       : 1;      // bit 5
      BITFIELD_UINT   ARR6       : 1;      // bit 6
      BITFIELD_UINT   ARR7       : 1;      // bit 7
    };  // ARRL bitfield

    /// register _TIM5_ARRL reset value
    #define sfr_TIM5_ARRL_RESET_VALUE   ((uint8_t) 0xFF)

  } ARRL;


  /** TIM5 Capture/Compare register 1 high (CCR1H at 0x5311) */
  union {

    /// bytewise access to CCR1H
    uint8_t  byte;

    /// bitwise access to register CCR1H
    struct {
      BITFIELD_UINT   CCR18      : 1;      // bit 0
      BITFIELD_UINT   CCR19      : 1;      // bit 1
      BITFIELD_UINT   CCR110     : 1;      // bit 2
      BITFIELD_UINT   CCR111     : 1;      // bit 3
      BITFIELD_UINT   CCR112     : 1;      // bit 4
      BITFIELD_UINT   CCR113     : 1;      // bit 5
      BITFIELD_UINT   CCR114     : 1;      // bit 6
      BITFIELD_UINT   CCR115     : 1;      // bit 7
    };  // CCR1H bitfield

    /// register _TIM5_CCR1H reset value
    #define sfr_TIM5_CCR1H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1H;


  /** TIM5 Capture/Compare register 1 low (CCR1L at 0x5312) */
  union {

    /// bytewise access to CCR1L
    uint8_t  byte;

    /// bitwise access to register CCR1L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR1L bitfield

    /// register _TIM5_CCR1L reset value
    #define sfr_TIM5_CCR1L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR1L;


  /** TIM5 Capture/Compare register 2 high (CCR2H at 0x5313) */
  union {

    /// bytewise access to CCR2H
    uint8_t  byte;

    /// bitwise access to register CCR2H
    struct {
      BITFIELD_UINT   CCR28      : 1;      // bit 0
      BITFIELD_UINT   CCR29      : 1;      // bit 1
      BITFIELD_UINT   CCR210     : 1;      // bit 2
      BITFIELD_UINT   CCR211     : 1;      // bit 3
      BITFIELD_UINT   CCR212     : 1;      // bit 4
      BITFIELD_UINT   CCR213     : 1;      // bit 5
      BITFIELD_UINT   CCR214     : 1;      // bit 6
      BITFIELD_UINT   CCR215     : 1;      // bit 7
    };  // CCR2H bitfield

    /// register _TIM5_CCR2H reset value
    #define sfr_TIM5_CCR2H_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2H;


  /** TIM5 Capture/Compare register 2 low (CCR2L at 0x5314) */
  union {

    /// bytewise access to CCR2L
    uint8_t  byte;

    /// bitwise access to register CCR2L
    struct {
      BITFIELD_UINT   CCR10      : 1;      // bit 0
      BITFIELD_UINT   CCR11      : 1;      // bit 1
      BITFIELD_UINT   CCR12      : 1;      // bit 2
      BITFIELD_UINT   CCR13      : 1;      // bit 3
      BITFIELD_UINT   CCR14      : 1;      // bit 4
      BITFIELD_UINT   CCR15      : 1;      // bit 5
      BITFIELD_UINT   CCR16      : 1;      // bit 6
      BITFIELD_UINT   CCR17      : 1;      // bit 7
    };  // CCR2L bitfield

    /// register _TIM5_CCR2L reset value
    #define sfr_TIM5_CCR2L_RESET_VALUE   ((uint8_t) 0x00)

  } CCR2L;


  /** TIM5 break register (BKR at 0x5315) */
  union {

    /// bytewise access to BKR
    uint8_t  byte;

    /// bitwise access to register BKR
    struct {
      BITFIELD_UINT   LOCK       : 2;      // bits 0-1
      BITFIELD_UINT   OSSI       : 1;      // bit 2
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   BKE        : 1;      // bit 4
      BITFIELD_UINT   BKP        : 1;      // bit 5
      BITFIELD_UINT   AOE        : 1;      // bit 6
      BITFIELD_UINT   MOE        : 1;      // bit 7
    };  // BKR bitfield

    /// register _TIM5_BKR reset value
    #define sfr_TIM5_BKR_RESET_VALUE   ((uint8_t) 0x00)

  } BKR;


  /** TIM5 output idle state register (OISR at 0x5316) */
  union {

    /// bytewise access to OISR
    uint8_t  byte;

    /// bitwise access to register OISR
    struct {
      BITFIELD_UINT   OIS1       : 1;      // bit 0
      BITFIELD_UINT              : 1;      // 1 bit
      BITFIELD_UINT   OIS2       : 1;      // bit 2
      BITFIELD_UINT              : 5;      // 5 bits
    };  // OISR bitfield

    /// register _TIM5_OISR reset value
    #define sfr_TIM5_OISR_RESET_VALUE   ((uint8_t) 0x00)

  } OISR;

} TIM5_t;

/// access to TIM5 SFR registers
#define sfr_TIM5   (*((TIM5_t*) 0x5300))


//------------------------
// Module USART1
//------------------------

/** struct containing USART1 module registers */
typedef struct {

  /** USART1 status register (SR at 0x5230) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   PE         : 1;      // bit 0
      BITFIELD_UINT   FE         : 1;      // bit 1
      BITFIELD_UINT   NF         : 1;      // bit 2
      BITFIELD_UINT   OR         : 1;      // bit 3
      BITFIELD_UINT   IDLE       : 1;      // bit 4
      BITFIELD_UINT   RXNE       : 1;      // bit 5
      BITFIELD_UINT   TC         : 1;      // bit 6
      BITFIELD_UINT   TXE        : 1;      // bit 7
    };  // SR bitfield

    /// register _USART1_SR reset value
    #define sfr_USART1_SR_RESET_VALUE   ((uint8_t) 0xC0)

  } SR;


  /** USART1 data register (DR at 0x5231) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _USART1_DR reset value
    #define sfr_USART1_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** USART1 baud rate register 1 (BRR1 at 0x5232) */
  union {

    /// bytewise access to BRR1
    uint8_t  byte;

    /// bitwise access to register BRR1
    struct {
      BITFIELD_UINT   USART_DIV4 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV5 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV6 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV7 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV8 : 1;      // bit 4
      BITFIELD_UINT   USART_DIV9 : 1;      // bit 5
      BITFIELD_UINT   USART_DIV10: 1;      // bit 6
      BITFIELD_UINT   USART_DIV11: 1;      // bit 7
    };  // BRR1 bitfield

    /// register _USART1_BRR1 reset value
    #define sfr_USART1_BRR1_RESET_VALUE   ((uint8_t) 0x00)

  } BRR1;


  /** USART1 baud rate register 2 (BRR2 at 0x5233) */
  union {

    /// bytewise access to BRR2
    uint8_t  byte;

    /// bitwise access to register BRR2
    struct {
      BITFIELD_UINT   USART_DIV0 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV1 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV2 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV3 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV12: 1;      // bit 4
      BITFIELD_UINT   USART_DIV13: 1;      // bit 5
      BITFIELD_UINT   USART_DIV14: 1;      // bit 6
      BITFIELD_UINT   USART_DIV15: 1;      // bit 7
    };  // BRR2 bitfield

    /// register _USART1_BRR2 reset value
    #define sfr_USART1_BRR2_RESET_VALUE   ((uint8_t) 0x00)

  } BRR2;


  /** USART1 control register 1 (CR1 at 0x5234) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   PIEN       : 1;      // bit 0
      BITFIELD_UINT   PS         : 1;      // bit 1
      BITFIELD_UINT   PCEN       : 1;      // bit 2
      BITFIELD_UINT   WAKE       : 1;      // bit 3
      BITFIELD_UINT   M          : 1;      // bit 4
      BITFIELD_UINT   USARTD     : 1;      // bit 5
      BITFIELD_UINT   T8         : 1;      // bit 6
      BITFIELD_UINT   R8         : 1;      // bit 7
    };  // CR1 bitfield

    /// register _USART1_CR1 reset value
    #define sfr_USART1_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** USART1 control register 2 (CR2 at 0x5235) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SBK        : 1;      // bit 0
      BITFIELD_UINT   RWU        : 1;      // bit 1
      BITFIELD_UINT   REN        : 1;      // bit 2
      BITFIELD_UINT   TEN        : 1;      // bit 3
      BITFIELD_UINT   ILIEN      : 1;      // bit 4
      BITFIELD_UINT   RIEN       : 1;      // bit 5
      BITFIELD_UINT   TCIEN      : 1;      // bit 6
      BITFIELD_UINT   TIEN       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _USART1_CR2 reset value
    #define sfr_USART1_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** USART1 control register 3 (CR3 at 0x5236) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   LBCL       : 1;      // bit 0
      BITFIELD_UINT   CPHA       : 1;      // bit 1
      BITFIELD_UINT   CPOL       : 1;      // bit 2
      BITFIELD_UINT   CLKEN      : 1;      // bit 3
      BITFIELD_UINT   STOP0      : 1;      // bit 4
      BITFIELD_UINT   STOP1      : 1;      // bit 5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CR3 bitfield

    /// register _USART1_CR3 reset value
    #define sfr_USART1_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /** USART1 control register 4 (CR4 at 0x5237) */
  union {

    /// bytewise access to CR4
    uint8_t  byte;

    /// bitwise access to register CR4
    struct {
      BITFIELD_UINT   ADD0       : 1;      // bit 0
      BITFIELD_UINT   ADD1       : 1;      // bit 1
      BITFIELD_UINT   ADD2       : 1;      // bit 2
      BITFIELD_UINT   ADD3       : 1;      // bit 3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CR4 bitfield

    /// register _USART1_CR4 reset value
    #define sfr_USART1_CR4_RESET_VALUE   ((uint8_t) 0x00)

  } CR4;


  /** USART1 control register 5 (CR5 at 0x5238) */
  union {

    /// bytewise access to CR5
    uint8_t  byte;

    /// bitwise access to register CR5
    struct {
      BITFIELD_UINT   EIE        : 1;      // bit 0
      BITFIELD_UINT   IREN       : 1;      // bit 1
      BITFIELD_UINT   IRLP       : 1;      // bit 2
      BITFIELD_UINT   HDSEL      : 1;      // bit 3
      BITFIELD_UINT   NACK       : 1;      // bit 4
      BITFIELD_UINT   SCEN       : 1;      // bit 5
      BITFIELD_UINT   DMAR       : 1;      // bit 6
      BITFIELD_UINT   DMAT       : 1;      // bit 7
    };  // CR5 bitfield

    /// register _USART1_CR5 reset value
    #define sfr_USART1_CR5_RESET_VALUE   ((uint8_t) 0x00)

  } CR5;


  /** USART1 guard time register (GTR at 0x5239) */
  union {

    /// bytewise access to GTR
    uint8_t  byte;

    /// bitwise access to register GTR
    struct {
      BITFIELD_UINT   GT         : 8;      // bits 0-7
    };  // GTR bitfield

    /// register _USART1_GTR reset value
    #define sfr_USART1_GTR_RESET_VALUE   ((uint8_t) 0x00)

  } GTR;


  /** USART1 prescaler register (PSCR at 0x523a) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 8;      // bits 0-7
    };  // PSCR bitfield

    /// register _USART1_PSCR reset value
    #define sfr_USART1_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;

} USART1_t;

/// access to USART1 SFR registers
#define sfr_USART1   (*((USART1_t*) 0x5230))


//------------------------
// Module USART2
//------------------------

/** struct containing USART2 module registers */
typedef struct {

  /** USART2 status register (SR at 0x53e0) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   PE         : 1;      // bit 0
      BITFIELD_UINT   FE         : 1;      // bit 1
      BITFIELD_UINT   NF         : 1;      // bit 2
      BITFIELD_UINT   OR         : 1;      // bit 3
      BITFIELD_UINT   IDLE       : 1;      // bit 4
      BITFIELD_UINT   RXNE       : 1;      // bit 5
      BITFIELD_UINT   TC         : 1;      // bit 6
      BITFIELD_UINT   TXE        : 1;      // bit 7
    };  // SR bitfield

    /// register _USART2_SR reset value
    #define sfr_USART2_SR_RESET_VALUE   ((uint8_t) 0xC0)

  } SR;


  /** USART2 data register (DR at 0x53e1) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _USART2_DR reset value
    #define sfr_USART2_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** USART2 baud rate register 1 (BRR1 at 0x53e2) */
  union {

    /// bytewise access to BRR1
    uint8_t  byte;

    /// bitwise access to register BRR1
    struct {
      BITFIELD_UINT   USART_DIV4 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV5 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV6 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV7 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV8 : 1;      // bit 4
      BITFIELD_UINT   USART_DIV9 : 1;      // bit 5
      BITFIELD_UINT   USART_DIV10: 1;      // bit 6
      BITFIELD_UINT   USART_DIV11: 1;      // bit 7
    };  // BRR1 bitfield

    /// register _USART2_BRR1 reset value
    #define sfr_USART2_BRR1_RESET_VALUE   ((uint8_t) 0x00)

  } BRR1;


  /** USART2 baud rate register 2 (BRR2 at 0x53e3) */
  union {

    /// bytewise access to BRR2
    uint8_t  byte;

    /// bitwise access to register BRR2
    struct {
      BITFIELD_UINT   USART_DIV0 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV1 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV2 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV3 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV12: 1;      // bit 4
      BITFIELD_UINT   USART_DIV13: 1;      // bit 5
      BITFIELD_UINT   USART_DIV14: 1;      // bit 6
      BITFIELD_UINT   USART_DIV15: 1;      // bit 7
    };  // BRR2 bitfield

    /// register _USART2_BRR2 reset value
    #define sfr_USART2_BRR2_RESET_VALUE   ((uint8_t) 0x00)

  } BRR2;


  /** USART2 control register 1 (CR1 at 0x53e4) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   PIEN       : 1;      // bit 0
      BITFIELD_UINT   PS         : 1;      // bit 1
      BITFIELD_UINT   PCEN       : 1;      // bit 2
      BITFIELD_UINT   WAKE       : 1;      // bit 3
      BITFIELD_UINT   M          : 1;      // bit 4
      BITFIELD_UINT   USARTD     : 1;      // bit 5
      BITFIELD_UINT   T8         : 1;      // bit 6
      BITFIELD_UINT   R8         : 1;      // bit 7
    };  // CR1 bitfield

    /// register _USART2_CR1 reset value
    #define sfr_USART2_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** USART2 control register 2 (CR2 at 0x53e5) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SBK        : 1;      // bit 0
      BITFIELD_UINT   RWU        : 1;      // bit 1
      BITFIELD_UINT   REN        : 1;      // bit 2
      BITFIELD_UINT   TEN        : 1;      // bit 3
      BITFIELD_UINT   ILIEN      : 1;      // bit 4
      BITFIELD_UINT   RIEN       : 1;      // bit 5
      BITFIELD_UINT   TCIEN      : 1;      // bit 6
      BITFIELD_UINT   TIEN       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _USART2_CR2 reset value
    #define sfr_USART2_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** USART2 control register 3 (CR3 at 0x53e6) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   LBCL       : 1;      // bit 0
      BITFIELD_UINT   CPHA       : 1;      // bit 1
      BITFIELD_UINT   CPOL       : 1;      // bit 2
      BITFIELD_UINT   CLKEN      : 1;      // bit 3
      BITFIELD_UINT   STOP       : 2;      // bits 4-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CR3 bitfield

    /// register _USART2_CR3 reset value
    #define sfr_USART2_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /** USART2 control register 4 (CR4 at 0x53e7) */
  union {

    /// bytewise access to CR4
    uint8_t  byte;

    /// bitwise access to register CR4
    struct {
      BITFIELD_UINT   ADD        : 4;      // bits 0-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CR4 bitfield

    /// register _USART2_CR4 reset value
    #define sfr_USART2_CR4_RESET_VALUE   ((uint8_t) 0x00)

  } CR4;


  /** USART2 control register 5 (CR5 at 0x53e8) */
  union {

    /// bytewise access to CR5
    uint8_t  byte;

    /// bitwise access to register CR5
    struct {
      BITFIELD_UINT   EIE        : 1;      // bit 0
      BITFIELD_UINT   IREN       : 1;      // bit 1
      BITFIELD_UINT   IRLP       : 1;      // bit 2
      BITFIELD_UINT   HDSEL      : 1;      // bit 3
      BITFIELD_UINT   NACK       : 1;      // bit 4
      BITFIELD_UINT   SCEN       : 1;      // bit 5
      BITFIELD_UINT   DMAR       : 1;      // bit 6
      BITFIELD_UINT   DMAT       : 1;      // bit 7
    };  // CR5 bitfield

    /// register _USART2_CR5 reset value
    #define sfr_USART2_CR5_RESET_VALUE   ((uint8_t) 0x00)

  } CR5;


  /** USART2 guard time register (GTR at 0x53e9) */
  union {

    /// bytewise access to GTR
    uint8_t  byte;

    /// bitwise access to register GTR
    struct {
      BITFIELD_UINT   GT         : 8;      // bits 0-7
    };  // GTR bitfield

    /// register _USART2_GTR reset value
    #define sfr_USART2_GTR_RESET_VALUE   ((uint8_t) 0x00)

  } GTR;


  /** USART2 prescaler register (PSCR at 0x53ea) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 8;      // bits 0-7
    };  // PSCR bitfield

    /// register _USART2_PSCR reset value
    #define sfr_USART2_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;

} USART2_t;

/// access to USART2 SFR registers
#define sfr_USART2   (*((USART2_t*) 0x53e0))


//------------------------
// Module USART3
//------------------------

/** struct containing USART3 module registers */
typedef struct {

  /** USART3 status register (SR at 0x53f0) */
  union {

    /// bytewise access to SR
    uint8_t  byte;

    /// bitwise access to register SR
    struct {
      BITFIELD_UINT   PE         : 1;      // bit 0
      BITFIELD_UINT   FE         : 1;      // bit 1
      BITFIELD_UINT   NF         : 1;      // bit 2
      BITFIELD_UINT   OR         : 1;      // bit 3
      BITFIELD_UINT   IDLE       : 1;      // bit 4
      BITFIELD_UINT   RXNE       : 1;      // bit 5
      BITFIELD_UINT   TC         : 1;      // bit 6
      BITFIELD_UINT   TXE        : 1;      // bit 7
    };  // SR bitfield

    /// register _USART3_SR reset value
    #define sfr_USART3_SR_RESET_VALUE   ((uint8_t) 0xC0)

  } SR;


  /** USART3 data register (DR at 0x53f1) */
  union {

    /// bytewise access to DR
    uint8_t  byte;

    /// bitwise access to register DR
    struct {
      BITFIELD_UINT   DR         : 8;      // bits 0-7
    };  // DR bitfield

    /// register _USART3_DR reset value
    #define sfr_USART3_DR_RESET_VALUE   ((uint8_t) 0x00)

  } DR;


  /** USART3 baud rate register 1 (BRR1 at 0x53f2) */
  union {

    /// bytewise access to BRR1
    uint8_t  byte;

    /// bitwise access to register BRR1
    struct {
      BITFIELD_UINT   USART_DIV4 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV5 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV6 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV7 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV8 : 1;      // bit 4
      BITFIELD_UINT   USART_DIV9 : 1;      // bit 5
      BITFIELD_UINT   USART_DIV10: 1;      // bit 6
      BITFIELD_UINT   USART_DIV11: 1;      // bit 7
    };  // BRR1 bitfield

    /// register _USART3_BRR1 reset value
    #define sfr_USART3_BRR1_RESET_VALUE   ((uint8_t) 0x00)

  } BRR1;


  /** USART3 baud rate register 2 (BRR2 at 0x53f3) */
  union {

    /// bytewise access to BRR2
    uint8_t  byte;

    /// bitwise access to register BRR2
    struct {
      BITFIELD_UINT   USART_DIV0 : 1;      // bit 0
      BITFIELD_UINT   USART_DIV1 : 1;      // bit 1
      BITFIELD_UINT   USART_DIV2 : 1;      // bit 2
      BITFIELD_UINT   USART_DIV3 : 1;      // bit 3
      BITFIELD_UINT   USART_DIV12: 1;      // bit 4
      BITFIELD_UINT   USART_DIV13: 1;      // bit 5
      BITFIELD_UINT   USART_DIV14: 1;      // bit 6
      BITFIELD_UINT   USART_DIV15: 1;      // bit 7
    };  // BRR2 bitfield

    /// register _USART3_BRR2 reset value
    #define sfr_USART3_BRR2_RESET_VALUE   ((uint8_t) 0x00)

  } BRR2;


  /** USART3 control register 1 (CR1 at 0x53f4) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   PIEN       : 1;      // bit 0
      BITFIELD_UINT   PS         : 1;      // bit 1
      BITFIELD_UINT   PCEN       : 1;      // bit 2
      BITFIELD_UINT   WAKE       : 1;      // bit 3
      BITFIELD_UINT   M          : 1;      // bit 4
      BITFIELD_UINT   USARTD     : 1;      // bit 5
      BITFIELD_UINT   T8         : 1;      // bit 6
      BITFIELD_UINT   R8         : 1;      // bit 7
    };  // CR1 bitfield

    /// register _USART3_CR1 reset value
    #define sfr_USART3_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** USART3 control register 2 (CR2 at 0x53f5) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   SBK        : 1;      // bit 0
      BITFIELD_UINT   RWU        : 1;      // bit 1
      BITFIELD_UINT   REN        : 1;      // bit 2
      BITFIELD_UINT   TEN        : 1;      // bit 3
      BITFIELD_UINT   ILIEN      : 1;      // bit 4
      BITFIELD_UINT   RIEN       : 1;      // bit 5
      BITFIELD_UINT   TCIEN      : 1;      // bit 6
      BITFIELD_UINT   TIEN       : 1;      // bit 7
    };  // CR2 bitfield

    /// register _USART3_CR2 reset value
    #define sfr_USART3_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** USART3 control register 3 (CR3 at 0x53f6) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   LBCL       : 1;      // bit 0
      BITFIELD_UINT   CPHA       : 1;      // bit 1
      BITFIELD_UINT   CPOL       : 1;      // bit 2
      BITFIELD_UINT   CLKEN      : 1;      // bit 3
      BITFIELD_UINT   STOP       : 2;      // bits 4-5
      BITFIELD_UINT              : 2;      // 2 bits
    };  // CR3 bitfield

    /// register _USART3_CR3 reset value
    #define sfr_USART3_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /** USART3 control register 4 (CR4 at 0x53f7) */
  union {

    /// bytewise access to CR4
    uint8_t  byte;

    /// bitwise access to register CR4
    struct {
      BITFIELD_UINT   ADD        : 4;      // bits 0-3
      BITFIELD_UINT              : 4;      // 4 bits
    };  // CR4 bitfield

    /// register _USART3_CR4 reset value
    #define sfr_USART3_CR4_RESET_VALUE   ((uint8_t) 0x00)

  } CR4;


  /** USART3 control register 5 (CR5 at 0x53f8) */
  union {

    /// bytewise access to CR5
    uint8_t  byte;

    /// bitwise access to register CR5
    struct {
      BITFIELD_UINT   EIE        : 1;      // bit 0
      BITFIELD_UINT   IREN       : 1;      // bit 1
      BITFIELD_UINT   IRLP       : 1;      // bit 2
      BITFIELD_UINT   HDSEL      : 1;      // bit 3
      BITFIELD_UINT   NACK       : 1;      // bit 4
      BITFIELD_UINT   SCEN       : 1;      // bit 5
      BITFIELD_UINT   DMAR       : 1;      // bit 6
      BITFIELD_UINT   DMAT       : 1;      // bit 7
    };  // CR5 bitfield

    /// register _USART3_CR5 reset value
    #define sfr_USART3_CR5_RESET_VALUE   ((uint8_t) 0x00)

  } CR5;


  /** USART3 guard time register (GTR at 0x53f9) */
  union {

    /// bytewise access to GTR
    uint8_t  byte;

    /// bitwise access to register GTR
    struct {
      BITFIELD_UINT   GT         : 8;      // bits 0-7
    };  // GTR bitfield

    /// register _USART3_GTR reset value
    #define sfr_USART3_GTR_RESET_VALUE   ((uint8_t) 0x00)

  } GTR;


  /** USART3 prescaler register (PSCR at 0x53fa) */
  union {

    /// bytewise access to PSCR
    uint8_t  byte;

    /// bitwise access to register PSCR
    struct {
      BITFIELD_UINT   PSC        : 8;      // bits 0-7
    };  // PSCR bitfield

    /// register _USART3_PSCR reset value
    #define sfr_USART3_PSCR_RESET_VALUE   ((uint8_t) 0x00)

  } PSCR;

} USART3_t;

/// access to USART3 SFR registers
#define sfr_USART3   (*((USART3_t*) 0x53f0))


//------------------------
// Module WFE
//------------------------

/** struct containing WFE module registers */
typedef struct {

  /** WFE control register 1 (CR1 at 0x50a6) */
  union {

    /// bytewise access to CR1
    uint8_t  byte;

    /// bitwise access to register CR1
    struct {
      BITFIELD_UINT   TIM2_EV0   : 1;      // bit 0
      BITFIELD_UINT   TIM2_EV1   : 1;      // bit 1
      BITFIELD_UINT   TIM1_EV0   : 1;      // bit 2
      BITFIELD_UINT   TIM1_EV1   : 1;      // bit 3
      BITFIELD_UINT   EXTI_EV0   : 1;      // bit 4
      BITFIELD_UINT   EXTI_EV1   : 1;      // bit 5
      BITFIELD_UINT   EXTI_EV2   : 1;      // bit 6
      BITFIELD_UINT   EXTI_EV3   : 1;      // bit 7
    };  // CR1 bitfield

    /// register _WFE_CR1 reset value
    #define sfr_WFE_CR1_RESET_VALUE   ((uint8_t) 0x00)

  } CR1;


  /** WFE control register 2 (CR2 at 0x50a7) */
  union {

    /// bytewise access to CR2
    uint8_t  byte;

    /// bitwise access to register CR2
    struct {
      BITFIELD_UINT   EXTI_EV4   : 1;      // bit 0
      BITFIELD_UINT   EXTI_EV5   : 1;      // bit 1
      BITFIELD_UINT   EXTI_EV6   : 1;      // bit 2
      BITFIELD_UINT   EXTI_EV7   : 1;      // bit 3
      BITFIELD_UINT   EXTI_EVB   : 1;      // bit 4
      BITFIELD_UINT   EXTI_EVD   : 1;      // bit 5
      BITFIELD_UINT   EXTI_EVF   : 1;      // bit 6
      BITFIELD_UINT   EXTI_EVE   : 1;      // bit 7
    };  // CR2 bitfield

    /// register _WFE_CR2 reset value
    #define sfr_WFE_CR2_RESET_VALUE   ((uint8_t) 0x00)

  } CR2;


  /** WFE control register 3 (CR3 at 0x50a8) */
  union {

    /// bytewise access to CR3
    uint8_t  byte;

    /// bitwise access to register CR3
    struct {
      BITFIELD_UINT   TIM3_EV0   : 1;      // bit 0
      BITFIELD_UINT   TIM3_EV1   : 1;      // bit 1
      BITFIELD_UINT   TIM4_EV    : 1;      // bit 2
      BITFIELD_UINT   SPI1_EV    : 1;      // bit 3
      BITFIELD_UINT   I2C1_EV    : 1;      // bit 4
      BITFIELD_UINT   USART1_EV  : 1;      // bit 5
      BITFIELD_UINT   DMA1CH01_EV: 1;      // bit 6
      BITFIELD_UINT   DMA1CH23_EV: 1;      // bit 7
    };  // CR3 bitfield

    /// register _WFE_CR3 reset value
    #define sfr_WFE_CR3_RESET_VALUE   ((uint8_t) 0x00)

  } CR3;


  /** WFE control register 4 (CR4 at 0x50a9) */
  union {

    /// bytewise access to CR4
    uint8_t  byte;

    /// bitwise access to register CR4
    struct {
      BITFIELD_UINT   RTC_CSSLSE_EV: 1;      // bit 0
      BITFIELD_UINT   SPI2_EV    : 1;      // bit 1
      BITFIELD_UINT   USART2_EV  : 1;      // bit 2
      BITFIELD_UINT   USART3_EV  : 1;      // bit 3
      BITFIELD_UINT   TIM5_EV0   : 1;      // bit 4
      BITFIELD_UINT   TIM5_EV1   : 1;      // bit 5
      BITFIELD_UINT   AES_EV     : 1;      // bit 6
      BITFIELD_UINT              : 1;      // 1 bit
    };  // CR4 bitfield

    /// register _WFE_CR4 reset value
    #define sfr_WFE_CR4_RESET_VALUE   ((uint8_t) 0x00)

  } CR4;

} WFE_t;

/// access to WFE SFR registers
#define sfr_WFE   (*((WFE_t*) 0x50a6))


//------------------------
// Module WWDG
//------------------------

/** struct containing WWDG module registers */
typedef struct {

  /** WWDG control register (CR at 0x50d3) */
  union {

    /// bytewise access to CR
    uint8_t  byte;

    /// bitwise access to register CR
    struct {
      BITFIELD_UINT   T0         : 1;      // bit 0
      BITFIELD_UINT   T1         : 1;      // bit 1
      BITFIELD_UINT   T2         : 1;      // bit 2
      BITFIELD_UINT   T3         : 1;      // bit 3
      BITFIELD_UINT   T4         : 1;      // bit 4
      BITFIELD_UINT   T5         : 1;      // bit 5
      BITFIELD_UINT   T6         : 1;      // bit 6
      BITFIELD_UINT   T7         : 1;      // bit 7
    };  // CR bitfield

    /// register _WWDG_CR reset value
    #define sfr_WWDG_CR_RESET_VALUE   ((uint8_t) 0x7F)

  } CR;


  /** WWDR window register (WR at 0x50d4) */
  union {

    /// bytewise access to WR
    uint8_t  byte;

    /// bitwise access to register WR
    struct {
      BITFIELD_UINT   W0         : 1;      // bit 0
      BITFIELD_UINT   W1         : 1;      // bit 1
      BITFIELD_UINT   W2         : 1;      // bit 2
      BITFIELD_UINT   W3         : 1;      // bit 3
      BITFIELD_UINT   W4         : 1;      // bit 4
      BITFIELD_UINT   W5         : 1;      // bit 5
      BITFIELD_UINT   W6         : 1;      // bit 6
      BITFIELD_UINT   W7         : 1;      // bit 7
    };  // WR bitfield

    /// register _WWDG_WR reset value
    #define sfr_WWDG_WR_RESET_VALUE   ((uint8_t) 0x7F)

  } WR;

} WWDG_t;

/// access to WWDG SFR registers
#define sfr_WWDG   (*((WWDG_t*) 0x50d3))


// undefine local macros
#undef  BITFIELD_UINT

// required for C++
#ifdef __cplusplus
  }   // extern "C"
#endif

/*-------------------------------------------------------------------------
  END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-------------------------------------------------------------------------*/
#endif // STM8AL3189_H
