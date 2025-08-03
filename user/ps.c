#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/uproc.h"

// TODO: You may need to define additional constants or structures
// Hint: Look at kernel/proc.h for process states

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

  int n = getprocs(addr);
  if (n < 0) {
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }
  printf("PID   STATE      NAME\n");
  for (int i = 0; i < n; i++) {
    if (table[i].pid == 0) continue;
    printf("%d     %s   %s\n", table[i].pid, states[table[i].state], table[i].name);
  }
  exit(0);
}