#include "types.h"
#define NPROC 64

struct proc_meminfo {
  int pid;
  char name[16];
  int state;
  uint64 size_pages;
};

struct sys_meminfo {
  uint64 total_pages;
  uint64 free_pages;
  uint64 used_pages;
  int nprocs;
  struct proc_meminfo pinfo[NPROC];
};