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
