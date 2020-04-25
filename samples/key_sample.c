#include <rtthread.h>
#include "key.h"
#include <board.h>
#define KEYB1_PIN   GET_PIN(C,13)
/* 定时器的控制块 */
static struct rt_timer timer1;
static key_t key1;
	

/* 定时器 1 超时函数 */
static void timeout1(void* parameter)
{
	   key_process(); 
        //rt_timer_stop(&timer1);//停止不会释放内存
}


rt_uint8_t Read_KEY1_Level(void)
{
	return rt_pin_read(KEYB1_PIN);
}

void Btn1_Dowm_CallBack(void *btn)
{
  rt_kprintf("key1 Click!\r\n");
}

void Btn1_Double_CallBack(void *btn)
{
  rt_kprintf("key1 Double click!\r\n");
}

void Btn1_Long_CallBack(void *btn)
{
  rt_kprintf("key1 Long press!\r\n");
}


int timer_static_sample(void)
{

  rt_pin_mode(KEYB1_PIN, PIN_MODE_INPUT_PULLUP);
  key_create("key1",&key1, Read_KEY1_Level, 0);	
  key_attach(&key1,KEY_DOWM,Btn1_Dowm_CallBack);                     
  key_attach(&key1,KEY_DOUBLE,Btn1_Double_CallBack);                 
  key_attach(&key1,KEY_LONG,Btn1_Long_CallBack);  	
  get_key_event(&key1);
	
    /* 静态初始化定时器 */ 
    rt_timer_init(&timer1, "timer1",  /* 定时器名字是 timer1 */
                    timeout1, /* 超时时回调的处理函数 */
                    RT_NULL, /* 超时函数的入口参数 */
                    20,      /* 定时长度，以 OS Tick 为单位，即 10 个 OS Tick */
                    RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER); /* 周期性定时器 */
    /* 启动定时器 */
    rt_timer_start(&timer1);
    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(timer_static_sample, timer_static sample);


