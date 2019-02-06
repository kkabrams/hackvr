#ifndef _HV_KEYBOARD_X11_H_
#define _HV_KEYBOARD_X11_H_

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define HV_X11_KB_EVENT_MASK KeyPressMask|KeyReleaseMask

#if 0 //why do I still have this shit enabled!?!?
//exit hackvr quickly by pressing this key
#define HVK_EXIT 

//increase (INC) or decrease (DEC) X, Y, or Z of currently selected object
#define HVK_X_INC	XK_Left		//not normally used.
#define HVK_X_DEC	XK_Right	//you'd think these would be strafe
#define HVK_Y_INC	XK_Up		//maybe ignore all of these
#define HVK_Y_DEC	XK_Down		//I used to have this instead of UP and DOWN.
#define HVK_Z_INC			//but they're not.
#define HVK_Z_DEC			//these X and Z don't care about the direction the object is facing

//pass the move commands for forward, backward, left, right, up or down
#define HV_KEY_FORWARD	XK_w		//+1 in the direction the object is facing
#define HV_KEY_BACKWARD	XK_s
#define HV_KEY_LEFT	XK_a
#define HV_KEY_RIGHT	XK_d
#define HV_KEY_UP	XK_r		//up and down may either work only on Y axis
#define HV_KEY_DOWN	XK_f		//or may be relative to you're X rotation (and Z if you need that)
#define HVK_JUMP	XK_space
#define HVK_ACTION	XK_Return	//not really used actually...

//inc or dec x, y, or z rotation of currently selected object.
#define HV_KEY_XR_INC //
#define HV_KEY_XR_DEC //
#define HV_KEY_YR_INC //
#define HV_KEY_YR_DEC //
#define HV_KEY_ZR_INC //do a barrel roll?
#define HV_KEY_ZR_DEC //maybe not.
#endif

#endif
