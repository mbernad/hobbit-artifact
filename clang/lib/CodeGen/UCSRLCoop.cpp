#include "UCSRLCoop.h"

static llvm::cl::OptionCategory UCSRLCoopCat("UCSRL Coop Signature Options");

static llvm::cl::opt<long long> UCSRLCoopSecret(
    "ucsrl-coop-secret",
    llvm::cl::desc(
        "Secret used for COOP signature calculation (16 digits hex number)"),
    llvm::cl::value_desc("secret"), llvm::cl::cat(UCSRLCoopCat),
    llvm::cl::init(0x4242424242424242));

static long long UCSRLRandomNumber = UCSRLCoopSecret;

static Address CheckAndApplyNonVirtualAndVirtualOffset(
    CodeGenFunction &CGF, CodeGenFunction::VPtr Vptr,
    const CXXRecordDecl *derivedClass, const CXXRecordDecl *nearestVBase) {
  CGBuilderTy &Builder = CGF.Builder;
  CodeGenModule &CGM = CGF.CGM;

  auto thisAddress = CGF.LoadCXXThisAddress();

  // Compute where to store the address point.
  llvm::Value *virtualOffset = nullptr;
  CharUnits nonVirtualOffset = CharUnits::Zero();

  if (CGM.getCXXABI().isVirtualOffsetNeededForVTableField(CGF, Vptr)) {
    // We need to use the virtual base offset offset because the virtual base
    // might have a different offset in the most derived class.

    virtualOffset = CGM.getCXXABI().GetVirtualBaseClassOffset(
        CGF, thisAddress, Vptr.VTableClass, Vptr.NearestVBase);
    nonVirtualOffset = Vptr.OffsetFromNearestVBase;
  } else {
    // We can just use the base offset in the complete class.
    nonVirtualOffset = Vptr.Base.getBaseOffset();
  }

  if (!nonVirtualOffset.isZero() || virtualOffset) {
    // Compute the offset from the static and dynamic components.
    llvm::Value *baseOffset;
    if (!nonVirtualOffset.isZero()) {
      llvm::Type *OffsetType =
          (CGF.CGM.getTarget().getCXXABI().isItaniumFamily() &&
           CGF.CGM.getItaniumVTableContext().isRelativeLayout())
              ? CGF.Int32Ty
              : CGF.PtrDiffTy;
      baseOffset =
          llvm::ConstantInt::get(OffsetType, nonVirtualOffset.getQuantity());
      if (virtualOffset) {
        baseOffset = CGF.Builder.CreateAdd(virtualOffset, baseOffset);
      }
    } else {
      baseOffset = virtualOffset;
    }

    // Apply the base offset.
    llvm::Value *ptr = thisAddress.getPointer();
    unsigned AddrSpace = ptr->getType()->getPointerAddressSpace();
    ptr = CGF.Builder.CreateBitCast(ptr, CGF.Int8Ty->getPointerTo(AddrSpace));
    ptr = CGF.Builder.CreateInBoundsGEP(CGF.Int8Ty, ptr, baseOffset, "add.ptr");

    // If we have a virtual component, the alignment of the result will
    // be relative only to the known alignment of that vbase.
    CharUnits alignment;
    if (virtualOffset) {
      assert(nearestVBase && "virtual offset without vbase?");
      alignment = CGF.CGM.getVBaseAlignment(thisAddress.getAlignment(),
                                            derivedClass, nearestVBase);
    } else {
      alignment = thisAddress.getAlignment();
    }
    alignment = alignment.alignmentAtOffset(nonVirtualOffset);

    return Address(ptr, CGF.Int8Ty, alignment);
  } else {
    return thisAddress;
  }
}

Address GetCOOPSignatureFieldAddress(CodeGenFunction &CGF,
                                     CodeGenFunction::VPtr Vptr,
                                     const CXXRecordDecl *derivedClass,
                                     const CXXRecordDecl *nearestVBase) {

  CodeGenModule &CGM = CGF.CGM;
  CGBuilderTy &Builder = CGF.Builder;

  Address addr = CGF.LoadCXXThisAddress();

  // Compute where to store the address point.
  llvm::Value *virtualOffset = nullptr;
  CharUnits nonVirtualOffset = CharUnits::Zero();

  if (CGM.getCXXABI().isVirtualOffsetNeededForVTableField(CGF, Vptr)) {
    // We need to use the virtual base offset offset because the virtual base
    // might have a different offset in the most derived class.

    virtualOffset = CGM.getCXXABI().GetVirtualBaseClassOffset(
        CGF, addr, Vptr.VTableClass, Vptr.NearestVBase);
    nonVirtualOffset = Vptr.OffsetFromNearestVBase;
  } else {
    // We can just use the base offset in the complete class.
    nonVirtualOffset = Vptr.Base.getBaseOffset();
  }

  // Compute the offset from the static and dynamic components.
  llvm::Value *baseOffset;
  llvm::Type *OffsetType = (CGM.getTarget().getCXXABI().isItaniumFamily() &&
                            CGM.getItaniumVTableContext().isRelativeLayout())
                               ? CGF.Int32Ty
                               : CGF.PtrDiffTy;
  baseOffset =
      llvm::ConstantInt::get(OffsetType, nonVirtualOffset.getQuantity() + 8);
  if (virtualOffset) {
    baseOffset = Builder.CreateAdd(virtualOffset, baseOffset,
                                   "coop.signature.field.add");
  }

  // Apply the base offset.
  llvm::Value *ptr = addr.getPointer();
  unsigned AddrSpace = ptr->getType()->getPointerAddressSpace();
  ptr = Builder.CreateBitCast(ptr, CGF.Int8Ty->getPointerTo(AddrSpace),
                              "coop.signature.field.bitcast");
  ptr = Builder.CreateInBoundsGEP(CGF.Int8Ty, ptr, baseOffset,
                                  "coop.signature.field.gep");

  // If we have a virtual component, the alignment of the result will
  // be relative only to the known alignment of that vbase.
  CharUnits alignment;
  if (virtualOffset) {
    assert(nearestVBase && "virtual offset without vbase?");
    alignment =
        CGM.getVBaseAlignment(addr.getAlignment(), derivedClass, nearestVBase);
  } else {
    alignment = addr.getAlignment();
  }
  alignment = alignment.alignmentAtOffset(nonVirtualOffset);

  return Address(ptr, CGF.Int8Ty, alignment);
}

llvm::Value *CalculateCOOPSignatureValue(CodeGenFunction *CGF,
                                         CodeGenFunction::VPtr Vptr) {

  CGBuilderTy &Builder = CGF->Builder;
  CodeGenModule &CGM = CGF->CGM;

  Address VTableField = CheckAndApplyNonVirtualAndVirtualOffset(
      *CGF, Vptr, Vptr.VTableClass, Vptr.NearestVBase);

  auto *VtablePtr = CGF->GetVTablePtr(
      VTableField, llvm::PointerType::getUnqual(CGF->getLLVMContext()),
      Vptr.VTableClass);
  VtablePtr = Builder.CreatePtrToInt(VtablePtr, Builder.getInt64Ty(), "coop.vptr");

  auto *thisAddressValue = Builder.CreatePtrToInt(
      VTableField.getPointer(), Builder.getInt64Ty(), "coop.thisptr");
  auto *CoopSignatureValue =
      Builder.CreateXor(thisAddressValue, VtablePtr, "coop.xorvptr");
  CoopSignatureValue = Builder.CreateXor(
      CoopSignatureValue, Builder.getInt64(UCSRLRandomNumber), "coop.xorsecret");

  std::vector<llvm::Type *> ArgTypes{Builder.getInt64Ty()};
  std::vector<llvm::Value *> ArgValues{CoopSignatureValue};
  auto *UCSRLCoopHashFunctionType = llvm::FunctionType::get(Builder.getInt64Ty(), ArgTypes, false);
  auto UCSRLCoopHashFunction = CGM.getModule().getOrInsertFunction("ucsrl_coop_hash", UCSRLCoopHashFunctionType);
  CoopSignatureValue = Builder.CreateCall(UCSRLCoopHashFunction, ArgValues, "coop.hash");

  return CoopSignatureValue;
}

llvm::Value *LoadSavedCOOPSignatureValue(CodeGenFunction *CGF,
                                         CodeGenFunction::VPtr Vptr) {

  CodeGenModule &CGM = CGF->CGM;
  CGBuilderTy &Builder = CGF->Builder;

  auto CoopSignaturePtr = GetCOOPSignatureFieldAddress(
      *CGF, Vptr, Vptr.VTableClass, Vptr.NearestVBase);
  unsigned GlobalsAS = CGM.getDataLayout().getDefaultGlobalsAddressSpace();
  CoopSignaturePtr = CoopSignaturePtr.withElementType(llvm::PointerType::get(CGM.getLLVMContext(), GlobalsAS));
  auto *CoopSignatureValue = Builder.CreateLoad(CoopSignaturePtr, "coop.value");

  return CoopSignatureValue;
}

llvm::Value *StoreCOOPSignatureValue(CodeGenFunction *CGF,
                                     CodeGenFunction::VPtr Vptr) {

  CodeGenModule &CGM = CGF->CGM;
  CGBuilderTy &Builder = CGF->Builder;

  auto CoopSignaturePtr = GetCOOPSignatureFieldAddress(
      *CGF, Vptr, Vptr.VTableClass, Vptr.NearestVBase);
  unsigned GlobalsAS = CGM.getDataLayout().getDefaultGlobalsAddressSpace();
  CoopSignaturePtr = CoopSignaturePtr.withElementType(llvm::PointerType::get(CGM.getLLVMContext(), GlobalsAS));

  auto *CoopSignatureValue = CalculateCOOPSignatureValue(CGF, Vptr);
  auto *CoopSignatureStore =
      Builder.CreateStore(CoopSignatureValue, CoopSignaturePtr);

  return CoopSignatureStore;
}

bool ShouldCheckCOOPSignature(const CXXRecordDecl *RD) {

  // possible RTTI classes
  // https://itanium-cxx-abi.github.io/cxx-abi/abi.html#rtti-layout
  std::vector<std::string> exceptions{
      "std::type_info",
      "__cxxabiv1::__fundamental_type_info",
      "__cxxabiv1::__array_type_info",
      "__cxxabiv1::__function_type_info",
      "__cxxabiv1::__enum_type_info",
      "__cxxabiv1::__class_type_info",
      "__cxxabiv1::__si_class_type_info",
      "__cxxabiv1::__vmi_class_type_info",
      "__cxxabiv1::__pbase_type_info",
      "__cxxabiv1::__pointer_type_info",
      "__cxxabiv1::__pointer_to_member_type_info"};
  return std::find(exceptions.begin(), exceptions.end(),
                   RD->getQualifiedNameAsString()) == exceptions.end();
}