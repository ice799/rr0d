/*
 * Copyright (C) 2004 Droids Corporation
 * http://www.droids-corp.org/
 *
 * important parts of this code were taken from the Linux source code,
 * Copyright (C) 1996 Linus Torvalds (file linux/mm/remap.c). See
 * http://www.kernel.org/
 *
 * $Id: find_fb.c,v 1.3 2004-06-21 18:05:12 lalet Exp $
 *
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fb.h>

int init_module()
{

  /* 0 est le mineur de /dev/fb0 */
  struct fb_info *info = registered_fb[0];
#if 0
  int i;
#endif

  printk("fb is at 0x%X\n", (unsigned int) info->screen_base);

#if 0
  for (i=0; i<1024*3; i+=2) {
    * ((unsigned char *) info->disp->screen_base + i) = 0xff;
    * ((unsigned char *) info->disp->screen_base + i + 1) = 0;
  }
#endif

  return 0;
}

void cleanup_module()
{
  return ;
}
