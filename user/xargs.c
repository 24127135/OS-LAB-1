#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  if(argc < 2) {
    fprintf(2, "usage: xargs command [args ...]\n");
    exit(1);
  }

  char buf[512];
  char *args[MAXARG];
  
  // Skip xargs options and find the actual command
  int cmd_index = 1;
  while(cmd_index < argc && argv[cmd_index][0] == '-') {
    // Accept -n N and skip it; this minimal xargs appends one input line each run.
    if(strcmp(argv[cmd_index], "-n") == 0) {
      if(cmd_index + 1 >= argc) {
        fprintf(2, "xargs: missing value for -n\n");
        exit(1);
      }
      cmd_index += 2;
    } else {
      cmd_index++;
    }
  }

  if(cmd_index >= argc) {
    fprintf(2, "xargs: missing command\n");
    exit(1);
  }

  // Copy command and its arguments to args array
  int nargs = 0;
  // Keep one slot for the input line and one for the final null.
  for(int i = cmd_index; i < argc && nargs < MAXARG - 2; i++) {
    args[nargs++] = argv[i];
  }

  if(cmd_index + nargs < argc) {
    fprintf(2, "xargs: too many initial arguments\n");
    exit(1);
  }

  // Read lines from stdin
  while(1) {
    int i = 0;
    int truncated = 1;
    
    // Read one line character by character.
    while(i < sizeof(buf) - 1) {
      int n = read(0, buf + i, 1);
      
      if(n <= 0) {
        // EOF
        if(i > 0) break;  // Process partial line
        exit(0);
      }
      
      if(buf[i] == '\n') {
        truncated = 0;
        break;
      }
      i++;
    }

    if(truncated && i == sizeof(buf) - 1) {
      char ch;
      while(read(0, &ch, 1) == 1 && ch != '\n')
        ;
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
