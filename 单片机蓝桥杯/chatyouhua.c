/*==================== 头文件 ====================*/
#include "STC15F2K60S2.H"
#include "Init.h"
#include "Key.h"
#include "Seg.h"
#include "Led.h"

/*==================== 全局变量 ====================*/
// 按键
unsigned char Key_Val, Key_Down, Key_Up, Key_Old;
bit Key_Flag;

// 数码管
unsigned char Seg_Buf[8]   = {10,10,10,10,10,10,10,10};
unsigned char Seg_Point[8] = {0,0,0,0,0,0,0,0};
unsigned char Seg_Pos;
bit Seg_Flag;

// LED
unsigned char ucLed[8] = {0,0,0,0,0,0,0,0};

// 系统计时
unsigned int Sys_Tick;

// 任务分频计数器
unsigned char Key_Tick;
unsigned char Seg_Tick;

/*==================== 按键处理 ====================*/
void Key_Proc()
{
    if(Key_Flag) return;
    Key_Flag = 1;

    Key_Val = Key_Read();
    Key_Down = Key_Val & (Key_Old ^ Key_Val);
    Key_Up   = ~Key_Val & (Key_Old ^ Key_Val);
    Key_Old  = Key_Val;

    /* ===== 在这里写你的按键功能 ===== */
    /*
    if(Key_Down == 1)
    {
        ucLed[0] ^= 1;
    }
    */
}

/*==================== 数码管处理 ====================*/
void Seg_Proc()
{
    if(Seg_Flag) return;
    Seg_Flag = 1;

    /* ===== 在这里写显示逻辑 ===== */
    /*
    Seg_Buf[0] = 1;
    Seg_Buf[1] = 2;
    */
}

/*==================== LED处理 ====================*/
void Led_Proc()
{
    /* 一般LED不需要标志位，直接写逻辑 */

    /*
    ucLed[0] = 1;
    */
}

/*==================== 定时器1初始化（1ms） ====================*/
void Timer1_Init(void)
{
    AUXR &= 0xBF;     // 12T模式
    TMOD &= 0x0F;
    TMOD |= 0x10;     // 定时器1 16位

    TL1 = 0x18;
    TH1 = 0xFC;

    TF1 = 0;
    TR1 = 1;

    ET1 = 1;
    EA  = 1;
}

/*==================== 定时器1中断 ====================*/
void Timer1_Isr(void) interrupt 3
{
    /* ===== 必须重装 ===== */
    TL1 = 0x18;
    TH1 = 0xFC;

    Sys_Tick++;

    /* ===== 按键 10ms ===== */
    if(++Key_Tick == 10)
    {
        Key_Tick = 0;
        Key_Flag = 0;
    }

    /* ===== 数码管 200ms ===== */
    if(++Seg_Tick == 200)
    {
        Seg_Tick = 0;
        Seg_Flag = 0;
    }

    /* ===== 数码管动态扫描 ===== */
    Seg_Pos++;
    if(Seg_Pos >= 8) Seg_Pos = 0;

    Seg_Disp(Seg_Pos, Seg_Buf[Seg_Pos], Seg_Point[Seg_Pos]);

    /* ===== LED扫描 ===== */
    Led_Disp(Seg_Pos, ucLed[Seg_Pos]);
}

/*==================== 主函数 ====================*/
void main()
{
    System_Init();
    Timer1_Init();

    while(1)
    {
        Key_Proc();
        Seg_Proc();
        Led_Proc();
    }
}