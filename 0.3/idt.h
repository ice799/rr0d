#ifndef IDH_H
#define IDT_H

unsigned int* getbaseidt(void);
unsigned int* getbasegdt(void);  
unsigned int getbaseldt(void);


unsigned int translate_logic_to_linear(unsigned int , unsigned int );


void  visualise_idt(unsigned long *);

unsigned long modifie_idt(unsigned int ,unsigned int *);

unsigned long read_idt_entry(unsigned int *);


#endif /* IDT_H */
