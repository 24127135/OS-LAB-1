#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  if(argc < 2) {
    fprintf(2, "Usage: xargs command\n");
    exit(1);
  }

  char buf[512];
  char *args[MAXARG];
  
  // Skip xargs options and find the actual command
  int cmd_index = 1;
  while(cmd_index < argc && argv[cmd_index][0] == '-') {
    // Skip option and its argument if needed
    if(cmd_index + 1 < argc && argv[cmd_index][1] == 'n') {
      cmd_index += 2;  // skip -n and its value
    } else {
      cmd_index++;
    }
  }

  // Copy command and its arguments to args array
  int nargs = 0;
  for(int i = cmd_index; i < argc && nargs < MAXARG - 1; i++) {
    args[nargs++] = argv[i];
  }

  // Read lines from stdin
  while(1) {
    int i = 0;
    
    // Read one line character by character
    while(i < sizeof(buf) - 1) {
      int n = read(0, buf + i, 1);
      
      if(n <= 0) {
        // EOF
        if(i > 0) break;  // Process partial line
        exit(0);
      }
      
      if(buf[i] == '\n') break;
      i++;
    }
    
    // Null-terminate the line
    buf[i] = 0;

    // Add line as argument and execute
    args[nargs] = buf;
    args[nargs + 1] = 0;

    if(fork() == 0) {
      exec(args[0], args);
      fprintf(2, "xargs: exec failed\n");
      exit(1);
    }
    wait(0);
  }
}
