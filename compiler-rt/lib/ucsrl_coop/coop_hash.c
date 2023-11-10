#ifndef LLVM_COOP_HASH_C_H
#define LLVM_COOP_HASH_C_H

#endif // LLVM_COOP_HASH_C_H

#include "lib/blake3.h"


long ucsrl_coop_hash(long input) {
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, &input, 8);
  long output;
  blake3_hasher_finalize(&hasher, &output, 8);
  return output;
}
