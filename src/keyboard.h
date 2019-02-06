#ifndef _HACKVR_KEYBOARD_H_
#define _HACKVR_KEYBOARD_H_

typedef enum {
 HVK_NONE,
 HVK_ESCAPE,
 HVK_FORWARD,
 HVK_BACKWARD,
 HVK_UP,
 HVK_DOWN,
 HVK_LEFT,
 HVK_RIGHT,
 HVK_JUMP,
 HVK_DEBUG,
 HVK_ENTER,
 HVK_MAGIC
} hvk_t;

hvk_t get_keyboard_event();

#endif
