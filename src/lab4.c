#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "lib/log.h"
#include "lib/ds.h"

void ls(const char* path);
void display_file_details(struct stat* stats);
void display_mode(int mode);

int main(int argc, char const *argv[]) {

  const char* path;
  
  // Check arguments and set the root path.
  if (argc == 1) {
    path = ".";
  } else if (argc == 2) {
    path = argv[1];
  } else {
    printf("Error: Invalid arguments.\nUsage: lab4 [dir]\n");
    exit(1);
  }

  ls(path);

  return 0;
}

// ls -lR
void ls(const char* path) {

  // variables
  DIR* dir;
  struct dirent* dir_entry;
  int err;
  stack* stk = init_stack();

  // Open current directory
  dir = opendir(path);
  if (dir == NULL) {
    printf("%s is not a directory.\n", path);
    exit(1);
  }

  // Print the directory's name
  printf("\n\e[1;36m%s:\e[0m\n", path);

  // Traverse the directory
  while ((dir_entry = readdir(dir)) != NULL) {
    struct stat stats;

    // Build the complete file path
    char *file_name = (char *)malloc(FILENAME_MAX/16);
    memset(file_name, 0, FILENAME_MAX/16);
    sprintf(file_name, "%s/%s", path, dir_entry->d_name);

    // Get file's stats info
    err = stat(file_name, &stats);
    if (err < 0) {
      fault("can not read file's stats.\n\
File name: %s\n\
Error code: %d\n", file_name, errno);
    }

    // If this file is a directory, push it into stack.
    // We will deal with directories before returning.
    if (S_ISDIR(stats.st_mode)) {

      // Do not push '.' and '..' into stack.
      if (strcmp(dir_entry->d_name, ".") != 0 && 
      strcmp(dir_entry->d_name, "..") != 0) {
        push(stk, (void *)file_name);
      }

      // We will free directories' names later.
      else free(file_name);
    } else {
      free(file_name);
    }

    display_file_details(&stats);

    if (S_ISDIR(stats.st_mode))
      printf("\e[34m%s\e[0m\n", dir_entry->d_name);
    else if (S_ISLNK(stats.st_mode))
      printf("\e[36m%s\e[0m\n", dir_entry->d_name);
    else
      printf("\e[32m%s\e[0m\n", dir_entry->d_name);
  }

  // List all directories recursively
  while (!is_empty(stk)) {
    char* file_name = pop(stk);
    ls(file_name);
    free(file_name);
  }

}

// Print a file's information in a line
void display_file_details(struct stat* stats) {

  display_mode(stats->st_mode);

  // Link cnt of this file
  printf("%4d ", (int)stats->st_nlink);

  // Username
	struct passwd* pw_ptr;
	if ((pw_ptr = getpwuid(stats->st_uid)) != NULL) {
		printf("%-8s ", pw_ptr->pw_name);
	} else {
		printf("%-8d ", (int)stats->st_uid);
	}

  // Group name
	struct group* grp_ptr;
	if ((grp_ptr = getgrgid(stats->st_gid)) != NULL) {
		printf("%-8s ", grp_ptr->gr_name);
	} else {
		printf("%-8d ", (int)stats->st_gid);
	}

  // File size
  if (stats->st_size < 1024) {
    printf("%4ldB ", (long)stats->st_size);
  } else if (stats->st_size < 1024 * 1024) {
    printf("%4ldK ", (long)stats->st_size / 1024);
  } else if (stats->st_size < 1024 * 1024 * 1024) {
    printf("%4ldM ", (long)stats->st_size / 1024 / 1024);
  } else {
    printf("%4ldG ", (long)stats->st_size / 1024 / 1024 / 1024);
  }

  // Modified time
	printf("%.12s ", ctime(&stats->st_mtime) + 4);

}

// Display file mode
void display_mode(int mode) {

  char str[11] = {0};

  str[0] = S_ISDIR(mode) ? 'd' : '-'; // directory
	str[0] = S_ISCHR(mode) ? 'c' : '-'; // char decices
	str[0] = S_ISBLK(mode) ? 'b' : '-'; // block devices

	str[1] = mode & S_IRUSR ? 'r' : '-'; // user
	str[2] = mode & S_IWUSR ? 'w' : '-';
	str[3] = mode & S_IXUSR ? 'x' : '-';

	str[4] = mode & S_IRGRP ? 'r' : '-'; // group
	str[5] = mode & S_IWGRP ? 'w' : '-';
	str[6] = mode & S_IXGRP ? 'x' : '-';

	str[7] = mode & S_IROTH ? 'r' : '-'; // other
	str[8] = mode & S_IWOTH ? 'w' : '-';
	str[9] = mode & S_IXOTH ? 'x' : '-';

  printf("%s", str);
}