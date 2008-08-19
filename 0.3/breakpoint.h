#ifndef BREAKPOINT_H
#define BREAKPOINT_H

typedef struct _bp_struct {
  unsigned long address;
  unsigned long paddress;
  unsigned char hw;
  unsigned char value;
  unsigned int attrib;
} Bp_struct;

#define NUM_BREAKPOINTS 20
#define NUM_HW_BREAKPOINTS 4
#define HW_MAGIC_VALUE 0x100
extern Bp_struct list_breakpoints[NUM_BREAKPOINTS];

#define BP_CLASSIC    0x1
#define BP_ONE_SHOT    0x2

int insert_bp(unsigned int, unsigned int, unsigned int );
int is_breakpoint(unsigned char *);

int is_hw_breakpoint(unsigned char* );

int erase_breakpoint(unsigned int );
int pass_breakpoint(unsigned int );
int spawn_breakpoint(unsigned int );

void display_breakpoints(void);
unsigned char true_byte_bp(unsigned char *);

int parse_inst(unsigned int , unsigned int , insn*);

unsigned int get_dr0_val(void);
unsigned int get_dr1_val(void);
unsigned int get_dr2_val(void);
unsigned int get_dr3_val(void);
unsigned int get_dr4_val(void);
unsigned int get_dr5_val(void);
unsigned int get_dr6_val(void);
unsigned int get_dr7_val(void);


void set_dr0_val(unsigned int value);
void set_dr1_val(unsigned int value);
void set_dr2_val(unsigned int value);
void set_dr3_val(unsigned int value);
void set_dr4_val(unsigned int value);
void set_dr5_val(unsigned int value);
void set_dr6_val(unsigned int value);
void set_dr7_val(unsigned int value);



unsigned int get_cr4_val(void);
void set_cr4_val(unsigned int value);



void reset_hw_bp(void);
void set_hw_bp(unsigned int ad);

unsigned int page_info_i(unsigned int adresse);

int check_hw_bp(void);

void enable_hw_bp(int i);
void disable_hw_bp(int i);


void enable_id_hw_bp(int id_hw_bp);
void disable_id_hw_bp(int id_hw_bp);


extern int unmapped_bp_to_shoot;

#endif /* BREAKPOINT_H */

