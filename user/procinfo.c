#include "kernel/types.h"
#include "user/user.h"

static char *
state_name(int state)
{
  switch(state) {
  case 0:
    return "UNUSED";
  case 1:
    return "USED";
  case 2:
    return "SLEEPING";
  case 3:
    return "RUNNABLE";
  case 4:
    return "RUNNING";
  case 5:
    return "ZOMBIE";
  default:
    return "UNKNOWN";
  }
}

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
  printf("State: %s\n", state_name(info.state));
  printf("Memory: %ld bytes\n", info.sz);
  exit(0);
}
