#ifndef LLVM_COOP_HASH_C_H
#define LLVM_COOP_HASH_C_H

#endif // LLVM_COOP_HASH_C_H

#include "lib/include/highwayhash/highwayhash_target.h"
#include "lib/include/highwayhash/instruction_sets.h"

using namespace highwayhash;

static const uint64_t kTestKey1[4] = {
    0x0706050403020100ull, 0x0F0E0D0C0B0A0908ull,
    0x1716151413121110ull, 0x1F1E1D1C1B1A1918ull
};

uint64_t ucsrl_coop_hash(long input) {
  HHResult64 result;
  InstructionSets::Run<HighwayHash>(kTestKey1, reinterpret_cast<const char *>(&input), 8, &result);
  return result;
}
