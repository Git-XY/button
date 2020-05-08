#ifndef __KEY_H__
#define __KEY_H__

#include <rtthread.h>

#define KEY_NAME_MAX          16     //名字最大为16字节
        
#ifndef KEY_DEBOUNCE_TIME
#define KEY_DEBOUNCE_TIME     2   //Debounce time  (n-1)*call cycle
#endif

#ifndef KEY_CONTINUOS_CYCLE
#define KEY_CONTINUOS_CYCLE   1  //Double-click the trigger cycle time  (n-1)*call cycle
#endif

#ifndef KEY_LONG_CYCLE
#define KEY_LONG_CYCLE        1  //Long press trigger cycle time   (n-1)*call cycle
#endif

#ifndef KEY_DOUBLE_TIME
#define KEY_DOUBLE_TIME      15 //Double click interval  (n-1)*call cycle  Recommended at 200-600ms
#endif

#ifndef KEY_LONG_TIME
#define KEY_LONG_TIME        50  //For n seconds ((n-1)*call cycle)ms, think long press event
#endif


#define TRIGGER_CB(event)   \
        if(btn->callback_function[event]) \
          btn->callback_function[event]((key_t*)btn)

typedef void (*key_callback)(void*);   //The key triggers the callback function and needs to be implemented by the user.



typedef enum {
  KEY_DOWM = 0,
  KEY_UP,
  KEY_DOUBLE,
  KEY_LONG,
  KEY_LONG_FREE,
  KEY_ALL_RIGGER,
  number_of_event,                         
  NONE_TRIGGER
}Key_Event;


typedef struct key
{
 
  rt_uint8_t (*read_key_level)(void);
  
  char name[KEY_NAME_MAX];
  
  rt_uint8_t key_state              :   4;    /* The current state of the key (pressed or bounced) */
  rt_uint8_t key_last_state         :   4;    /* The last key state used to determine the double click */
  rt_uint8_t key_trigger_level      :   2;    /* key trigger level */
  rt_uint8_t key_last_level         :   2;    /* key current level */
 
  rt_uint8_t key_trigger_event;               /* key trigger event, click, double click, long press, etc. */
  key_callback callback_function[number_of_event];
  rt_uint8_t key_cycle;                       /* Continuous key cycle */
  rt_uint8_t timer_count;                        /* Timing */
  rt_uint8_t debounce_time;                      /* Debounce time */
  rt_uint8_t long_time;                          /* key press duration */
  struct key *next;
}key_t;


/* Function declaration for external calls */

void key_create( 
	              const char *name,
                  key_t *btn, 
                  rt_uint8_t(*read_btn_level)(void),
                  rt_uint8_t btn_trigger_level);
                  
void key_attach(
				  key_t *btn,
				  Key_Event btn_event,
				  key_callback btn_callback);   
 
void key_delete(key_t *btn);   	
		               
void key_process(void);
  
rt_uint8_t get_key_event(key_t *btn);

rt_uint8_t get_key_state(key_t *btn);

#if defined(RT_USING_KEY_PRINT)
void key_process_callback(void *btn);
 
void search_key(void);    

void get_key_eventlnfo(key_t *btn);

void print_key_info(key_t* btn);   
#endif

#endif

