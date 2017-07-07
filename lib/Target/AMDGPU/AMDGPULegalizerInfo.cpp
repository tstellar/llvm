//===- AMDGPULegalizerInfo.cpp -----------------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the Machinelegalizer class for
/// AMDGPU.
/// \todo This should be generated by TableGen.
//===----------------------------------------------------------------------===//

#include "AMDGPULegalizerInfo.h"
#include "AMDGPUSubtarget.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetOpcodes.h"

using namespace llvm;

#ifndef LLVM_BUILD_GLOBAL_ISEL
#error "You shouldn't build this"
#endif

AMDGPULegalizerInfo::AMDGPULegalizerInfo() {
  using namespace TargetOpcode;

  const LLT S1= LLT::scalar(1);
  const LLT V2S16 = LLT::vector(2, 16);
  const LLT S16 = LLT::scalar(16);
  const LLT S32 = LLT::scalar(32);
  const LLT V2S32 = LLT::vector(2, 32);
  const LLT V3S32 = LLT::vector(3, 32);
  const LLT V4S32 = LLT::vector(4, 32);
  const LLT V8S32 = LLT::vector(8, 32);
  const LLT S64 = LLT::scalar(64);
  const LLT V2S64 = LLT::vector(2, 64);
  const LLT P1 = LLT::pointer(1, 64);
  const LLT P2 = LLT::pointer(2, 64);

  setAction({G_ADD, S32}, Legal);
  setAction({G_ADD, S64}, Legal);
  
  setAction({G_AND, S1}, Legal);
  setAction({G_AND, S32}, Legal);
  setAction({G_AND, S64}, NarrowScalar);
  
  setAction({G_ASHR, S64}, Legal);
  setAction({G_ASHR, 1, S64}, Legal);
  setAction({G_ASHR, 2, S64}, Legal);

  setAction({G_BITCAST, V2S16}, Legal);
  setAction({G_BITCAST, 1, S32}, Legal);

  setAction({G_BITCAST, S32}, Legal);
  setAction({G_BITCAST, 1, V2S16}, Legal);

  setAction({G_BITCAST, V2S64}, Legal);
  setAction({G_BITCAST, 1, V4S32}, Legal);

  setAction({G_BITCAST, V2S32}, Legal);
  setAction({G_BITCAST, 1, S64}, Legal);

  setAction({G_BRCOND, S1}, Legal);

  // FIXME: i1 operands to intrinsics should always be legal, but other i1
  // values may not be legal.  We need to figure out how to distinguish
  // between these two scenarios.
  setAction({G_CONSTANT, S1}, Legal);
  setAction({G_CONSTANT, S16}, Legal);
  setAction({G_CONSTANT, S32}, Legal);
  setAction({G_CONSTANT, S64}, Legal);

  setAction({G_EXTRACT_VECTOR_ELT, S32}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, S64}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 1, V4S32}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 1, V8S32}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 1, V3S32}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 1, V2S32}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 1, V2S64}, Legal);
  setAction({G_EXTRACT_VECTOR_ELT, 2, S32}, Legal);

  setAction({G_FCONSTANT, S32}, Legal);

  setAction({G_FADD, S32}, Legal);

  setAction({G_FCMP, S1}, Legal);
  setAction({G_FCMP, 1, S32}, Legal);

  setAction({G_FMUL, S32}, Legal);
  
  setAction({G_FPTOSI, S32}, Legal);
  setAction({G_FPTOSI, 1, S32}, Legal);

  setAction({G_FPTOUI, S32}, Legal);
  setAction({G_FPTOUI, 1, S32}, Legal);

  setAction({G_GEP, P1}, Legal);
  setAction({G_GEP, P2}, Legal);
  setAction({G_GEP, 1, S64}, Legal);

  //setDefaultAction(G_INTRINSIC_W_SIDE_EFFECTS, Custom);
  //setDefaultAction(G_INTRINSIC, Custom);

  // FIXME: We probably want custom for all intrinsics.
  //setAction({G_INTRINSIC_W_SIDE_EFFECTS, S32}, Custom);
  //setAction({G_INTRINSIC, S32}, Custom);
  //
  setAction({G_ICMP, S1}, Legal);
  setAction({G_ICMP, 1, S32}, Legal);

  setAction({G_IMPLICIT_DEF, S32}, Legal);
  setAction({G_IMPLICIT_DEF, V2S32}, Legal);
  setAction({G_IMPLICIT_DEF, V4S32}, Legal);

  setAction({G_INSERT_VECTOR_ELT, V2S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, 1, V4S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, V4S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, 1, V2S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, 1, S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, 2, S32}, Legal);
  setAction({G_INSERT_VECTOR_ELT, 3, S32}, Legal);

  setAction({G_LOAD, P1}, Legal);
  setAction({G_LOAD, P2}, Legal);
  setAction({G_LOAD, S32}, Legal);
  setAction({G_LOAD, V4S32}, Legal);
  setAction({G_LOAD, V8S32}, Legal);
  setAction({G_LOAD, 1, P1}, Legal);
  setAction({G_LOAD, 1, P2}, Legal);

  setAction({G_MERGE_VALUES, S32}, Custom);
  setAction({G_MERGE_VALUES, 1, S16}, Custom);

  setAction({G_UNMERGE_VALUES, S32}, Custom);
  setAction({G_UNMERGE_VALUES, 1, S32}, Custom);
  setAction({G_UNMERGE_VALUES, 1, S64}, Custom);
  setAction({G_UNMERGE_VALUES, 2, S64}, Custom);

  setAction({G_MUL, S32}, Legal);

  setAction({G_OR, S32}, Legal);

  setAction({G_SELECT, S32}, Legal);
  setAction({G_SELECT, S64}, NarrowScalar);
  setAction({G_SELECT, 1, S1}, Legal);

  setAction({G_SHL, S32}, Legal);

  setAction({G_SHUFFLE_VECTOR, 0, V4S32}, Lower);

  setAction({G_STORE, S32}, Legal);
  setAction({G_STORE, 1, P1}, Legal);

  setAction({G_SUB, S64}, Custom);

  setAction({G_TRUNC, S32}, Custom);
  setAction({G_TRUNC, 1, S64}, Custom);

  // FIXME: When RegBankSelect inserts copies, it will only create new
  // registers with scalar types.  This means we can end up with
  // G_LOAD/G_STORE/G_GEP instruction with scalar types for their pointer
  // operands.  In assert builds, the instruction selector will assert
  // if it sees a generic instruction which isn't legal, so we need to
  // tell it that scalar types are legal for pointer operands
  setAction({G_GEP, S64}, Legal);
  setAction({G_LOAD, 1, S64}, Legal);
  setAction({G_STORE, 1, S64}, Legal);

  setAction({G_XOR, S1}, Legal);
  setAction({G_XOR, S32}, Legal);
  setAction({G_XOR, S64}, NarrowScalar);

  setAction({G_ZEXT, S32}, Legal);
  setAction({G_ZEXT, 1, S1}, Legal);
  computeTables();
}

bool AMDGPULegalizerInfo::legalizeG_MERGE_VALUES(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           MachineIRBuilder &MIRBuilder) const {
  unsigned DstReg = MI.getOperand(0).getReg();
  unsigned Src0 = MI.getOperand(1).getReg();
  unsigned Src1 = MI.getOperand(2).getReg();

  LLT DstTy = MRI.getType(DstReg);
  LLT SrcTy = MRI.getType(Src0);

  if (!DstTy.isScalar() || DstTy.getSizeInBits() != 32)
    return false;

  if (SrcTy != DstTy.halfScalarSize())
    return false;

  MIRBuilder.setInstr(MI);

  unsigned Src0Ext = MRI.createGenericVirtualRegister(DstTy);
  unsigned Src1Ext = MRI.createGenericVirtualRegister(DstTy);
  unsigned Src1Shl = MRI.createGenericVirtualRegister(DstTy);
  unsigned C16 = MRI.createGenericVirtualRegister(DstTy);
  MIRBuilder.buildZExt(Src0Ext, Src0);
  MIRBuilder.buildZExt(Src1Ext, Src1);
  MIRBuilder.buildConstant(C16, SrcTy.getSizeInBits());
  MIRBuilder.buildInstr(TargetOpcode::G_SHL).addDef(Src1Shl)
                                            .addReg(Src1Ext)
                                            .addReg(C16);
  MIRBuilder.buildInstr(TargetOpcode::G_OR).addDef(DstReg)
                                            .addReg(Src0Ext)
                                            .addReg(Src1Shl);
  MI.eraseFromParent();
  return true;
}

bool AMDGPULegalizerInfo::legalizeG_SUB(MachineInstr &MI,
                                        MachineRegisterInfo &MRI,
                                        MachineIRBuilder &MIRBuilder) const {
  unsigned Src1 = MI.getOperand(2).getReg();
  unsigned NegOne = MRI.createGenericVirtualRegister(LLT::scalar(64));
  unsigned NotReg = MRI.createGenericVirtualRegister(LLT::scalar(64));
  unsigned One = MRI.createGenericVirtualRegister(LLT::scalar(64));
  unsigned AddReg = MRI.createGenericVirtualRegister(LLT::scalar(64));

  MIRBuilder.setInstr(MI);
  MIRBuilder.buildConstant(NegOne, -1);
  MIRBuilder.buildConstant(One, 1);
  MIRBuilder.buildInstr(TargetOpcode::G_XOR).addDef(NotReg)
                                            .addReg(Src1)
                                            .addReg(NegOne);
  MIRBuilder.buildAdd(AddReg, NotReg, One);
  MIRBuilder.buildAdd(MI.getOperand(0).getReg(), MI.getOperand(1).getReg(),
                      AddReg);
  MI.eraseFromParent();
  return true;
}

bool AMDGPULegalizerInfo::legalizeG_TRUNC(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          MachineIRBuilder &MIRBuilder) const {
  unsigned DstReg = MI.getOperand(0).getReg();
  unsigned SrcReg = MI.getOperand(1).getReg();

  LLT DstTy = MRI.getType(DstReg);
  LLT SrcTy = MRI.getType(SrcReg);

  if (SrcTy.getSizeInBits() % DstTy.getSizeInBits())
    return false;

  LLT VecTy = LLT::vector(SrcTy.getSizeInBits() / DstTy.getSizeInBits(),
                          DstTy.getSizeInBits());

  dbgs () << "Dst = " << DstTy << ", " << VecTy << '\n';
  unsigned VecReg = MRI.createGenericVirtualRegister(VecTy);
  unsigned ZeroReg = MRI.createGenericVirtualRegister(LLT::scalar(32));
  MIRBuilder.setInstr(MI);
  MIRBuilder.buildCast(VecReg, SrcReg);
  MIRBuilder.buildConstant(ZeroReg, 0);
  MIRBuilder.buildExtractVectorElement(DstReg, VecReg, ZeroReg);
  MI.eraseFromParent();
  return true;
}

bool AMDGPULegalizerInfo::legalizeCustom(MachineInstr &MI,
                                         MachineRegisterInfo &MRI,
                                         MachineIRBuilder &MIRBuilder) const {
  switch (MI.getOpcode()) {
  default:
    return false;
  case TargetOpcode::G_MERGE_VALUES:
    return legalizeG_MERGE_VALUES(MI, MRI, MIRBuilder);
  case TargetOpcode::G_SUB:
    return legalizeG_SUB(MI, MRI, MIRBuilder);
  case TargetOpcode::G_TRUNC:
    return legalizeG_TRUNC(MI, MRI, MIRBuilder);
  }

}
