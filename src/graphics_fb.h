#ifndef _HACKVR_GRAPHICS_FB_H_
#define _HACKVR_GRAPHICS_FB_H_

#include <linux/input.h>

#define DRAW_MODE_CLEAR	0x0
#define DRAW_MODE_AND	0x1
#define DRAW_MODE_COPY	0x3
#define DRAW_MODE_OR	0x7

struct fb_global {
  int fb;
  int kb;
  int draw_mode;
  unsigned int current_color;
  unsigned int *backbuf;//this is assuming unsigned int is 4 bytes and color depth is 32.
  struct fb_var_screeninfo info;
  unsigned int *buf;
  unsigned int fblen;
  char keystate[KEY_MAX/8+1];
};

#endif
#if 0
#define	GXclear			0x0		/* 0 */
#define GXand			0x1		/* src AND dst */
#define GXandReverse		0x2		/* src AND NOT dst */
#define GXcopy			0x3		/* src */
#define GXandInverted		0x4		/* NOT src AND dst */
#define	GXnoop			0x5		/* dst */
#define GXxor			0x6		/* src XOR dst */
#define GXor			0x7		/* src OR dst */
#define GXnor			0x8		/* NOT src AND NOT dst */
#define GXequiv			0x9		/* NOT src XOR dst */
#define GXinvert		0xa		/* NOT dst */
#define GXorReverse		0xb		/* src OR NOT dst */
#define GXcopyInverted		0xc		/* NOT src */
#define GXorInverted		0xd		/* NOT src OR dst */
#define GXnand			0xe		/* NOT src OR NOT dst */
#define GXset			0xf		/* 1 */
#endif
