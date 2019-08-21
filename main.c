#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "ax_sr04.h"
 
int main(void)

{    
    uint32_t len; 
	delay_init ();
    uart_init (115200); 
    printf("测距开始\r\n\r\n");    
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组
    AX_SR04_Init();
	LED_Init();

	while (1)

    {

        AX_SR04_StartRanging();
        
		len=AX_SR04_GetDistance(); 
		printf("当前距离：%d mm \r\n",len);
		
		delay_ms(100);
		
		{
			LED0=!LED0;
			LED1=!LED1;
		}
   
	}
}
