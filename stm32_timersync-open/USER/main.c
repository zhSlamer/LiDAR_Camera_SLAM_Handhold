#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
//灰色 SWIO  7 左4
//白色 SWCLK 9 左5
//黑色 GND 右 2

/* 
STM32 任务
一是 PWM脉冲触发信号：配置两路定时器PWM2和3，分别为10Hz以及1Hz，触发相机和激光雷达采集数据
二是 串口通信：定时器中断函数以1Hz频率利用UART向工控机发送自定义GPRMC时间信息
*/

// 优化1：配置主从定时器
// 优化2：GNSS PPS中断，外部IO中断，分频1Hz和10Hz

extern vu16 var_Exp;
int main(void)
{
	// 延时函数初始化；配置系统时钟；依赖于定时器或者SysTick提供准确延时
	delay_init();	    	 	  

	// 设置中断优先级分组；
	// STM32的中断优先级由抢占优先级和子优先级组成，这里设置为分组2，即抢占优先级2位，子优先级2位。
	// 这样整个系统中可以有4个级别的抢占优先级，每个抢占优先级内有4个子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	// 初始化串口配置 - 输出1Hz自定义GPRMC信息，用于激光雷达读取时间信息
	//uart_init(115200);	 //串口初始化为115200
	uart_init(9600);

	// LED端口初始化
 	LED_Init();			     

	// 定时器为硬件PWM模式：计算PWM频率；
	// 自动重装载值（ARR=999），预分频器（PSC=7199）
	// 定时器输出频率公式：系统时钟频率（72MHz）/(ARR+1)*(PSC+1)
	TIM2_PWM_Init(999,7199); // 10 Hz    pin_A1       

 	TIM3_PWM_Init(9999,7199);	 // 1 Hz  pin_B5


	
	while(1)
	{
		
	}
}
