#include "button.h"
#include <string.h>

//#ifdef PKG_USING_BUTTON
/*******************************************************************
 *                          Variable Declaration
 *******************************************************************/

static struct button* head_button = RT_NULL;


/*******************************************************************
 *                         Function Declaration
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, rt_uint32_t n);
static void add_button(button_t* btn);
static void button_cycle_process(button_t *btn);

void button_create(
    const char *name,button_t *btn,
    rt_uint8_t(*read_btn_level)(void),
    rt_uint8_t btn_trigger_level)
{
    if( btn == RT_NULL)
        RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct button is RT_NULL!"));

    memset(btn, 0, sizeof(struct button));      //Clear structure information

    StrnCopy(btn->name, name, BUTTON_NAME_MAX);    //button name


    btn->button_state         = NONE_TRIGGER;                     //Button status
    btn->button_last_state    = NONE_TRIGGER;                //Button last status
    btn->button_trigger_event = NONE_TRIGGER;             //Button trigger event
    btn->read_button_level    = read_btn_level;              //Button trigger level reading function
    btn->button_trigger_level = btn_trigger_level;        //Button trigger level
    btn->button_last_level    = btn->read_button_level();    //Button current level
    btn->debounce_time        = 0;

    //RT_DEBUG_LOG(RT_DEBUG_THREAD,("button create success!"));
    add_button(btn);          //Added to the singly linked list when button created
	
    //print_button_info(btn);
}

void button_attach(button_t *btn,Button_Event btn_event,button_callBack btn_callback)
{
    if( btn == RT_NULL)
        RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct button is RT_NULL!"));

    if(BUTTON_ALL_RIGGER == btn_event)
    {
        for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
            btn->callback_function[i] = btn_callback; //A callback function triggered by a button event ,Used to handle button events
    }
    else
        btn->callback_function[btn_event] = btn_callback;
}


void button_delete(button_t *btn)
{
    struct button** curr;
    for(curr = &head_button; *curr;)
    {
        struct button* entry = *curr;
        if (entry == btn)
            *curr = entry->next;
        else
            curr = &entry->next;
    }
}

void button_process(void)
{
    struct button* pass_button;
    for(pass_button = head_button; pass_button != RT_NULL; pass_button = pass_button->next)
    {
        button_cycle_process(pass_button);
    }
}

rt_uint8_t get_button_state(button_t *btn)
{
    return (rt_uint8_t)(btn->button_state);
}

rt_uint8_t get_button_event(button_t *btn)
{
  return (rt_uint8_t)(btn->button_trigger_event);
}

void button_process_callback(void *btn)
{
  rt_uint8_t btn_event = get_button_event(btn);

  switch(btn_event)
  {
    case BUTTON_DOWM:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your press trigger"));
      break;
    }

    case BUTTON_UP:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your trigger release"));
      break;
    }

    case BUTTON_DOUBLE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your double-click trigger"));
      break;
    }

    case BUTTON_LONG:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger"));
      break;
    }

    case BUTTON_LONG_FREE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger free"));
      break;
    }

    case BUTTON_CONTINUOS:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add your continuous trigger processing logic"));
      break;
    }

    case BUTTON_CONTINUOS_FREE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your continuous trigger release"));
      break;
    }
  }
}


void search_button(void)
{
  struct button* pass_button;
  for(pass_button = head_button; pass_button != RT_NULL; pass_button = pass_button->next)
  {
    RT_DEBUG_LOG(RT_DEBUG_THREAD,("button node have %s",pass_button->name));
  }
}


void get_button_eventlnfo(button_t *btn)
{
  for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
  {
    if(btn->callback_function[i] != 0)
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Button_Event:%d",i));
    }
  }
}


void print_button_info(button_t* btn)
{
  RT_DEBUG_LOG(RT_DEBUG_THREAD,("button struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\
              ",
              btn->name,
              btn->button_state,
              btn->button_trigger_event,
              btn->button_trigger_level,
              btn->button_last_level));
  search_button();
}

/**************************** The following is the internal call function ********************/

static char *StrnCopy(char *dst, const char *src, rt_uint32_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;
        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return (dst);
}


static void add_button(button_t* btn)
{
    struct button *pass_button = head_button;

    while(pass_button)
    {
        pass_button = pass_button->next;
    }

    btn->next   = head_button;
    head_button = btn;
}


static void button_cycle_process(button_t *btn)
{
    rt_uint8_t current_level = (rt_uint8_t)btn->read_button_level();  

    if((current_level != btn->button_last_level)&&(++(btn->debounce_time) >= BUTTON_debounce_time))   
    {
        btn->button_last_level = current_level;     //Update current button level
        btn->debounce_time     = 0;                 //button is pressed

    
        if((btn->button_state == NONE_TRIGGER)||(btn->button_state == BUTTON_DOUBLE))
        {
            btn->button_state = BUTTON_DOWM;
        }
        else if(btn->button_state == BUTTON_DOWM)
        {
            btn->button_state = BUTTON_UP;
            RT_DEBUG_LOG(RT_DEBUG_THREAD,("button release"));
        }
    }

    switch(btn->button_state)
    {
    case BUTTON_DOWM :                                        // button dowm
    {
        if(btn->button_last_level == btn->button_trigger_level)
        {
#ifdef CONTINUOS_TRIGGER                              //Support continuous triggering

            if(++(btn->button_cycle) >= BUTTON_CONTINUOS_CYCLE)
            {
                btn->button_cycle = 0;
                btn->button_trigger_event = BUTTON_CONTINUOS;
                TRIGGER_CB(BUTTON_CONTINUOS);                      //continuous triggering
                RT_DEBUG_LOG(RT_DEBUG_THREAD,("continuous triggering"));
            }

#else

            btn->button_trigger_event = BUTTON_DOWM;

            if(++(btn->long_time) >= BUTTON_long_time)            //Update the trigger event before releasing the button as long press
            {
#ifdef LONG_FREE_TRIGGER

                btn->button_trigger_event = BUTTON_LONG;

#else

                if(++(btn->button_cycle) >= BUTTON_LONG_CYCLE)      //Continuous triggering of long press cycles
                {
                    btn->button_cycle = 0;
                    btn->button_trigger_event = BUTTON_LONG;
                    TRIGGER_CB(BUTTON_LONG);    //long triggering
                }
#endif

                if(btn->long_time == 0xFF)    //Update time overflow
                {
                    btn->long_time = BUTTON_long_time;
                }
                RT_DEBUG_LOG(RT_DEBUG_THREAD,("Long press"));
            }

#endif
        }

        break;
    }

    case BUTTON_UP :                                        //button up
    {
        if(btn->button_trigger_event == BUTTON_DOWM)          //Trigger click
        {
            if((btn->timer_count <= BUTTON_DOUBLE_TIME)&&(btn->button_last_state == BUTTON_DOUBLE)) // double click
            {
                btn->button_trigger_event = BUTTON_DOUBLE;
                TRIGGER_CB(BUTTON_DOUBLE);
                RT_DEBUG_LOG(RT_DEBUG_THREAD,("double click"));
                btn->button_state = NONE_TRIGGER;
                btn->button_last_state = NONE_TRIGGER;
            }
            else
            {
                btn->timer_count=0;
                btn->long_time = 0;                   //Detection long press failed, clear 0

#ifndef SINGLE_AND_DOUBLE_TRIGGER
                TRIGGER_CB(BUTTON_DOWM);              //click
#endif
                btn->button_state = BUTTON_DOUBLE;
                btn->button_last_state = BUTTON_DOUBLE;

            }
        }

        else if(btn->button_trigger_event == BUTTON_LONG)
        {
#ifdef LONG_FREE_TRIGGER
            TRIGGER_CB(BUTTON_LONG);                    //Long press
#else
            TRIGGER_CB(BUTTON_LONG_FREE);               //Long press free
#endif
            btn->long_time = 0;
            btn->button_state = NONE_TRIGGER;
            btn->button_last_state = BUTTON_LONG;
        }

#ifdef CONTINUOS_TRIGGER
        else if(btn->button_trigger_event == BUTTON_CONTINUOS)  //Press continuously
        {
            btn->long_time = 0;
            TRIGGER_CB(BUTTON_CONTINUOS_FREE);                    //Press continuously free
            btn->button_state = NONE_TRIGGER;
            btn->button_last_state = BUTTON_CONTINUOS;
        }
#endif

        break;
    }

    case BUTTON_DOUBLE :
    {
        btn->timer_count++;                                       //Update time
        if(btn->timer_count>=BUTTON_DOUBLE_TIME)
        {
            btn->button_state = NONE_TRIGGER;
            btn->button_last_state = NONE_TRIGGER;
        }
#ifdef SINGLE_AND_DOUBLE_TRIGGER

        if((btn->timer_count>=BUTTON_DOUBLE_TIME)&&(btn->button_last_state != BUTTON_DOWM))
        {
            btn->timer_count=0;
            TRIGGER_CB(BUTTON_DOWM);
            btn->button_state = NONE_TRIGGER;
            btn->button_last_state = BUTTON_DOWM;
        }

#endif

        break;
    }

    default :
        break;
    }

}

//#endif
