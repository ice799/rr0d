#ifndef IDH_H
#define IDT_H

#include <types.h>

unsigned int* getbaseidt(void);
unsigned int* getbasegdt(void);  
unsigned int getbaseldt(void);
unsigned int translate_logic_to_linear(unsigned int , unsigned int );
void  visualise_idt(unsigned long *);
unsigned long modifie_idt(unsigned int ,unsigned int *);
unsigned long read_idt_entry(unsigned int *);

union descriptor {
  struct gate_descriptor gd;
  struct segment_descriptor sd;
};

struct segment_descriptor {
  unsigned limit0:16;
  unsigned base0:24;
  unsigned type:5;
  unsigned dpl:2;
  unsigned present:1;
  unsigned limit1:4;
  unsigned reserved:2;
  unsigned def32:1;
  unsigned gran:1;
  unsigned base:8;
};

static inline u32 sd_offset(segment_descriptor *sd) {
  return (sd->base0 | sd->base >> 24);
}

static inline u32 sd_limit(segment_descriptor *sd) {
  return (sd->limit0 | sd->limit1 >> 16);
}

static inline u32 sd_present(segment_descriptor *sd) {
  return (sd->present);
}

static inline u32 sd_dpl(sd) {
  return (sd->dpl);
}

struct gate_descriptor {
  unsigned offset0:16;
  unsigned selector:16;
  unsigned stkcpy:5;
  unsigned reserved:3;
  unsigned type:5;
  unsigned dpl:2;
  unsigned present:1;
  unsigned offset1:16;
};

static inline u32 gd_offset(gd) {
  return (gd->offset0 | gd->offset >> 16);
}

static inline u32 gd_present(gd) {
  return (gd->present);
}

static inline u32  gd_dpl(gd) {
  return (gd->dpl);
}

static inline u32 gd_type(gd) {
  return (gd->type);
}

static inline u32 gd_selector(gd) {
  return (gd->selector);
}

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
