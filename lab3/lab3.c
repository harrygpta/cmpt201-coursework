// Lab3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 5

void print_old(char *old[], int count, int start) {

  for (int i = 0; i < count; i++) {
    int index = (start + i) % SIZE;
    printf("%s", old[index]);
  }
}

void free_old(char *old[], int count) {

  for (int i = 0; i < count; i++) {
    free(old[i]);
  }
}

int main() {

  char *old[SIZE] = {NULL};
  int count = 0;
  int start = 0;

  char *line = NULL;
  size_t len = 0;

  while (1) {

    printf("Enter input: ");
    ssize_t readInput = getline(&line, &len, stdin);

    if (readInput == -1) {
      break;
    }

    if (strcmp(line, "print\n") == 0) {
      print_old(old, count, start);
      continue;
    }

    // Store input
    if (count < SIZE) {
      old[(start + count) % SIZE] = strdup(line);
      count++;
    }

    else {
      free(old[start]);
      old[start] = strdup(line);
      start = (start + 1) % SIZE;
    }
  }

  free(line);
  free_old(old, count);
  return 0;
}
