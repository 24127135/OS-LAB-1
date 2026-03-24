#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct procinfo info;
  int pid;

  if(argc > 2) {
    fprintf(2, "usage: procinfo [pid]\n");
    exit(1);
  }

  pid = (argc == 2) ? atoi(argv[1]) : getpid();
  if(procinfo(pid, &info) < 0) {
    fprintf(2, "procinfo: failed for pid %d\n", pid);
    exit(1);
  }

  printf("Process: %s\n", info.name);
  printf("PID: %d, PPID: %d\n", info.pid, info.ppid);
  printf("State: %d\n", info.state);
  printf("Memory: %ld bytes\n", info.sz);
  exit(0);
}
