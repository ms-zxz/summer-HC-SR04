#include "ax_sr04.h" 
#include "led.h"
static volatile uint32_t sr04_measurement;


void AX_SR04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	       //使能PORTA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);           //使能SYSCFG时钟
	
	//TRIG触发引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				       //PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		       //复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//ECHO测量引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				       //PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	       //复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//配置中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7); 

	//配置EXTI_Line1
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;                      //LINE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;             //中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //边沿触发类型
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                       //使能LINE
	EXTI_Init(&EXTI_InitStructure);   

	NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);             //使能TIM时钟
	
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;  
	TIM_TimeBaseInitStructure.TIM_Prescaler = (72-1);               //1us
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;   //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);              //初始化TIM
	TIM_Cmd(TIM5,DISABLE);                                          //使能定时器
}



void AX_SR04_StartRanging(void)                                    //启动测量
	{    
		TRI_H;
		delay_us(30);
		TRI_L;
	}


uint32_t AX_SR04_GetDistance(void)                                // 获取距离值,实际距离，单位mm
	{    
		return (sr04_measurement*0.173-4.51);                     //模型一次函数，多次实际测量后求得系数
	}
	

void EXTI9_5_IRQHandler(void)                                    //外部中断1处理函数
{
	
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)                   //确认是否产生了EXTI Line中断
		{	
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) != 0)   // 高电平回波
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
		
    EXTI_ClearITPendingBit(EXTI_Line7);	                        //清除中断标志位
}

