#include <stdio.h>

int main(void) {
  printf("Please enter some text: ");

  gitline();

  char words[10];

  scanf("%s", words);

  printf("You typed: %s\n", words);
  return 0;
}
