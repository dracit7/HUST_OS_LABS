
#include "buffer.h"

// init_buf returns the head pointer of a bufnode array.
ring_buf* init_buf(int size) {

  // Allocate a buf struct
  ring_buf* buf = (ring_buf*)malloc(sizeof(ring_buf));
  if (buf == NULL) {
    return NULL;
  }

  // Do not put this afterward - or it will erase `buf->nodes`
  memset(buf, 0, sizeof(ring_buf));

  // Allocate an node array
  buf->nodes = (ring_buf_node*)malloc(size * sizeof(ring_buf_node));
  if (buf->nodes == NULL) {
    return NULL;
  }

  // Initialize the array
  buf->size = size;
  memset(buf->nodes, 0, sizeof(ring_buf_node) * size);

  return buf;
}

// Read all valid data from of buf, one node each time.
int read_from_buf(int fd, ring_buf* buf) {

  // -1 indicates the end of file
  if (buf->nodes[buf->buf_tail].size == ENDSIZE) {
    return -ERR_END;
  }

  // The buffer is empty (must put this after judging EOF)
  if (buf->nodes[buf->buf_tail].status == FREE) {
    return -ERR_EMPTY;
  }

  // write the content of buf node to fd
  int size = write(
    fd, 
    (void *)(&(buf->nodes[buf->buf_tail].buffer)), 
    buf->nodes[buf->buf_tail].size
  );
  if (size < 0) {
    return -ERR_WRITE;
  }

  // mark that node as free
  buf->nodes[buf->buf_tail].status = FREE;
  buf->nodes[buf->buf_tail].size = 0;
  buf->buf_tail = (buf->buf_tail + 1) % buf->size;

  return 0;
}

// Write to a empty node in buf.
int write_to_buf(int fd, ring_buf* buf) {

  // The buffer is full
  if (buf->nodes[buf->buf_head].status == IN_USE) {
    return -ERR_FULL;
  }

  // Read in
  int size = read(
    fd,
    (void *)&(buf->nodes[buf->buf_head].buffer),
    BUFFER_SIZE
  );

  switch (size) {
  case -1: /* error */
    return -ERR_READ;
  
  case 0: /* The file is completely readed into buffer */
    buf->nodes[buf->buf_head].size = ENDSIZE;
    return -ERR_END;
  
  default:
    buf->nodes[buf->buf_head].status = IN_USE;
    buf->nodes[buf->buf_head].size = size;
    buf->buf_head = (buf->buf_head + 1) % buf->size;
  }

  return 0;
}

// free_buf recycles memory allocated to buf.
void free_buf(ring_buf* buf) {
  free(buf->nodes);
  free(buf);
}