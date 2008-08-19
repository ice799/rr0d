#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>





#define SYMBOL_NAME_LEN 16


typedef struct symb_struct {
  unsigned int adress;
  char name[SYMBOL_NAME_LEN];
} Symbol;

Symbol *buffer_export;
unsigned int export_num=0;



void add_export(char* name, unsigned int ad)
{

  buffer_export = realloc(buffer_export, (export_num+1)*sizeof(Symbol));
  if (!buffer_export)
    {
      fprintf(stderr, "err malloc\n");
      exit(0);
    }

  strncpy(buffer_export[export_num].name, name, SYMBOL_NAME_LEN-1);
  buffer_export[export_num].adress = ad;
  export_num++;

}


int main(int argc, char** argv)
{

  void* handle;
  unsigned int (*my_strlen)(char*);
  char* error;
  FILE* f_list;
  char name_export[1024];
  unsigned int ad_export;
  int filtre=1;
  unsigned int a, b;




  buffer_export = malloc(1*sizeof(Symbol));
  if (!buffer_export)
    {
      fprintf(stderr, "err malloc\n");
      exit(0);
    }


  f_list = fopen("lib_list_libc", "r");

  if (!f_list)
    {
      fprintf(stderr, "open lib list failed\n");
      exit(1);
    }
#ifdef FREEBSD
  handle = dlopen("/lib/libc.so.5", RTLD_LAZY);
#else
  handle = dlopen("/lib/libc.so.6", RTLD_LAZY);
#endif



  if (!handle)
    {
      fputs(dlerror(), stderr);
      exit(1);
    }

  while(fscanf(f_list, "%s", name_export)!=-1)
    {
      if (filtre)
	{
	  if ( (name_export[0]=='_')||
	       (name_export[0]=='.'))
	    continue;
	}
      ad_export = dlsym(handle, name_export);
      if ( (error = dlerror())!=NULL)
	{
	  fprintf(stderr, "%s\n", error);
	  //exit(1);
	  continue;
	}
      
      printf("%s\t %.8X\n",name_export,  ad_export);
      add_export(name_export, ad_export);

    }

  printf("total export: %d\n", export_num);
  /*
  a = (unsigned int) buffer_export;
  b = 0;
  a = a/b;
  */
  __asm("mov buffer_export, %eax");
  __asm("mov export_num, %ebx");

  __asm("mov $0, %edx");
  __asm("mov $0x1337beef, %ecx");
  //  __asm("div %edx");
  __asm("int $0x3");




  a = strlen(argv[0]);

  __asm("int $0x3");

  a+= strlen(argv[0]);



  return a;
}

