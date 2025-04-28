/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
//#include <stdio.h>

uint16_t freq2index(float freq);
void sendNum(float freq);
void sendString(char* p);
void sendData(uint8_t data);
float getFreq();
float vol2gain(float voltage);
float vol2deg(float voltage);
float findFloat(unsigned char* str);

#define SAMPLE_SIZE	 210		//����1-43MHzƵ�ʵ�����ź����Ƶ�ʷ���,ÿ0.2MHz����һ����
#define RX_BUFFER_SIZE	30	//���ڽ��ջ�������С

#define E     DL_GPIO_readPins(GPIO_GRP_0_PORT, GPIO_GRP_0_E_PIN)
#define DB4   DL_GPIO_readPins(GPIO_GRP_0_PORT, GPIO_GRP_0_DB4_PIN)
#define DB5   DL_GPIO_readPins(GPIO_GRP_0_PORT, GPIO_GRP_0_DB5_PIN)
#define DB6   DL_GPIO_readPins(GPIO_GRP_0_PORT, GPIO_GRP_0_DB6_PIN)
#define DB7   DL_GPIO_readPins(GPIO_GRP_0_PORT, GPIO_GRP_0_DB7_PIN)

volatile uint8_t stage = 0;
volatile uint8_t LCD_Byte = '\0';
volatile unsigned char LCD_Data[30] = {[0]='\0'};
volatile unsigned char* LCD_Data_p = LCD_Data;
volatile uint8_t decode_done = 0;

typedef struct{
	float freq;
	float gain_db;
	float phase_deg;
}AmpPoint;

volatile AmpPoint SamplePoints[SAMPLE_SIZE];	//�洢��������źŵ���Ϣ

volatile bool gCheckADC = false;
volatile bool uart_rx_finish = false;
volatile bool uart_rx_start = false;

volatile unsigned char uart_rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t uart_rx_index = 0;		//���ջ���������

int main(void)
{
    SYSCFG_DL_init();
	//NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
	NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
	//NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	//DL_SYSCTL_enableSleepOnExit();
	DL_ADC12_startConversion(ADC12_0_INST);
	//NVIC_EnableIRQ(GPIO_GRP_0_INT_IRQN);
			
    while (1) {
			if(gCheckADC == true) {
				sendString("ADC Complete\r\n");
				DL_ADC12_disableConversions(ADC12_0_INST);
				DL_ADC12_disableInterrupt(ADC12_0_INST, DL_ADC12_INTERRUPT_MEM1_RESULT_LOADED);
				NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
				float frequency = getFreq();
				//float frequency = 10.0;
				uint16_t index = freq2index(frequency);
				sendString("index\r\n");
				SamplePoints[index].freq = frequency;
				sendString("Freq: ");

				char str[20];	sprintf(str, "%.2f", frequency);
				sendString(str);	sendString("\r\n");
				
				uint16_t vol_db = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
				uint16_t vol_deg = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
				
				// sendString("vol_db: "); sendNum((float)vol_db * 3300 / 4095);
				// sendString("vol_deg: "); sendNum((float)vol_deg * 3300 / 4095);
				
				float gain_db = vol2gain((float)vol_db * 3300 / 4095);
				float gain_deg = vol2deg((float)vol_deg * 3300 / 4095);
				sendString("gain_db: "); sendNum(gain_db);
				sendString("gain_deg: "); sendNum(gain_deg);
				
				gCheckADC = false;
				DL_ADC12_enableInterrupt(ADC12_0_INST, DL_ADC12_INTERRUPT_MEM1_RESULT_LOADED);
				NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
				DL_ADC12_enableConversions(ADC12_0_INST);
				DL_ADC12_startConversion(ADC12_0_INST);
			}
			//DL_Common_delayCycles(1000);
    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
						DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PIN_G_PIN);
			sendString("ADC Interrupt\r\n");
            gCheckADC = true;
            break;
        default:
            break;
    }
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
        case GPIO_GRP_0_E_IIDX :
		DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PIN_R_PIN);
            if (E == GPIO_GRP_0_E_PIN) {
                if(stage == 0){
                    uint8_t db4 = (DB4 >> 1);
                    uint8_t db5 = (DB5 >> 28); 
                    uint8_t db6 = (DB6 >> 31); 
                    uint8_t db7 = DB7 >> 5; 

                    LCD_Byte = '\0';
                    LCD_Byte += (db4 << 4);
                    LCD_Byte += (db5 << 5);
                    LCD_Byte += (db6 << 6);
                    LCD_Byte += (db7 << 7);
                    if(LCD_Byte == 128){
                        stage = 1;
                    }
                    
                }else if(stage == 1){
                    uint8_t db4 = DB4 >> 1;
                    uint8_t db5 = DB5 >> 28; 
                    uint8_t db6 = DB6 >> 31; 
                    uint8_t db7 = DB7 >> 5; 

                    //sendStr("try to syn");
                    LCD_Byte += db4;
                    LCD_Byte += (db5 << 1);
                    LCD_Byte += (db6 << 2);
                    LCD_Byte += (db7 << 3);
                    if(LCD_Byte == 128){
                        stage = 2;
						decode_done = 0;
                        //sendStr("syn success!");
						//sendString("syn success\r\n");
                    }else{
                        stage = 0;
                    }
                }
                else if(stage == 2){
                    uint8_t db4 = DB4 >> 1;
                    uint8_t db5 = DB5 >> 28; 
                    uint8_t db6 = DB6 >> 31; 
                    uint8_t db7 = DB7 >> 5; 

                    LCD_Byte = '\0';
                    LCD_Byte += (db4 << 4);
                    LCD_Byte += (db5 << 5);
                    LCD_Byte += (db6 << 6);
                    LCD_Byte += (db7 << 7);
                    //LCD_Data_p = LCD_Data;
                    stage = 3;
                    
                }else if(stage == 3){
                    uint8_t db4 = DB4 >> 1;
                    uint8_t db5 = DB5 >> 28; 
                    uint8_t db6 = DB6 >> 31; 
                    uint8_t db7 = DB7 >> 5; 

                    LCD_Byte += db4;
                    LCD_Byte += (db5 << 1);
                    LCD_Byte += (db6 << 2);
                    LCD_Byte += (db7 << 3);
                    //sendData('\r');
                    //sendData('\r');sendData('\n');
                    //sendStr("the end");
                    if(LCD_Byte == 192){
                        stage = 0;
                        *LCD_Data_p = '\0';
                        LCD_Data_p = LCD_Data;
                        decode_done = 1;
                    }else{
                        stage = 2;
                        *LCD_Data_p = LCD_Byte;
                        LCD_Data_p++;
                    }
                }
            }
            break;
        default:
            break;
    }
    DL_GPIO_clearInterruptStatus(GPIO_GRP_0_PORT, GPIO_GRP_0_E_PIN);
}

uint16_t freq2index(float freq) {
		if(freq < 1.0 || freq > 45.0) {
			return 0;
		}
		uint16_t index = (freq - 1.0) / 0.2f;
		return index;
}

float vol2gain(float voltage) {
		return voltage / 30 - 30;
}

float vol2deg(float voltage) {
		return -voltage / 10 + 180;
}

float getFreq() {
		NVIC_EnableIRQ(GPIO_GRP_0_INT_IRQN);
		sendString("Start E Interrupt\r\n");
		while(decode_done == 0) {
			__WFE();
		}
		NVIC_DisableIRQ(GPIO_GRP_0_INT_IRQN);
		sendString("End E Interrupt\r\n");
		return findFloat(LCD_Data_p);
}

void sendNum(float freq) {
		uint16_t sendData_1 = freq;		//��������
		uint16_t sendData_2 = (freq - sendData_1) * 10;	//һλС��
		//������������
		sendData((sendData_1 / 1000) + 48); 		//ǧλ
		sendData((sendData_1 / 100 % 10) + 48);	//��λ
		sendData((sendData_1 / 10 % 10) + 48);	//ʮλ
		sendData((sendData_1 % 10) + 48);				//��λ
		sendData(46);
		//����С������
		sendData((sendData_2) + 48);
		
		sendData(13); sendData(10);
}

void sendString(char* p) {
		while(*p != '\0') {
			sendData(*p);
			p++;
		}
}

void sendData(uint8_t data) {
		while(DL_UART_isBusy(UART_1_INST)){};
		DL_UART_Main_transmitData(UART_1_INST, data);
}
float findFloat(unsigned char* str) {
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