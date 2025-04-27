/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0L130X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0L130X
#define CONFIG_MSPM0L1304

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_26
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_25
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM27)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM26)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM27_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM26_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)





/* Port definition for Pin Group LCD */
#define LCD_PORT                                                         (GPIOA)

/* Defines for RS: GPIOA.6 with pinCMx 7 on package pin 8 */
#define LCD_RS_PIN                                               (DL_GPIO_PIN_6)
#define LCD_RS_IOMUX                                              (IOMUX_PINCM7)
/* Defines for DB4: GPIOA.17 with pinCMx 18 on package pin 9 */
#define LCD_DB4_PIN                                             (DL_GPIO_PIN_17)
#define LCD_DB4_IOMUX                                            (IOMUX_PINCM18)
/* Defines for DB5: GPIOA.22 with pinCMx 23 on package pin 13 */
#define LCD_DB5_PIN                                             (DL_GPIO_PIN_22)
#define LCD_DB5_IOMUX                                            (IOMUX_PINCM23)
/* Defines for DB6: GPIOA.23 with pinCMx 24 on package pin 14 */
#define LCD_DB6_PIN                                             (DL_GPIO_PIN_23)
#define LCD_DB6_IOMUX                                            (IOMUX_PINCM24)
/* Defines for DB7: GPIOA.24 with pinCMx 25 on package pin 15 */
#define LCD_DB7_PIN                                             (DL_GPIO_PIN_24)
#define LCD_DB7_IOMUX                                            (IOMUX_PINCM25)
/* Defines for E: GPIOA.2 with pinCMx 3 on package pin 7 */
// pins affected by this interrupt request:["E"]
#define LCD_INT_IRQN                                            (GPIOA_INT_IRQn)
#define LCD_INT_IIDX                            (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define LCD_E_IIDX                                           (DL_GPIO_IIDX_DIO2)
#define LCD_E_PIN                                                (DL_GPIO_PIN_2)
#define LCD_E_IOMUX                                               (IOMUX_PINCM3)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_UART_0_init(void);



#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
