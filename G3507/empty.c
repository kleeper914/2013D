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
	//NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
	//DL_SYSCTL_enableSleepOnExit();
	DL_ADC12_startConversion(ADC12_0_INST);
			
    while (1) {
			if(gCheckADC == true) {
				sendString("ADC Complete\r\n");
				//��Ƶ��
				float frequency = getFreq();
				//float frequency = 10.0;
				uint16_t index = freq2index(frequency);
				sendString("index\r\n");
				SamplePoints[index].freq = frequency;
				sendString("Freq: "); sendNum(frequency);
				//��Ƶ�ʺ���λ
				uint16_t vol_db = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
				uint16_t vol_deg = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
				
				// sendString("vol_db: "); sendNum((float)vol_db * 3300 / 4095);
				// sendString("vol_deg: "); sendNum((float)vol_deg * 3300 / 4095);
				
				float gain_db = vol2gain((float)vol_db * 3300 / 4095);
				float gain_deg = vol2deg((float)vol_deg * 3300 / 4095);
				sendString("gain_db: "); sendNum(gain_db);
				sendString("gain_deg: "); sendNum(gain_deg);
				
				gCheckADC = false;
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

void UART_0_INST_IRQHandler(void) {
	DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PIN_R_PIN);

	while(DL_UART_Main_isRXFIFOEmpty(UART_0_INST) == false && !uart_rx_finish) {
	uint8_t rx_data = DL_UART_Main_receiveData(UART_0_INST);
	//sendString("Receive data\r\n");
	//��ʼ��ʶ��'\n'
	if(rx_data == '\n') {
		sendString("Start\r\n");
		uart_rx_start = true;
		uart_rx_finish = false;
		uart_rx_index = 0;
	}
	//������ʶ��'\r'
	else if(rx_data == '\r' && uart_rx_start) {
		sendString("Finish\r\n");
		uart_rx_buffer[uart_rx_index] = '\0';
		uart_rx_finish = true;
		uart_rx_start = false;
		uart_rx_index = 0;
	}
	//��ȡƵ��
	else if(uart_rx_start && uart_rx_index < RX_BUFFER_SIZE - 1) {
		sendData(rx_data);
		sendString("\r\n");
		uart_rx_buffer[uart_rx_index++] = rx_data;
	}
	}
	
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
		uart_rx_finish = false;
		NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
		while(!uart_rx_finish);
		NVIC_DisableIRQ(UART_0_INST_INT_IRQN);
		
		return findFloat(uart_rx_buffer);
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