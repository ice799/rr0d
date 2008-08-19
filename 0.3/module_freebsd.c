#include <sys/types.h>
#include <sys/module.h>

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysproto.h>


extern int init_rr0d(void);
extern void cleanup_rr0d(void);


static int handler (struct module *module, int cmd, void *arg) {
  int error = 0;

  switch (cmd) {
  case MOD_LOAD :
    init_rr0d();
    break;
  case MOD_UNLOAD:
    cleanup_rr0d();
    break;
  default :
    error = EINVAL;
    break;
  }

  return error;
}

static moduledata_t rr0d_mod = {
  "rr0d",
  handler,
  NULL
};

DECLARE_MODULE(rr0d, rr0d_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
