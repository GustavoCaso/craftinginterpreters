#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
  }
  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

static void storeLongConstant(Chunk* chunk, uint32_t byte) {
  chunk->code[chunk->count]  = byte >> 16;
  chunk->code[chunk->count+1]  = byte >> 8;
  chunk->code[chunk->count+2]  = byte & 0xff;
}

static void updateLineForLongConstant(Chunk* chunk, int line) {
  chunk->lines[chunk->count] = line;
  chunk->lines[chunk->count + 1] = line;
  chunk->lines[chunk->count + 2] = line;
}

static void writeChunkLong(Chunk* chunk, uint32_t byte, int line) {
  if (chunk->capacity < chunk->count + LONG_CONSTANT_SIZE) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
  }
  storeLongConstant(chunk, byte);
  updateLineForLongConstant(chunk, line);
  chunk->count += LONG_CONSTANT_SIZE;
}

static int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, Value value, int line) {
  int constant = addConstant(chunk, value);
  if (constant > 255) {
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    writeChunkLong(chunk, constant, line);
  } else {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, constant, line);
  }
}
