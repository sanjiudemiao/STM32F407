//����ģ��ͷ�ļ�����
#ifndef _LMT70_H
#define _LMT70_H
//����ģ��ʹ��ͷ�ļ�
#include "stm32f4xx_hal.h"
//����ģ��ʹ�õ�����
extern uint16_t ADC_ConvertedValue[20];
extern double Temperature;

float Get_LMT70_Temperature(void);
#endif
