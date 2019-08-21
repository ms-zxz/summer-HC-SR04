#include "ax_sr04.h" 
#include "led.h"
static volatile uint32_t sr04_measurement;


void AX_SR04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	       //ʹ��PORTAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);           //ʹ��SYSCFGʱ��
	
	//TRIG��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				       //PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		       //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//ECHO��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				       //PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	       //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//�����ж���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7); 

	//����EXTI_Line1
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;                      //LINE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;             //�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //���ش�������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                       //ʹ��LINE
	EXTI_Init(&EXTI_InitStructure);   

	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);             //ʹ��TIMʱ��
	
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseInitStructure.TIM_Prescaler = (72-1);               //1us
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;   //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);              //��ʼ��TIM
	TIM_Cmd(TIM5,DISABLE);                                          //ʹ�ܶ�ʱ��
}



void AX_SR04_StartRanging(void)                                    //��������
	{    
		TRI_H;
		delay_us(30);
		TRI_L;
	}


uint32_t AX_SR04_GetDistance(void)                                // ��ȡ����ֵ,ʵ�ʾ��룬��λmm
	{    
		return (sr04_measurement*0.173-4.51);                     //ģ��һ�κ��������ʵ�ʲ��������ϵ��
	}
	

void EXTI9_5_IRQHandler(void)                                    //�ⲿ�ж�1������
{
	
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)                   //ȷ���Ƿ������EXTI Line�ж�
		{	
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) != 0)   // �ߵ�ƽ�ز�
				{    
					TIM_Cmd(TIM5, ENABLE);  
				} 
			else 
				{
					
					TIM_Cmd(TIM5, DISABLE);  
					sr04_measurement = TIM_GetCounter(TIM5);
					TIM_SetCounter(TIM5, 0); 
				}
		}
		
    EXTI_ClearITPendingBit(EXTI_Line7);	                        //����жϱ�־λ
}

