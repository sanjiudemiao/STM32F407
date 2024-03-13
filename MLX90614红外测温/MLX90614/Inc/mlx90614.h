/********************************************
����оƬ��STM32 Controller
ģ���ͺţ�MLX90614
ͨѶ��ʽ��SPI����ͨ��
���ߣ�������
��Ȩ��δ������������ֹת��
********************************************/
//����ģ��ͷ�ļ�����
#ifndef _MLX90614_H
#define _MLX90614_H
//����ģ��ʹ��ͷ�ļ�
#include "tim.h"
#include "usart.h" 
//����ģ��ʹ�õ�����
#define SDA_R HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)
#define SDA_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)
#define SDA_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)
#define SCL_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)
#define SCL_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)

//����ģ��ʹ�õĺ���
float Tempinput(void);//��ȡ�¶Ⱥ���
void MLX90614_Init(void);//��ʼ��MLX90614
uint8_t PEC_Calculation(uint8_t*);
#endif
