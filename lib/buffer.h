
#ifndef _BUFFER_H
#define _BUFFER_H

#include <string.h>
#include <malloc.h>
#include <unistd.h>

#define BUFFER_SIZE 128

// Flag
#define ENDSIZE -1

// Node status
#define FREE 0
#define IN_USE 1

// Error codes
enum ERR {
  ERR_EMPTY = 1,
  ERR_WRITE,
  ERR_FULL,
  ERR_READ,
  ERR_END,
};

typedef struct {
  char buffer[BUFFER_SIZE]; // The content of this buffer node
  int status; // IN_USE | FREE
  int size; // How many bytes in `buffer` is valid
} ring_buf_node;

typedef struct {
  ring_buf_node* nodes; // Use a circular array to store buffer nodes
  int size; // The number of nodes
  int buf_tail; // the first IN_USE node's index
  int buf_head; // the first EMPTY node's index
} ring_buf;

ring_buf* init_buf(int size);
int read_from_buf(int fd, ring_buf* buf);
int write_to_buf(int fd, ring_buf* buf);
void free_buf(ring_buf* buf);

#endif