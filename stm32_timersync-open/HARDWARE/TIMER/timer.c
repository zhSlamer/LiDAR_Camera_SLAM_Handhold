#include "timer.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>

/* 
TIM2 生成10hz PWM信号，占空比为50%，触发相机采集数据
TIM3 生成1Hz PWM信号，同时启用中断服务函数，在中断模拟GPRMC时间信息，通过uart串口发送信息

*/

//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}




vu16 varl=0;
//定时器3中断服务程序
vu16 var_Exp=0;
vu16 global_time;
char snum[7];
vu16 shorttt=0;

char gprmcStr[7]="$GPRMC,";
int chckNum=0;
char chckNumChar[2];

int ss=0;
int mm=0;
int hh=0;

unsigned char result;
int i;
int checkNum(const char *gprmcContext)
{
    if (gprmcContext == NULL) 
    {
        // printf("Input is NULL.\n");
        return -1;
		}

    result = gprmcContext[1];

    for (i = 2; gprmcContext[i] != '*' && gprmcContext[i] != '\0'; i++)
    {
        // printf("Processing character: %c (ASCII: %d)\n", gprmcContext[i], gprmcContext[i]);
        result ^= gprmcContext[i];
    }

    if (gprmcContext[i] != '*') 
    {
        // printf("No '*' found in the string.\n");
        return -1;
    }

    //printf("Final result before returning: %02X\n", result);
    return result;
}

char value_1[100]="";
char value_2[100]="";
char value_time[10]="";

char test[100]="$GPRMC,004015,A,2812.0498,N,11313.1361,E,0.0,180.0,150122,3.9,W,A*";
// 中断服务程序，处理定时器中断事件 - 以1Hz频率利用UART发送GPRMC时间信息
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );  //清除TIMx的中断待处理位:TIM 中断源 
		LED1=!LED1;
			
		//var_Exp 做半曝光时长的相移操作 重置TIM2计数器为ARR/2（实现相位同步）
		// PWM模式2 上升沿触发拍照 保证上升沿，严格一致
		TIM2->CNT=TIM2->ARR/2;
		PCout(13)=0;
	}
		
	//global_time++;
	//shorttt=0;
	//sprintf(snum, "%06d", global_time); //产生："000011D7"
	//snum[6]=0;
	//printf("$GPRMC,");
	//printf(snum);
	//printf(".00,A,2237.496474,N,11356.089515,E,0.0,225.5,310518,2.3,W,A*23\n");
	//************************************* add *********************************************
	//UTCtime format: hhmmss
	
	// 模拟GPS数据生成
    if(ss<59){
		ss++;
		}else{
			ss=0;
			if(mm<59){
				mm++;
			}else{
				mm=0;
				if(hh<23){
					hh++;
				}else{
					hh=0;
				}
			}
		}
		
    sprintf(value_2, "%s%02d%02d%02d%s", gprmcStr, hh, mm, ss, ".00,A,2237.496474,N,11356.089515,E,0.0,225.5,230520,2.3,W,A*");
	strcpy(value_1,value_2);
	// 计算异或校验和 ：计算NMEA协议（GPS数据）的校验和，通过异或运算验证数据完整性
	chckNum =checkNum(value_1); 
	sprintf(chckNumChar, "%02X", chckNum);
	printf("%s", value_2);
    printf("%s\n", chckNumChar);

	//**********************************************************************************

	/**
	chckNum=checkNum(test);	
	sprintf(chckNumChar, "%02X", chckNum);
    printf("%s\n", chckNumChar);
    */
}


// 初始化定时器2为PWM模式  - 
// 设置PA1输出-10Hz -相机
// 设置周期和占空比
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// 1 使能时钟TIM2（APB1总线）；使能时钟PA（APB2总线）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	TIM_DeInit(TIM2);
 
	// 2 配置PA1为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // PA1 - 10Hz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //??????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//???GPIO
 
   // 3 配置TIM2时钟参数
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	
	// 4 配置PMW模式	 
	// PWM模式1和模式2的区别：模式2 CNT > CCR 输出有效电平
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 				// PWM模式2（CNT > CCR时输出有效
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// 输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		// 高电平有效
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 			// CCR预装载使能
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  					// 初始化TIM2通道2
	
 	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);  

	//	TIM_SetCompare2(TIM2,50);	 // ccr  set pwm value
	TIM_SetCompare2(TIM2,TIM2->ARR/2);
}



//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

// 配置TIM3 通道2，PB5为PWM输出 并启用中断函数
void TIM3_PWM_Init(u16 arr,u16 psc)
{   // 定时器结构体、GPIO、PWM输出比较通道、中断配置结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	// 1 使能定时器时钟 以及 GPIO时钟 (AFIO重映射)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 	//Timer3部分重映射  TIM3_CH2->PB5    
 
   // 2 初始化GPIO，指定引脚、输出模式、输出速度
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // PB5（TIM3_CH2）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   // 3 配置定时器频率参数 arr psc 向上计数模式
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	// 4 配置PWM模式 选择模式2 比较输出使能 输出极性
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	
	// 5 使能预装载 和启动定时器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
	TIM_SetCompare2(TIM3,TIM3->ARR/2);	
	
	// 6 配置中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 		//使能指定的TIM3中断,允许更新中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
