/********************************************
����оƬ��STM32 Controller
ģ���ͺţ�MLX90614
ͨѶ��ʽ��SPI����ͨ��
���ߣ�������
��Ȩ��δ������������ֹת��
********************************************/
#include "mlx90614.h"
//mlx90614 �˿ڶ��� 
GPIO_InitTypeDef GPIO_Initure;

/*******************************************************************************
* ������: PEC_calculation
* ���� : ����У��
* Input          : pec[]
* Output         : None
* Return         : pec[0]-this byte contains calculated crc value
*******************************************************************************/
uint8_t PEC_Calculation(uint8_t pec[])
{
    uint8_t 	crc[6];//��Ŷ���ʽ
    uint8_t	BitPosition=47;//��������������λ��6*8=48 ���λ����47λ
    uint8_t	shift;
    uint8_t	i;
    uint8_t	j;
    uint8_t	temp;

    do
    {
        /*Load pattern value 0x00 00 00 00 01 07*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;

        /*Set shift position at 0*/
        shift=0;

        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */

        /*Get shift value for pattern value*/
        shift=BitPosition-8;

        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/

        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}

void MLX90614_Init()//��ʼ��MLX90614
{
    __HAL_RCC_GPIOB_CLK_ENABLE();					
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;  				//�������
    GPIO_Initure.Pull=GPIO_PULLUP;         					//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     				//��ʼ��GPIO
		SCL_H;
		SDA_H;
}
void SMBstart()//SMB���Ϳ�ʼ��־
{ 
   SDA_H; 
   delay_us(10); 
   SCL_H; 
   delay_us(10); 
   SDA_L; 
   delay_us(10); 
   SCL_L; 
   delay_us(10); 
} 
void SMBstop()//SMB����ֹͣ��־
{ 

   SCL_L; 
   delay_us(10); 
   SDA_L; 
   delay_us(10); 
   SCL_H; 
   delay_us(10); 
   SDA_H; 
   delay_us(10); 
} 
void SMBsend(uint8_t buf)//SMB����һ���ֽ�
{ 
		uint8_t i;
    for(i=0;i<8;i++)
    {     
        SCL_L;  		
			
        if((buf&0x80)==0)  
        {SDA_L;}  
        else  
        {SDA_H;};  
				
        buf<<=1;  
        delay_us(10);     
        SCL_H;
        delay_us(10);    
		}  
		SCL_L; 
		delay_us(10); 
		SDA_H; 				   
		delay_us(10);  
		SCL_H; 
		delay_us(10);  
		SCL_L;
} 
uint8_t   SMBread()//SMB����һ���ֽ� 
{ 
		uint8_t i;
		uint8_t buf;
    for(i=0;i<8;i++)
    {     
        SCL_L;       
        delay_us(10);  			
        SDA_H; 
        delay_us(10);  
        SCL_H;  
        delay_us(10);  			
        buf<<=1;  
        if(SDA_R){buf|=0x01;};  
        delay_us(10);  
    }
		delay_us(10);
		SCL_L;	
		delay_us(10);
		SDA_L;
		delay_us(10);
		SCL_H;
		delay_us(10);
		SCL_L;	
		return buf;
} 
float Tempinput()//��ȡ�¶Ⱥ���
{
//	uint8_t string[]={"000.00���϶�\r\n"};
	uint8_t arr[6];			// Buffer for the sent bytes
	uint8_t SMBdataL;
	uint8_t SMBdataH;
	uint8_t Pec;				// PEC byte storage
	uint8_t PecReg;			// Calculated PEC byte storage
	uint32_t SMBdata;
	float temp;
	
	//�����¶ȼ��㹫ʽ��T*0.02��-273.15
  SMBstart();//��ʼ��ʼ��־��д��
  SMBsend(0x00);//�ӻ���ַ
  SMBsend(0x07);//��������
	
  SMBstart();//�ظ���ʼ��־������
  SMBsend(0x01);  
	SMBdataL=SMBread();//���ո�λ
	SMBdataH=SMBread();//���յ�λ
	Pec=SMBread();
	SMBsend(1);
	//SMBread();
  SMBstop(); 
	delay_us(10);
	arr[5] = 0x00;		//
	arr[4] = 0x07;			//
	arr[3] = 0x00+1;	//Load array arr
	arr[2] = SMBdataL;				//
	arr[1] = SMBdataH;				//
	arr[0] = 0;					//
	PecReg=PEC_Calculation(arr);//Calculate CRC ����У��
	while(PecReg != Pec);//If received and calculated CRC are equal go out from do-while{}
	SMBdata=(SMBdataH*256)+SMBdataL;
	SMBdata=SMBdata*2;
	SMBdata=SMBdata-27315;
//	string[0]=(0x30+(SMBdata/10000));
//	string[1]=(0x30+((SMBdata%10000)/1000));
//	string[2]=(0x30+((SMBdata%1000)/100));
//	string[4]=(0x30+((SMBdata%100)/10));
//	string[5]=(0x30+((SMBdata%10)/1));
	
	temp=SMBdata*0.01;
//	printf("temp=%.2f\r\n", temp);
//	HAL_UART_Transmit(&huart1,string,sizeof(string),1000);
	return temp;
} 
