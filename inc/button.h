#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <rtthread.h>
#include <rtdevice.h>

#define BUTTON_NAME_MAX          16     //名字最大为16字节

         
#ifndef BUTTON_debounce_time
#define BUTTON_debounce_time     2   //Debounce time  (n-1)*call cycle
#endif

#ifndef BUTTON_CONTINUOS_CYCLE
#define BUTTON_CONTINUOS_CYCLE   1  //Double-click the trigger cycle time  (n-1)*call cycle
#endif

#ifndef BUTTON_LONG_CYCLE
#define BUTTON_LONG_CYCLE        1  //Long press trigger cycle time   (n-1)*call cycle
#endif

#ifndef BUTTON_DOUBLE_TIME
#define BUTTON_DOUBLE_TIME      15 //Double click interval  (n-1)*call cycle  Recommended at 200-600ms
#endif

#ifndef BUTTON_long_time
#define BUTTON_long_time        50  //For n seconds ((n-1)*call cycle)ms, think long press event
#endif


#define TRIGGER_CB(event)   \
        if(btn->callback_function[event]) \
          btn->callback_function[event]((button_t*)btn)

typedef void (*button_callBack)(void*);   //The button triggers the callback function and needs to be implemented by the user.



typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_CONTINUOS,
  BUTTON_CONTINUOS_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event,                         
  NONE_TRIGGER
}Button_Event;


typedef struct button
{
 
  rt_uint8_t (*read_button_level)(void);
  
  char name[BUTTON_NAME_MAX];
  
  rt_uint8_t button_state              :   4;    /* The current state of the button (pressed or bounced) */
  rt_uint8_t button_last_state         :   4;    /* The last button state used to determine the double click */
  rt_uint8_t button_trigger_level      :   2;    /* Button trigger level */
  rt_uint8_t button_last_level         :   2;    /* Button current level */
 
  rt_uint8_t button_trigger_event;               /* Button trigger event, click, double click, long press, etc. */
  button_callBack callback_function[number_of_event];
  rt_uint8_t button_cycle;                       /* Continuous button cycle */
  rt_uint8_t timer_count;                        /* Timing */
  rt_uint8_t debounce_time;                      /* Debounce time */
  rt_uint8_t long_time;                          /* Button press duration */
  struct button *next;
}button_t;


/* Function declaration for external calls */

void button_create(const char *name,
                  button_t *btn, 
                  rt_uint8_t(*read_btn_level)(void),
                  rt_uint8_t btn_trigger_level);
                  
void button_attach(
				  button_t *btn,
				  Button_Event btn_event,
				  button_callBack btn_callback);   
 
void button_delete(button_t *btn);   	
		               
void button_process(void);
  
rt_uint8_t get_button_event(button_t *btn);

rt_uint8_t get_button_state(button_t *btn);

void button_process_callback(void *btn);
 
void search_button(void);    

void get_button_eventlnfo(button_t *btn);

void print_button_info(button_t* btn);   

#endif

