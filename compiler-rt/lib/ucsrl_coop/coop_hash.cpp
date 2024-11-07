// Copyright 2024 Matthias Bernad
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
