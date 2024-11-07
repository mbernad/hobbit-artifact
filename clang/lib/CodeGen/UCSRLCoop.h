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

#ifndef LLVM_UCSRLCOOP_H
#define LLVM_UCSRLCOOP_H

#endif // LLVM_UCSRLCOOP_H

#include "CGCXXABI.h"
#include "CodeGenFunction.h"
#include "llvm/Support/RandomNumberGenerator.h"

using namespace clang;
using namespace CodeGen;

Address GetCOOPSignatureFieldAddress(CodeGenFunction &CGF,
                                     CodeGenFunction::VPtr Vptr,
                                     const CXXRecordDecl *derivedClass,
                                     const CXXRecordDecl *nearestVBase);

llvm::Value *CalculateCOOPSignatureValue(CodeGenFunction *CGF,
                                         CodeGenFunction::VPtr Vptr);

llvm::Value *LoadSavedCOOPSignatureValue(CodeGenFunction *CGF,
                                         CodeGenFunction::VPtr Vptr);

llvm::Value *StoreCOOPSignatureValue(CodeGenFunction *CGF,
                                     CodeGenFunction::VPtr Vptr);

bool ShouldCheckCOOPSignature(const CXXRecordDecl *CXXRecordDecl);

bool HasContainerField(const CXXRecordDecl *CXXRecordDecl);
