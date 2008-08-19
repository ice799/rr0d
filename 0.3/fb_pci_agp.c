/*
 * This file is part of the Rasta Ring 0 debug
 *
 * Copyright (C) 2004 Droids Corporation.
 * based on Deblin debuger
 *
 * $Id: fb_pci_agp.c,v 1.1 2006-06-29 08:37:30 serpilliere Exp $
 *
 */

#include "vars.h"

#include "page.h"

#include "utils.h"
#include "buffering.h"

#include "fb_pci_agp.h"

unsigned char fb_hw_padding_var1[0x1000];
unsigned char fb_hw_flying_page1[1024*0x1000];
unsigned char fb_hw_padding_var2[0x1000];


unsigned int fb_hw_flying_page1_sav = 0;
unsigned int fb_hw_flying_page1_level = 0;



