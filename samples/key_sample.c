#include <board.h>
#include <rtdevice.h>
#include "key.h"
#if defined(PKG_USING_KEY_SAMPLE)||defined(RT_DEBUG_KEY)

#define KEY_PIN   GET_PIN(C,13)
/* 定时器的控制块 */
static struct rt_timer key_timer;
static key_t key;
	

/* 定时器 1 超时函数 */
static void key_timeout(void* parameter)
{
	   key_process(); 
        //rt_timer_stop(&key_timer);//停止不会释放内存
}


static rt_uint8_t read_key_level(void)
{
	return rt_pin_read(KEY_PIN);
}

static void key_down_callback(void *btn)
{
   rt_kprintf("key Click!\r\n");
}

static void key_double_callback(void *btn)
{
  rt_kprintf("key Double click!\r\n");
}

static void key_long_callback(void *btn)
{
  rt_kprintf("key Long press!\r\n");
}

int key_sample(void)
{

  rt_pin_mode(KEY_PIN, PIN_MODE_INPUT_PULLUP);
	
  key_create("key",&key, read_key_level, 0);	
  key_attach(&key,KEY_DOWM,key_down_callback);                     
  key_attach(&key,KEY_DOUBLE,key_double_callback);                 
  key_attach(&key,KEY_LONG,key_long_callback);  	
  get_key_event(&key);

    /* 静态初始化定时器 */ 
    rt_timer_init(&key_timer, "key_timer",  /* 定时器名字是 key_timer */
                    key_timeout, /* 超时时回调的处理函数 */
                    RT_NULL, /* 超时函数的入口参数 */
                    20,      /* 定时长度，以 OS Tick 为单位，即 10 个 OS Tick */
                    RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER); /* 周期性定时器 */
    /* 启动定时器 */
    rt_timer_start(&key_timer);
    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(key_sample, key sample);

#endif

