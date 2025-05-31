// lab2
// May 29, 2025

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

  char *input = NULL;
  size_t length = 0;

  printf("Enter programs to run.\n");
  while (true) {

    int wstatus = 0;
    printf("> ");

    // if getline does not work
    if (getline(&input, &length, stdin) == -1) {
      perror("Getline failed");
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    char *token = NULL;
    char *savePointer = NULL;

    token = strtok_r(input, " ", &savePointer);

    pid_t aChild = fork();

    if (aChild == 0) {

      execl(token, token, NULL);
      perror("Exec failure");
      exit(EXIT_FAILURE);
    }

    if (waitpid(aChild, &wstatus, 0) == -1) {
      perror("Chile failed");
      exit(EXIT_FAILURE);
    } else {
      if (WIFEXITED(wstatus)) {
        printf("Enter programs to run.\n");
      }
    }
  }

  free(input);

  return 0;
}
