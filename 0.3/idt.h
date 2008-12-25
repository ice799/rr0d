#ifndef IDH_H
#define IDT_H

#include <types.h>

unsigned int* getbaseidt(void);
unsigned int* getbasegdt(void);  
unsigned int getbaseldt(void);


unsigned int translate_logic_to_linear(unsigned int , unsigned int );


void  visualise_idt(unsigned long *);

unsigned long modifie_idt(unsigned int ,unsigned int *);

typedef struct {
  u16 limit;
  u32 base;
} IDTR;

typedef struct {
  u16 limit;
  u32 base;
} GDTR;

typedef struct {
  u16 limit;
  u32 base;
} LDTR;

#define IDT_MAX (0xFF)


#endif /* IDT_H */
