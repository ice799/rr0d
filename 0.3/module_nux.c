/*
 * Copyright (C) 2004 Droids Corporation
 * http://www.droids-corp.org/
 *
 * important parts of this code were taken from the Linux source code,
 * Copyright (C) 1996 Linus Torvalds (file linux/mm/remap.c). See
 * http://www.kernel.org/
 *
 * $Id: module_nux.c,v 1.3 2006-06-30 12:25:35 serpilliere Exp $
 *
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <stdarg.h>


extern int init_rr0d(void);
extern void cleanup_rr0d(void);

int init_module()
{
  MODULE_LICENSE("GPL");

  init_rr0d();
  return 0;
}

void cleanup_module()
{
  cleanup_rr0d();
}
