#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr)                                                           \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s\nFile: %s, Line: %d\n", #expr,     \
              __FILE__, __LINE__);                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TEST(expr)                                                             \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed: %s\n", #expr);                             \
      exit(1);                                                                 \
    } else {                                                                   \
      printf("Test passed: %s\n", #expr);                                      \
    }                                                                          \
  }

typedef struct node {
  uint64_t data;
  struct node *next;
} node_t;

typedef struct info {
  uint64_t sum;
} info_t;

node_t *head = NULL;
info_t info = {0};

void insert_sorted(uint64_t data) {
  node_t *new_node = malloc(sizeof(node_t));
  new_node->data = data;
  new_node->next = NULL;

  if (head == NULL || data < head->data) {
    new_node->next = head;
    head = new_node;
  } else {
    node_t *curr = head;
    while (curr->next != NULL && curr->next->data < data) {
      curr = curr->next;
    }
    new_node->next = curr->next;
    curr->next = new_node;
  }

  info.sum += data;
}

int index_of(uint64_t data) {
  node_t *curr = head;
  int index = 0;

  while (curr != NULL) {
    if (curr->data == data) {
      return index;
    }

    curr = curr->next;
    index++;
  }

  return -1;
}

uint64_t compute_sum() {
  uint64_t sum = 0;
  node_t *curr = head;
  while (curr != NULL) {
    sum += curr->data;
    curr = curr->next;
  }
  return sum;
}

int main() {
  insert_sorted(1);
  insert_sorted(3);
  insert_sorted(5);
  insert_sorted(2);

  TEST(info.sum == 1 + 3 + 5 + 2);
  TEST(index_of(2) == 1);

  ASSERT(info.sum == compute_sum());

  return 0;
}
