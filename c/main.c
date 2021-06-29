#include "common.h"
#include "chunk.h"
#include "debug.h"

int main (int argc , const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  writeConstant(&chunk, 1.4, 123);
  writeChunk(&chunk, OP_RETURN, 123);

  disassembleChunk(&chunk, "test_chunk");
  freeChunk(&chunk);
  return 0;
}
