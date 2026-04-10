/* 头文件声明区 */
// 包含各种硬件驱动的头文件，如I2C通信、1-Wire通信、超声波传感器、单片机寄存器、数码管和串口通信等。
#include "string.h"
#include "stdio.h"
#include <Init.h>		  // 初始化底层驱动专用头文件
#include <Key.h>		  // 按键底层驱动专用头文件
#include <Led.h>		  // LED底层驱动专用头文件
#include <STC15F2K60S2.H> // 单片机寄存器专用头文件
#include <Seg.h>		  // 数码管底层驱动专用头文件

/* 变量声明区 */
// 定义各种全局变量，包括按键状态、数码管显示数据、LED显示数据、串口接收数据等。
unsigned char Key_Val, Key_Down, Key_Old, Key_Up;			 // 按键状态变量
unsigned char Seg_Buf[8] = {10, 10, 10, 10, 10, 10, 10, 10}; // 数码管显示数据
unsigned char Seg_Point[8] = {0, 0, 0, 0, 0, 0, 0, 0};		 // 数码管小数点数据
unsigned char Seg_Pos;										 // 数码管扫描位置
unsigned char ucLed[8] = {0, 0, 0, 0, 0, 0, 0, 0};			 // LED显示数据
unsigned char ucRtc[3] = {11, 12, 13};						 // 实时时钟数据
unsigned int Slow_Down;										 // 减速计数器
bit Seg_Flag, Key_Flag;										 // 数码管和按键的标志位
unsigned int Time_1s = 0;									 // 1秒钟计数器（已修复初始化）
unsigned int Sys_Tick;										 // 系统时钟计数

/* 键盘处理函数 */
// 处理按键输入，检测按键的下降沿和上升沿，并更新按键状态。
void Key_Proc()
{
	if (Key_Flag)return;
	Key_Flag = 1;							  // 设置标志位，防止重复进入
	Key_Val = Key_Read();					  // 读取按键值
	Key_Down = Key_Val & (Key_Old ^ Key_Val); // 检测下降沿
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);  // 检测上升沿
	Key_Old = Key_Val;						  // 更新按键状态
}

/* 信息处理函数 */
// 更新数码管显示的数据
void Seg_Proc()
{
	if (Seg_Flag)return;
	Seg_Flag = 1; // 设置标志位
}

/* 其他显示函数 */
// LED显示处理函数，这里没有具体实现。
void Led_Proc() {}
	

/* 定时器1初始化函数 */
// 初始化定时器1，用于产生1ms的时钟中断，并允许中断。
void Timer1_Init(void)
{
	AUXR &= 0xBF; // 设置定时器时钟12T模式
	TMOD &= 0x0F; // 设置定时器模式为16位定时器
	TL1 = 0x18;	  // 设置定时器初始值
	TH1 = 0xFC;	  // 设置定时器初始值
	TF1 = 0;	  // 清除TF1标志位
	TR1 = 1;	  // 启动定时器
	ET1 = 1;	  // 使能定时器1中断
	EA = 1;		  // 开启全局中断
}

/* 定时器1中断服务函数 */
// 定时器1的中断服务函数，用于更新系统时钟、处理按键和数码管显示。
void Timer1_Isr(void) interrupt 3
{
	if (++Slow_Down == 400)
	{
		Seg_Flag = Slow_Down = 0; // 更新数码管显示标志位
	}
	if (Slow_Down % 10 == 0)
	{
		Key_Flag = 0; // 更新按键处理标志位
	}

	Sys_Tick++;

	Seg_Disp(Slow_Down % 8, Seg_Buf[Slow_Down % 8], Seg_Point[Slow_Down % 8]); // 更新数码管显示
	Led_Disp(Slow_Down % 8, ucLed[Slow_Down % 8]);							   // 更新LED显示
}

/* 主函数 */
// 系统初始化，设置定时器和串口，然后进入主循环。
void main()
{
	System_Init();	// 系统初始化
	Timer1_Init();	// 初始化定时器1


	while (1)
	{
		Key_Proc();	 // 处理按键
		Seg_Proc();	 // 更新数码管显示
		Led_Proc();	 // 更新LED显示
	}
}