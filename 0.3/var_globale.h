#ifndef VAR_GLOBALE_H
#define VAR_GLOBALE_H

extern  int fired;
extern unsigned int gdt_limit;


extern unsigned long idt_[2];

extern unsigned long gdt_[2];
extern unsigned long ldt_[1];


extern unsigned int data_ptr;
extern unsigned int old_data_ptr;

extern unsigned int code_ptr;
extern unsigned int old_code_ptr;

extern char debug__[80];

typedef struct reg_struct {

	unsigned int reg_EDI;
	unsigned int reg_ESI;
	unsigned int reg_EBP;
	unsigned int reg_ESP_;
	unsigned int reg_EBX;
	unsigned int reg_EDX;
	unsigned int reg_ECX;
	unsigned int reg_EAX;
	
	unsigned int reg_ES;
	unsigned int reg_FS;
	unsigned int reg_GS;
	unsigned int reg_DS;
	
	unsigned int reg_dummy_ret;

  	unsigned int reg_ERROR;
	
	unsigned int reg_EIP;
	unsigned int reg_CS;
	unsigned int reg_EFLAG;
	unsigned int reg_ESP;
	unsigned int reg_SS;
} Reg_struct;



extern Reg_struct *reg_stack;

extern Reg_struct current_reg_stack;
/* BUG /!\ pour nux, 0x18 */

void printf_disasm(unsigned int , unsigned char* );

#endif /* VAR_GLOBALE_H */
