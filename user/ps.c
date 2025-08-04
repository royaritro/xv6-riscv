#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/uproc.h"


static const char *states[] = {
    "UNUSED",
    "USED",
    "SLEEPING",
    "RUNNABLE",
    "RUNNING",
    "ZOMBIE"
};

int
main(int argc, char *argv[])
{
  static struct uproc table[64];
  uint64 addr = (uint64)table;

  printf("ps: calling getprocs() with addr=%p (%ld)\n", (void *)addr, addr);

  int n = getprocs(addr);
  printf("ps: getprocs() returned %d\n", n);
  if (n < 0) {
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }
  printf("ps: dumping process table from user buffer after syscall:\n");
  printf("PID   STATE      NAME\n");
  for (int i = 0; i < n; i++) {
    if (table[i].pid == 0) continue;
    printf("%d     %s   %s\n", table[i].pid, states[table[i].state], table[i].name);
  }
  exit(0);
}