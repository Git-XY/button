#include "key.h"
#include <string.h>

//#ifdef PKG_USING_KEY
/*******************************************************************
 *                          Variable Declaration
 *******************************************************************/

static struct key* head_key = RT_NULL;


/*******************************************************************
 *                         Function Declaration
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, rt_uint32_t n);
static void add_key(key_t* btn);
static void key_cycle_process(key_t *btn);

void key_create(
    const char *name,key_t *btn,
    rt_uint8_t(*read_btn_level)(void),
    rt_uint8_t btn_trigger_level)
{
    if( btn == RT_NULL)
        RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct key is RT_NULL!"));

    memset(btn, 0, sizeof(struct key));      //Clear structure information

    StrnCopy(btn->name, name, KEY_NAME_MAX);    //key name


    btn->key_state         = NONE_TRIGGER;                     //key status
    btn->key_last_state    = NONE_TRIGGER;                //key last status
    btn->key_trigger_event = NONE_TRIGGER;             //key trigger event
    btn->read_key_level    = read_btn_level;              //key trigger level reading function
    btn->key_trigger_level = btn_trigger_level;        //key trigger level
    btn->key_last_level    = btn->read_key_level();    //key current level
    btn->debounce_time        = 0;

    //RT_DEBUG_LOG(RT_DEBUG_THREAD,("key create success!"));
    add_key(btn);          //Added to the singly linked list when key created
	
    //print_key_info(btn);
}

void key_attach(key_t *btn,Key_Event btn_event,key_callback btn_callback)
{
    if( btn == RT_NULL)
        RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct key is RT_NULL!"));

    if(KEY_ALL_RIGGER == btn_event)
    {
        for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
            btn->callback_function[i] = btn_callback; //A callback function triggered by a key event ,Used to handle key events
    }
    else
        btn->callback_function[btn_event] = btn_callback;
}


void key_delete(key_t *btn)
{
    struct key** curr;
    for(curr = &head_key; *curr;)
    {
        struct key* entry = *curr;
        if (entry == btn)
            *curr = entry->next;
        else
            curr = &entry->next;
    }
}

void key_process(void)
{
    struct key* pass_key;
    for(pass_key = head_key; pass_key != RT_NULL; pass_key = pass_key->next)
    {
        key_cycle_process(pass_key);
    }
}

rt_uint8_t get_key_state(key_t *btn)
{
    return (rt_uint8_t)(btn->key_state);
}

rt_uint8_t get_key_event(key_t *btn)
{
  return (rt_uint8_t)(btn->key_trigger_event);
}

void key_process_callback(void *btn)
{
  rt_uint8_t btn_event = get_key_event(btn);

  switch(btn_event)
  {
    case KEY_DOWM:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your press trigger"));
      break;
    }

    case KEY_UP:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your trigger release"));
      break;
    }

    case KEY_DOUBLE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your double-click trigger"));
      break;
    }

    case KEY_LONG:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger"));
      break;
    }

    case KEY_LONG_FREE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger free"));
      break;
    }
  }
}


void search_key(void)
{
  struct key* pass_key;
  for(pass_key = head_key; pass_key != RT_NULL; pass_key = pass_key->next)
  {
    RT_DEBUG_LOG(RT_DEBUG_THREAD,("key node have %s",pass_key->name));
  }
}


void get_key_eventlnfo(key_t *btn)
{
  for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
  {
    if(btn->callback_function[i] != 0)
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Key_Event:%d",i));
    }
  }
}


void print_key_info(key_t* btn)
{
  RT_DEBUG_LOG(RT_DEBUG_THREAD,("key struct information:\n\
              btn->Name:%s \n\
              btn->key_State:%d \n\
              btn->key_trigger_Event:%d \n\
              btn->key_trigger_Level:%d \n\
              btn->key_Last_Level:%d \n\
              ",
              btn->name,
              btn->key_state,
              btn->key_trigger_event,
              btn->key_trigger_level,
              btn->key_last_level));
  search_key();
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


static void add_key(key_t* btn)
{
    struct key *pass_key = head_key;

    while(pass_key)
    {
        pass_key = pass_key->next;
    }

    btn->next   = head_key;
    head_key = btn;
}


static void key_cycle_process(key_t *btn)
{
    rt_uint8_t current_level = (rt_uint8_t)btn->read_key_level();  

    if((current_level != btn->key_last_level)&&(++(btn->debounce_time) >= KEY_DEBOUNCE_TIME))   
    {
        btn->key_last_level = current_level;     //Update current key level
        btn->debounce_time     = 0;                 //key is pressed

    
        if((btn->key_state == NONE_TRIGGER)||(btn->key_state == KEY_DOUBLE))
        {
            btn->key_state = KEY_DOWM;
        }
        else if(btn->key_state == KEY_DOWM)
        {
            btn->key_state = KEY_UP;
            RT_DEBUG_LOG(RT_DEBUG_THREAD,("key release"));
        }
    }

    switch(btn->key_state)
    {
    case KEY_DOWM :                                        // key dowm
    {
        if(btn->key_last_level == btn->key_trigger_level)
        {
            btn->key_trigger_event = KEY_DOWM;

            if(++(btn->long_time) >= KEY_LONG_TIME)            //Update the trigger event before releasing the key as long press
            {
#ifdef LONG_FREE_TRIGGER

                btn->key_trigger_event = KEY_LONG;

#else

                if(++(btn->key_cycle) >= KEY_LONG_CYCLE)      //Continuous triggering of long press cycles
                {
                    btn->key_cycle = 0;
                    btn->key_trigger_event = KEY_LONG;
                    TRIGGER_CB(KEY_LONG);    //long triggering
                }
#endif

                if(btn->long_time == 0xFF)    //Update time overflow
                {
                    btn->long_time = KEY_LONG_TIME;
                }
                RT_DEBUG_LOG(RT_DEBUG_THREAD,("Long press"));
            }
        }

        break;
    }

    case KEY_UP :                                        //key up
    {
        if(btn->key_trigger_event == KEY_DOWM)          //Trigger click
        {
            if((btn->timer_count <= KEY_DOUBLE_TIME)&&(btn->key_last_state == KEY_DOUBLE)) // double click
            {
                btn->key_trigger_event = KEY_DOUBLE;
                TRIGGER_CB(KEY_DOUBLE);
                RT_DEBUG_LOG(RT_DEBUG_THREAD,("double click"));
                btn->key_state = NONE_TRIGGER;
                btn->key_last_state = NONE_TRIGGER;
            }
            else
            {
                btn->timer_count=0;
                btn->long_time = 0;                   //Detection long press failed, clear 0

#ifndef SINGLE_AND_DOUBLE_TRIGGER
                TRIGGER_CB(KEY_DOWM);              //click
#endif
                btn->key_state = KEY_DOUBLE;
                btn->key_last_state = KEY_DOUBLE;

            }
        }

        else if(btn->key_trigger_event == KEY_LONG)
        {
#ifdef LONG_FREE_TRIGGER
            TRIGGER_CB(KEY_LONG);                    //Long press
#else
            TRIGGER_CB(KEY_LONG_FREE);               //Long press free
#endif
            btn->long_time = 0;
            btn->key_state = NONE_TRIGGER;
            btn->key_last_state = KEY_LONG;
        }
        break;
    }

    case KEY_DOUBLE :
    {
        btn->timer_count++;                                       //Update time
        if(btn->timer_count>=KEY_DOUBLE_TIME)
        {
            btn->key_state = NONE_TRIGGER;
            btn->key_last_state = NONE_TRIGGER;
        }
#ifdef SINGLE_AND_DOUBLE_TRIGGER

        if((btn->timer_count>=KEY_DOUBLE_TIME)&&(btn->key_last_state != KEY_DOWM))
        {
            btn->timer_count=0;
            TRIGGER_CB(KEY_DOWM);
            btn->key_state = NONE_TRIGGER;
            btn->key_last_state = KEY_DOWM;
        }

#endif

        break;
    }

    default :
        break;
    }

}

//#endif
