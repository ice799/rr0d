#ifndef PAGE_H
#define PAGE_H


extern unsigned int pagedir;


unsigned int* basetp(void);
unsigned int get_page_info(unsigned int adresse, unsigned int *tab_page, unsigned int *tab_page_entry, unsigned int *level );
int page_exist(unsigned int );
unsigned int set_page_info(unsigned int adresse, unsigned int data, unsigned int level);

int write_save_dirty(unsigned char* ptr, unsigned char o);
void display_page(unsigned int page_ad);

unsigned int get_page_phys(unsigned int ad);

unsigned int linear2physical(unsigned int ptr);


void poked(unsigned int ptr, unsigned int v);

void poked1(unsigned int ptr, unsigned int v);

void poked_cache(unsigned int ptr, unsigned int v);


extern unsigned int saved_wp_bit;



void clear_wp_bit(void);
void restore_wp_bit(void);



#endif

