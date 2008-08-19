#ifndef FB_PCI_AGP
#define FB_PCI_AGP



extern unsigned char fb_hw_padding_var1[0x1000];
extern unsigned char fb_hw_flying_page1[1024*0x1000];
extern unsigned char fb_hw_padding_var2[0x1000];


extern unsigned int fb_hw_flying_page1_sav;
extern unsigned int fb_hw_flying_page1_level;



#endif
