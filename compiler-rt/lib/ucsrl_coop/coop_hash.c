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

#include "lib/blake3.h"


long ucsrl_coop_hash(long input) {
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, &input, 8);
  long output;
  blake3_hasher_finalize(&hasher, &output, 8);
  return output;
}
