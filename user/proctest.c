#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  struct procinfo info;

  if(procinfo(getpid(), &info) < 0) {
    fprintf(2, "proctest: procinfo failed\n");
    exit(1);
  }

  printf("name=%s pid=%d ppid=%d state=%d sz=%d\n",
         info.name, info.pid, info.ppid, info.state, (int)info.sz);
  exit(0);
}
