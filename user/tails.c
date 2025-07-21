#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/stat.h"
#define T_DEV 3
#define MAX_LINE_LEN 512
#define MAX_LINES 128

int is_number(char *s) {
  for (int i = 0; s[i]; i++) {
    if (s[i] < '0' || s[i] > '9') return 0;
  }
  return 1;
}

void validate_args(int argc, char *argv[]) {
  struct stat st;
  fstat(0, &st);
  if (argc == 4) {
    if (strcmp(argv[1], "-n") != 0 || !is_number(argv[2])) {
      fprintf(2, "Usage: tails -n <number_of_lines> <file>\n");
      exit(1);
    }
  } else if (argc == 3) {
    if (strcmp(argv[1], "-n") != 0 || !is_number(argv[2]) || st.type == T_DEV) {
      fprintf(2, "Usage: tails -n <number_of_lines>\n");
      fprintf(2, "Either provide a file or pipe input into tails.\n");
      exit(1);
    }
} else if (argc == 2) {
    
} else {
    fprintf(2, "Usage:\n");
    fprintf(2, "  tails -n <lines> <file>\n");
    fprintf(2, "  cat file | tails -n <lines>\n");
    fprintf(2, "  tails <file>\n");
    fprintf(2, "  cat file | tails\n");
    exit(1);
}

  if (argc == 4 || (argc == 2 && strcmp(argv[1], "-n") != 0)) {
    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "tails: cannot open file %s\n", argv[argc - 1]);
      exit(1);
    }
    close(fd);
  }
}

int get_lines (int argc, char *argv[]) {
  if (argc == 4 && strcmp(argv[1], "-n") == 0) {
    return atoi(argv[2]);
  } else if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    return atoi(argv[2]);
  }
  return 10;
}

int
main(int argc, char *argv[])
{
  validate_args(argc, argv);
  int lines = get_lines(argc, argv);

  struct stat st;
  fstat(0, &st);

  printf("Tails: Reading last %d lines from %d\n", lines, st.type);
    if (st.type != T_DEV) {
      char *line_buf[MAX_LINES];
      for (int i = 0; i < MAX_LINES; i++) {
        line_buf[i] = malloc(MAX_LINE_LEN);
        memset(line_buf[i], 0, MAX_LINE_LEN);
      }

      char buf[512];
      int head = 0, count = 0;
      char line[MAX_LINE_LEN];
      int line_idx = 0;

      int n;
      while ((n = read(0, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
          if (buf[i] == '\n' || line_idx == MAX_LINE_LEN - 1) {
            line[line_idx] = '\0';
            strcpy(line_buf[head], line);
            head = (head + 1) % lines;
            if (count < lines) count++;
            line_idx = 0;
          } else {
            line[line_idx++] = buf[i];
          }
        }
      }
      int start = (head - count + lines) % lines;
      for (int i = 0; i < count; i++) {
        int index = (start + i) % lines;
        printf("%s\n", line_buf[index]);
      }

      for (int i = 0; i < MAX_LINES; i++) {
        free(line_buf[i]);
      }
  } else {
    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "tails: cannot open %s\n", argv[argc - 1]);
      exit(1);
    }

    char buf[512];
    int n;
    char *line_buf[MAX_LINES];
    for (int i = 0; i < MAX_LINES; i++) {
      line_buf[i] = malloc(MAX_LINE_LEN);
      memset(line_buf[i], 0, MAX_LINE_LEN);
    }

    int head = 0;
    int count = 0;

    char line[MAX_LINE_LEN];
    int line_idx = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
      for (int i = 0; i < n; i++) {
        if (buf[i] == '\n' || line_idx == MAX_LINE_LEN - 1) {
          line[line_idx] = '\0';
          strcpy(line_buf[head], line);
          head = (head + 1) % lines;
          if (count < lines)
            count++;
          line_idx = 0;
        } else {
          line[line_idx++] = buf[i];
        }
      }
    }

    if (fd != 0) {
      close(fd);
    }
    int start = (head - count + lines) % lines;
    for (int i = 0; i < count; i++) {
      int index = (start + i) % lines;
      printf("%s\n", line_buf[index]);
    }
    for (int i = 0; i < MAX_LINES; i++) {
      free(line_buf[i]);
    }
  }
  exit(0);
}
