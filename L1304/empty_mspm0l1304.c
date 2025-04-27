/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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

#include "ti_msp_dl_config.h"

#define EN_PORT     LCD_PORT
#define EN_PIN      LCD_E_PIN
#define DB7_DB4_PORT    LCD_PORT
#define DB4_PIN     LCD_DB4_PIN
#define DB5_PIN     LCD_DB5_PIN
#define DB6_PIN     LCD_DB6_PIN
#define DB7_PIN     LCD_DB7_PIN
#define DB7_DB4_PIN (DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN)
#define READ_DB7_DB4()  (DL_GPIO_readPins(DB7_DB4_PORT, DB7_DB4_PIN))

#define DECODE_START 0x80
#define DECODE_END 0xC0

volatile unsigned char LCD_Byte = '\0';
volatile unsigned char LCD_Data[30] = {[0]='\0'};
volatile unsigned char LCD_Decoding = 1;
volatile unsigned char LCD_StartFound = 0;
volatile unsigned char* LCD_Data_p = LCD_Data;
volatile unsigned char LCD_SecClk = 0;

double freq = 0.0;

//将字符串转换为小数
double findDouble(unsigned char* str) {
    double num = 0;
    int divisor = 1;
    unsigned char inPart = 1;
    while(*str != '\0') {
        if(*str == '.') {
            inPart = 0;
            str++;
        }
        if(*str >= '0' && *str <= '9') {
            if(inPart == 0) {
                divisor *= 10;
                num += (double)(*str - '0') / divisor;
            }
            else {
                num = num * 10 + *str - '0';
            }
        }
        str++;
    }
    return num;
}

void sendData(uint8_t data) {
    while(DL_UART_isBusy(UART_0_INST));
    DL_UART_transmitData(UART_0_INST, data);
}

void sendStr(char* str) {
    while(*str) {
        sendData(*str);
        str++;
    }
    sendData('\r'); sendData('\n');
}

void sendUint32(uint32_t data) {
    char str[30];
    sprintf(str, "%lu", data);
    sendStr(str);
}

void sendDouble(double data) {
    char str[30];
    sprintf(str, "%lf", data);
    sendStr(str);
}

// int _write(int file, char *ptr, int len)
// {
//     for (int i = 0; i < len; i++) {
//         while(DL_UART_isBusy(UART0));
//         DL_UART_transmitData(UART0, (uint8_t)ptr[i]);
//     }
//     return len;
// }

int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(LCD_INT_IRQN);

    while (1) {
        if(!LCD_Decoding) {
            freq = findDouble(LCD_Data_p);
            //sendStr("Freq:");
            sendDouble(freq);
            LCD_Decoding = 1;
        }
        // char tmp[16];
        // uint32_t data = DL_GPIO_readPins(EN_PORT, EN_PIN);
        // sprintf(tmp, "%lu", data);
        // sendStr(tmp);
    }
}

void GROUP1_IRQHandler(void) {
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
        case LCD_INT_IIDX:
            if(LCD_Decoding) {
                uint32_t pins = DL_GPIO_readPins(LCD_PORT, LCD_DB4_PIN | LCD_DB5_PIN | LCD_DB6_PIN | LCD_DB7_PIN);
                uint8_t nibble = 0;
                
                if(LCD_Byte == '\0') {  // 采第一个nibble，高四位
                    nibble |= (pins & DB4_PIN) ? (1 << 4) : 0;
                    nibble |= (pins & DB5_PIN) ? (1 << 5) : 0;
                    nibble |= (pins & DB6_PIN) ? (1 << 6) : 0;
                    nibble |= (pins & DB7_PIN) ? (1 << 7) : 0;
                    LCD_Byte = nibble;
                }
                else{ // 采第二个nibble，低四位
                    nibble |= (pins & DB4_PIN) ? (1 << 0) : 0;
                    nibble |= (pins & DB5_PIN) ? (1 << 1) : 0;
                    nibble |= (pins & DB6_PIN) ? (1 << 2) : 0;
                    nibble |= (pins & DB7_PIN) ? (1 << 3) : 0;
                    LCD_Byte |= nibble;  // 合成8位数据
                    
                    if(LCD_Byte == DECODE_START) {
                        LCD_Data_p = LCD_Data;
                        LCD_StartFound = 1;
                        //sendStr("LCD_Byte == DECODE_START");
                    }
                    else if(LCD_Byte == DECODE_END) {
                        LCD_Decoding = 0;
                        LCD_StartFound = 0;
                        *LCD_Data_p = '\0';
                        LCD_Data_p = LCD_Data;
                        //sendStr("LCD_Byte == DECODE_END");
                    }
                    else if(LCD_StartFound) {
                        *LCD_Data_p = LCD_Byte;
                        LCD_Data_p++;
                        //sendStr("LCD_StartFound");
                    }
                    LCD_Byte = '\0'; // 准备下一次采样
                }
            }
            break;
    }

    DL_GPIO_clearInterruptStatus(EN_PORT, EN_PIN);
}