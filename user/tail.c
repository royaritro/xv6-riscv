#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/stat.h"
#define T_DEV 3
#define MAX_LINE_LEN 512

/**
 * tail.c — Minimal reimplementation of the UNIX `tail` command for xv6
 *
 * This program prints the last N lines from a file or standard input.
 * It mimics basic functionality of the UNIX `tail` command and supports:
 *
 * Usage:
 *   tail -n <number_of_lines> <file>     // Read last N lines from file
 *   cat <file> | tail -n <number_of_lines> // Read last N lines from piped input
 *   tail <file>                          // Default: print last 10 lines
 *   cat <file> | tail                    // Default: print last 10 lines from stdin
 *
 * Features:
 * - Works with both regular files and piped input
 * - Uses a circular buffer to efficiently track the last N lines
 * - Handles dynamic input line-by-line up to a max line length
 *
 * Limitations:
 * - Maximum line length is fixed (MAX_LINE_LEN)
 * - Maximum number of lines tracked is capped (MAX_LINES)
 * - Does not support options like `-c`, `+N`, or follow mode
 *
 * Designed to demonstrate file I/O, dynamic memory, and buffer management
 * in a simplified, resource-constrained environment like xv6.
 *
 * Both the implementation uses a circular buffer to store the last N lines.
 * This allows efficient memory usage and quick access to the last lines without  needing to read the entire file into memory.
 * 
 * To run test cases, run usertests tailtest
 *
 * Author: Aritro Roy (roy.aritro@nyu.edu)
 */


 /**
  * is_number - Check if a string represents a valid number
  * @s: The string to check
  * Returns: 1 if the string is a valid number, 0 otherwise
  */

int is_number(char *s) {
  for (int i = 0; s[i]; i++) {
    if (s[i] < '0' || s[i] > '9') return 0;
  }
  return 1;
}

/**
 * validate_args - Validate command line arguments for tail
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Ensures the correct usage of the tail command and checks if the file exists.
 * Exits with an error message if the arguments are invalid.
 */
void validate_args(int argc, char *argv[]) {

 /**
  * fstat - Get file status of standard input
  * @fd: File descriptor for standard input (0)
  * @st: Pointer to a stat structure to hold the file status
  */
  struct stat st;
  fstat(0, &st);
/**
   * If the arguments are 4 or 2 and not -n, check if the file exists.
   * If it does not exist, print an error message and exit.
   */
  if ((argc == 4 && strcmp(argv[1], "-n") == 0 && is_number(argv[2])) || (argc == 2 && strcmp(argv[1], "-n") != 0)) {
    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "tail: cannot open file %s\n", argv[argc - 1]);
      exit(1);
    }
    close(fd);
  }
  
  /**
   * Validate args with 4 arguments, which should be in the format:
   * tail -n <number_of_lines> <file>
   * If the arguments do not match this format, print usage and exit.
   */
  else if (argc == 4) {
    if (strcmp(argv[1], "-n") != 0 || !is_number(argv[2])) {
      fprintf(2, "Usage: tail -n <number_of_lines> <file>\n");
      exit(1);
    }
  } 
  /**
   * Validate args with 3 arguments, which should be in the format:
   * stdin | tail -n <number_of_lines>
   */
  else if (argc == 3) {
    if (strcmp(argv[1], "-n") != 0 || !is_number(argv[2]) || st.type == T_DEV) {
      fprintf(2, "Usage: tail -n <number_of_lines>\n");
      fprintf(2, "Either provide a file or pipe input into tail.\n");
      exit(1);
    }
  /**
   * Validate args with 2 arguments, which should not be in the format:
   * tail file
   * T_DEV indicates that the input is from a device (like a terminal).
   */
} else if (argc == 2 && st.type == T_DEV) {
    // If only one argument is provided, it should not be -n
    if (strcmp(argv[1], "-n") == 0) {
      fprintf(2, "Usage: tail <file>\n");
      exit(1);
    }
} else if (argc == 1 && st.type != T_DEV) {
  // 
}
else {
    fprintf(2, "Usage:\n");
    fprintf(2, "  tail -n <lines> <file>\n");
    fprintf(2, "  cat file | tail -n <lines>\n");
    fprintf(2, "  tail <file>\n");
    fprintf(2, "  cat file | tail\n");
    exit(1);
  }
}

/**
 * get_lines - Get the number of lines to read from command line arguments
 * @argc: Argument count
 * @argv: Argument vector
 * Returns: The number of lines to read, default is 10 if not specified
 */
int get_lines (int argc, char *argv[]) {
  if (argc == 4 && strcmp(argv[1], "-n") == 0) {
    return atoi(argv[2]);
  } else if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    return atoi(argv[2]);
  }
  return 10;
}

/**
 * read_last_n_lines_circular_buffer - Read the last N lines from a file using a circular buffer
 * @lines: Number of lines to read
 * @fd: File descriptor of the file to read from
 * This function reads the file line by line and stores the last N lines in a circular buffer.
 */

void read_last_n_lines_circular_buffer(int lines, int fd) {
  /**
   * Allocate memory for the circular buffer to hold the last N lines.
   */
  char **line_buf = malloc(lines * sizeof(char *));
  if (!line_buf) {
    fprintf(2, "failed to allocate line buffer array\n");
    exit(1);
  }
  for (int i = 0; i < lines; i++) {
    line_buf[i] = malloc(MAX_LINE_LEN);
    memset(line_buf[i], 0, MAX_LINE_LEN);
  }

  /**
   * Read the file line by line and store the last N lines in the circular buffer.
   * If a line exceeds the maximum length, it is truncated.
   * If the number of lines exceeds the buffer size, it overwrites the oldest line.
   * This allows efficient memory usage and quick access to the last lines without needing to read the entire file into memory.
   * head keeps track of the current position in the circular buffer,
   * count keeps track of the number of lines read so far.
   * line_idx keeps track of the current position in the line buffer.
   */
  char buf[512];
  int head = 0, count = 0;
  char line[MAX_LINE_LEN];
  int line_idx = 0;

  int n;
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
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

  /**
   * 
   * Print the last N lines stored in the circular buffer.
   */
  
  int start = (head - count + lines) % lines;
  for (int i = 0; i < count; i++) {
    int index = (start + i) % lines;
    printf("%s\n", line_buf[index]);
  }

  /**
   * Free the allocated memory for the line buffer.
   */
  for (int i = 0; i < lines; i++) {
    free(line_buf[i]);
  }
  free(line_buf);
}

int
main(int argc, char *argv[])
{

  /**
   * Validate command line arguments and check if the file exists.
   */
  validate_args(argc, argv);
  int lines = get_lines(argc, argv);

  struct stat st;
  fstat(0, &st);

  /**
   * If the input is not from a device (T_DEV), read the last N lines from standard input.
   * Otherwise, open the file specified in the arguments and read the last N lines from it
   */
  if (st.type != T_DEV) {
    read_last_n_lines_circular_buffer(lines, 0);
  } else {
    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "tail: cannot open %s\n", argv[argc - 1]);
      exit(1);
    }
    read_last_n_lines_circular_buffer(lines, fd);
    close(fd);
  }
  exit(0);
}
