#ifndef LLVM_COOP_HASH_C_H
#define LLVM_COOP_HASH_C_H

#endif // LLVM_COOP_HASH_C_H

#include "xxh64.hpp"

uint64_t ucsrl_coop_hash(long input) {
  uint64_t result = xxh64::hash(reinterpret_cast<const char*> (&input), 8, 0x0706050403020100ull);
  return result;
}
