#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/meminfo.h"
#define PAGE_SIZE 4096
#define MAX_PROCS 64
#define MAX_NAME 16

// Process state strings (xv6 enums)
char *states[] = {
  [0] "UNUSED",
  [1] "USED?",
  [2] "SLEEPING",
  [3] "RUNNABLE",
  [4] "RUNNING",
  [5] "ZOMBIE"
};

// char* consume_memory() {
//     int pages = 32471;
//     char *ptr = malloc(pages * PAGE_SIZE);
//     if (ptr == 0) {
//         printf("malloc failed\n");
//         exit(1);
//     }

//     for (int i = 0; i < pages * PAGE_SIZE; i += PAGE_SIZE) {
//         ptr[i] = 1;
//     }
//     return ptr;
// }
int
main(int argc, char *argv[]) {
//   consume_memory();

  static struct sys_meminfo info;
  uint64 addr = (uint64)&info;

  if (meminfo(addr) < 0) {
    printf("meminfo: syscall failed\n");
    exit(1);
  }

  // Print overall memory usage
  printf("\n=== System Memory Info ===\n");
 
   printf("Total Memory: %ld MB (%ld pages)\n",
           (info.total_pages * PAGE_SIZE) / (1024 * 1024),
           info.total_pages);
    printf("Free Memory:  %ld MB (%ld pages)\n",
           (info.free_pages * PAGE_SIZE) / (1024 * 1024),
           info.free_pages);
    printf("Used Memory:  %ld MB (%ld pages)\n",
           (info.used_pages * PAGE_SIZE) / (1024 * 1024),
           info.used_pages);


  // Print per-process info
  printf("\n=== Per-Process Info ===\n");
//   printf("%-5s %-16s %-10s %-10s\n", "PID", "Name", "State", "Pages");
  for (int i = 0; i < info.nprocs; i++) {
    struct proc_meminfo *p = &info.pinfo[i];
    char *st = "???";
    if (p->state >= 0 && p->state < sizeof(states)/sizeof(states[0]) && states[p->state])
      st = states[p->state];
    printf("PID: %d, Name: %s, State: %s, Pages: %ld\n",
               p->pid, p->name, st, p->size_pages);
  }
//   free(mem);
  exit(0);
}