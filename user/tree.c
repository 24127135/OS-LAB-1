#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
tree(char *path, int depth)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512];

  if((fd = open(path, 0)) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0) {
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR) {
    close(fd);
    return;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    // Skip empty entries
    if(de.inum == 0) continue;
    
    // Skip . and ..
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    // Print indentation
    for(int i = 0; i < depth; i++)
      printf("  ");

    // Build full path
    memset(buf, 0, sizeof(buf));
    strcpy(buf, path);
    if(buf[strlen(buf) - 1] != '/') {
      buf[strlen(buf)] = '/';
    }
    strcpy(buf + strlen(buf), de.name);

    // Stat the entry to determine if it's a directory
    struct stat est;
    int efd = open(buf, 0);
    if(efd < 0) {
      fprintf(2, "tree: cannot open %s\n", buf);
      continue;
    }

    if(fstat(efd, &est) < 0) {
      fprintf(2, "tree: cannot stat %s\n", buf);
      close(efd);
      continue;
    }
    close(efd);

    // Print entry
    if(est.type == T_DIR) {
      printf("%s/\n", de.name);
      tree(buf, depth + 1);
    } else {
      printf("%s\n", de.name);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  char *path = ".";
  if(argc > 1)
    path = argv[1];

  printf("%s/\n", path);
  tree(path, 1);
  exit(0);
}
