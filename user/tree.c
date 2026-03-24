#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

static void
entryname(char *name, const struct dirent *de)
{
  memmove(name, de->name, DIRSIZ);
  name[DIRSIZ] = 0;
}

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
    char name[DIRSIZ + 1];

    // Skip empty entries
    if(de.inum == 0) continue;
    entryname(name, &de);
    
    // Skip . and ..
    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    // Print indentation
    for(int i = 0; i < depth; i++)
      printf("  ");

    // Build full path
    memset(buf, 0, sizeof(buf));
    int plen = strlen(path);
    if(plen + 1 + strlen(name) + 1 > sizeof(buf)) {
      fprintf(2, "tree: path too long: %s/%s\n", path, name);
      continue;
    }
    strcpy(buf, path);
    if(plen > 0 && buf[plen - 1] != '/') {
      buf[plen] = '/';
      buf[plen + 1] = 0;
    }
    strcpy(buf + strlen(buf), name);

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
      printf("%s/\n", name);
      tree(buf, depth + 1);
    } else {
      printf("%s\n", name);
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
