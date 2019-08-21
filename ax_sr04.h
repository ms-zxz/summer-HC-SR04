/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_SR04_H
#define __AX_SR04_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"

#define ECHO PAin(7)
#define TRI_H PAout(5)=1
#define TRI_L PAout(5)=0

void AX_SR04_Init(void);             //超声波测距初始化
void AX_SR04_StartRanging(void);     //启动测量
uint32_t AX_SR04_GetDistance(void);	 //获取距离值

#endif

