//===- AMDGPUInstructionSelector.cpp ----------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for
/// AMDGPU.
/// \todo This should be generated by TableGen.
//===----------------------------------------------------------------------===//

#include "AMDGPUInstructionSelector.h"
#include "AMDGPUInstrInfo.h"
#include "AMDGPURegisterBankInfo.h"
#include "AMDGPURegisterInfo.h"
#include "AMDGPUSubtarget.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "amdgpu-isel"

using namespace llvm;

AMDGPUInstructionSelector::AMDGPUInstructionSelector(
    const SISubtarget &STI, const AMDGPURegisterBankInfo &RBI)
    : InstructionSelector(), TII(*STI.getInstrInfo()),
      TRI(*STI.getRegisterInfo()), RBI(RBI), AMDGPUASI(STI.getAMDGPUAS()) {}

bool AMDGPUInstructionSelector::selectCOPY(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  I.setDesc(TII.get(TargetOpcode::COPY));
  unsigned Size = RBI.getSizeInBits(I.getOperand(0).getReg(), MRI, TRI);
  for (const MachineOperand &MO : I.operands()) {
    if (TargetRegisterInfo::isPhysicalRegister(MO.getReg()))
      continue;
    const RegisterBank *OpBank = RBI.getRegBank(MO.getReg(), MRI, TRI);
    if (!OpBank)
      continue;

    const TargetRegisterClass *RC;
    if (OpBank->getID() == AMDGPU::SGPRRegBankID) {
      RC = Size == 32 ? &AMDGPU::SGPR_32RegClass : &AMDGPU::SReg_64RegClass;
    } else {
      assert(OpBank->getID() == AMDGPU::VGPRRegBankID);
      RC = Size == 32 ? &AMDGPU::VGPR_32RegClass : &AMDGPU::VReg_64RegClass;
    }
    RBI.constrainGenericRegister(MO.getReg(), *RC, MRI);
  }
  return true;
}

MachineOperand
AMDGPUInstructionSelector::getSubOperand64(MachineOperand &MO,
                                           unsigned SubIdx) const {

  MachineInstr *MI = MO.getParent();
  MachineBasicBlock *BB = MO.getParent()->getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  unsigned DstReg = MRI.createVirtualRegister(&AMDGPU::SGPR_32RegClass);

  if (MO.isReg()) {
    unsigned ComposedSubIdx = TRI.composeSubRegIndices(MO.getSubReg(), SubIdx);
    unsigned Reg = MO.getReg();
    BuildMI(*BB, MI, MI->getDebugLoc(), TII.get(AMDGPU::COPY), DstReg)
            .addReg(Reg, 0, ComposedSubIdx);

    return MachineOperand::CreateReg(DstReg, MO.isDef(), MO.isImplicit(),
                                     MO.isKill(), MO.isDead(), MO.isUndef(),
                                     MO.isEarlyClobber(), 0, MO.isDebug(),
                                     MO.isInternalRead());
  }

  assert(MO.isImm());

  APInt Imm(64, MO.getImm());

  switch (SubIdx) {
  default:
    llvm_unreachable("do not know to split immediate with this sub index.");
  case AMDGPU::sub0:
    return MachineOperand::CreateImm(Imm.getLoBits(32).getSExtValue());
  case AMDGPU::sub1:
    return MachineOperand::CreateImm(Imm.getHiBits(32).getSExtValue());
  }
}

bool AMDGPUInstructionSelector::selectG_ADD(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  unsigned Size = RBI.getSizeInBits(I.getOperand(0).getReg(), MRI, TRI);
  unsigned DstLo = MRI.createVirtualRegister(&AMDGPU::SReg_32RegClass);
  unsigned DstHi = MRI.createVirtualRegister(&AMDGPU::SReg_32RegClass);

  if (Size != 64)
    return false;

  DebugLoc DL = I.getDebugLoc();

  MachineOperand Lo1(getSubOperand64(I.getOperand(1), AMDGPU::sub0));
  MachineOperand Lo2(getSubOperand64(I.getOperand(2), AMDGPU::sub0));

  BuildMI(*BB, &I, DL, TII.get(AMDGPU::S_ADD_U32), DstLo)
          .add(Lo1)
          .add(Lo2);

  MachineOperand Hi1(getSubOperand64(I.getOperand(1), AMDGPU::sub1));
  MachineOperand Hi2(getSubOperand64(I.getOperand(2), AMDGPU::sub1));

  BuildMI(*BB, &I, DL, TII.get(AMDGPU::S_ADDC_U32), DstHi)
          .add(Hi1)
          .add(Hi2);

  BuildMI(*BB, &I, DL, TII.get(AMDGPU::REG_SEQUENCE), I.getOperand(0).getReg())
          .addReg(DstLo)
          .addImm(AMDGPU::sub0)
          .addReg(DstHi)
          .addImm(AMDGPU::sub1);

  for (MachineOperand &MO : I.explicit_operands()) {
    if (!MO.isReg() || TargetRegisterInfo::isPhysicalRegister(MO.getReg()))
      continue;
    RBI.constrainGenericRegister(MO.getReg(), AMDGPU::SReg_64RegClass, MRI);
  }

  I.eraseFromParent();
  return true;
}

bool AMDGPUInstructionSelector::selectG_GEP(MachineInstr &I) const {
  return selectG_ADD(I);
}

static int64_t getConstant(const MachineInstr *MI) {
  return MI->getOperand(1).getCImm()->getSExtValue();
}

static unsigned getV_CMPOpcode(CmpInst::Predicate P, unsigned Size) {
  assert(Size == 32 || Size == 64);
  switch (P) {
  default:
    llvm_unreachable("Unknown condition code!");
  case CmpInst::ICMP_NE:
    return Size == 32 ? AMDGPU::V_CMP_NE_U32_e64 : AMDGPU::V_CMP_NE_U64_e64;
  case CmpInst::ICMP_EQ:
    return Size == 32 ? AMDGPU::V_CMP_EQ_U32_e64 : AMDGPU::V_CMP_EQ_U64_e64;
  case CmpInst::ICMP_SGT:
    return Size == 32 ? AMDGPU::V_CMP_GT_I32_e64 : AMDGPU::V_CMP_GT_I64_e64;
  case CmpInst::ICMP_SGE:
    return Size == 32 ? AMDGPU::V_CMP_GE_I32_e64 : AMDGPU::V_CMP_GE_I64_e64;
  case CmpInst::ICMP_SLT:
    return Size == 32 ? AMDGPU::V_CMP_LT_I32_e64 : AMDGPU::V_CMP_LT_I64_e64;
  case CmpInst::ICMP_SLE:
    return Size == 32 ? AMDGPU::V_CMP_LE_I32_e64 : AMDGPU::V_CMP_LE_I64_e64;
  case CmpInst::ICMP_UGT:
    return Size == 32 ? AMDGPU::V_CMP_GT_U32_e64 : AMDGPU::V_CMP_GT_U64_e64;
  case CmpInst::ICMP_UGE:
    return Size == 32 ? AMDGPU::V_CMP_GE_U32_e64 : AMDGPU::V_CMP_GE_U64_e64;
  case CmpInst::ICMP_ULT:
    return Size == 32 ? AMDGPU::V_CMP_LT_U32_e64 : AMDGPU::V_CMP_LT_U64_e64;
  case CmpInst::ICMP_ULE:
    return Size == 32 ? AMDGPU::V_CMP_LE_U32_e64 : AMDGPU::V_CMP_LE_U64_e64;
  }
}

static unsigned getS_CMPOpcode(CmpInst::Predicate P, unsigned Size) {
  // FIXME: VI supports 64-bit comparse.
  assert(Size == 32);
  switch (P) {
  default:
    llvm_unreachable("Unknown condition code!");
  case CmpInst::ICMP_NE:
    return AMDGPU::S_CMP_LG_U32;
  case CmpInst::ICMP_EQ:
    return AMDGPU::S_CMP_EQ_U32;
  case CmpInst::ICMP_SGT:
    return AMDGPU::S_CMP_GT_I32;
  case CmpInst::ICMP_SGE:
    return AMDGPU::S_CMP_GE_I32;
  case CmpInst::ICMP_SLT:
    return AMDGPU::S_CMP_LT_I32;
  case CmpInst::ICMP_SLE:
    return AMDGPU::S_CMP_LE_I32;
  case CmpInst::ICMP_UGT:
    return AMDGPU::S_CMP_GT_U32;
  case CmpInst::ICMP_UGE:
    return AMDGPU::S_CMP_GE_U32;
  case CmpInst::ICMP_ULT:
    return AMDGPU::S_CMP_LT_U32;
  case CmpInst::ICMP_ULE:
    return AMDGPU::S_CMP_LE_U32;
  }
}

bool AMDGPUInstructionSelector::selectG_ICMP(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  DebugLoc DL = I.getDebugLoc();

  unsigned SrcReg = I.getOperand(2).getReg();
  unsigned Size = RBI.getSizeInBits(SrcReg, MRI, TRI);
  // FIXME: VI supports 64-bit compares.
  assert(Size == 32);
  if (hasOnlySGPROperands(I, MRI)) {
    unsigned Opcode = getS_CMPOpcode((CmpInst::Predicate)I.getOperand(1).getPredicate(), Size);
    MachineInstr *ICmp = BuildMI(*BB, &I, DL, TII.get(Opcode))
            .add(I.getOperand(2))
            .add(I.getOperand(3));
    MachineInstr *CSelect = BuildMI(*BB, &I, DL, TII.get(AMDGPU::S_CSELECT_B64))
            .add(I.getOperand(0))
            .addImm(-1)
            .addImm(0);
    bool Ret = constrainSelectedInstRegOperands(*ICmp, TII, TRI, RBI) |
               constrainSelectedInstRegOperands(*CSelect, TII, TRI, RBI);
    I.eraseFromParent();
    return Ret;
  }

  assert(Size == 32 || Size == 64);
  unsigned Opcode = getV_CMPOpcode((CmpInst::Predicate)I.getOperand(1).getPredicate(), Size);
  MachineInstr *ICmp = BuildMI(*BB, &I, DL, TII.get(Opcode),
            I.getOperand(0).getReg())
            .add(I.getOperand(2))
            .add(I.getOperand(3));
  RBI.constrainGenericRegister(ICmp->getOperand(0).getReg(),
                               AMDGPU::SReg_64RegClass, MRI);
  bool Ret = constrainSelectedInstRegOperands(*ICmp, TII, TRI, RBI);
  I.eraseFromParent();
  return Ret;
}


static MachineInstr *
buildEXP(const TargetInstrInfo &TII, MachineInstr *Insert, unsigned Tgt,
         unsigned Reg0, unsigned Reg1, unsigned Reg2, unsigned Reg3,
         unsigned VM, bool Compr, unsigned Enabled, bool Done) {
  DebugLoc DL = Insert->getDebugLoc();
  MachineBasicBlock &BB = *Insert->getParent();
  unsigned Opcode = Done ? AMDGPU::EXP_DONE : AMDGPU::EXP;
  return BuildMI(BB, Insert, DL, TII.get(Opcode))
          .addImm(Tgt)
          .addReg(Reg0)
          .addReg(Reg1)
          .addReg(Reg2)
          .addReg(Reg3)
          .addImm(VM)
          .addImm(Compr)
          .addImm(Enabled);
}

bool AMDGPUInstructionSelector::selectG_INTRINSIC_W_SIDE_EFFECTS(
                                                 MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  DebugLoc DL = I.getDebugLoc();

  unsigned IntrinsicID = I.getOperand(0).getIntrinsicID();
  switch (IntrinsicID) {
  case Intrinsic::amdgcn_exp: {
    int64_t Tgt = getConstant(MRI.getVRegDef(I.getOperand(1).getReg()));
    int64_t Enabled = getConstant(MRI.getVRegDef(I.getOperand(2).getReg()));
    int64_t Done = getConstant(MRI.getVRegDef(I.getOperand(7).getReg()));
    int64_t VM = getConstant(MRI.getVRegDef(I.getOperand(8).getReg()));
   
    MachineInstr *Exp = buildEXP(TII, &I, Tgt, I.getOperand(3).getReg(),
                                 I.getOperand(4).getReg(),
                                 I.getOperand(5).getReg(),
                                 I.getOperand(6).getReg(),
                                 VM, false, Enabled, Done);
    
    // operands to use appropriate classes.
    bool Ret = constrainSelectedInstRegOperands(*Exp, TII, TRI, RBI);

    I.eraseFromParent();
    return Ret;
  }
  case Intrinsic::amdgcn_exp_compr: {
    int64_t Tgt = getConstant(MRI.getVRegDef(I.getOperand(1).getReg()));
    int64_t Enabled = getConstant(MRI.getVRegDef(I.getOperand(2).getReg()));
    unsigned Reg0 = I.getOperand(3).getReg();
    unsigned Reg1 = I.getOperand(4).getReg();
    unsigned Undef = MRI.createVirtualRegister(&AMDGPU::VGPR_32RegClass);
    int64_t Done = getConstant(MRI.getVRegDef(I.getOperand(5).getReg()));
    int64_t VM = getConstant(MRI.getVRegDef(I.getOperand(6).getReg()));

    BuildMI(*BB, &I, DL, TII.get(AMDGPU::IMPLICIT_DEF), Undef);
    MachineInstr *Exp = buildEXP(TII, &I, Tgt, Reg0, Reg1, Undef, Undef, VM,
                                 true,  Enabled, Done);
    bool Ret = constrainSelectedInstRegOperands(*Exp, TII, TRI, RBI);

    I.eraseFromParent();
    return Ret;
  }
  }
  return false;
}

bool AMDGPUInstructionSelector::hasOnlySGPROperands(const MachineInstr &I,
                                         const MachineRegisterInfo &MRI) const {
  for (const MachineOperand &MO : I.operands()) {
    if (!MO.isReg())
      continue;
    const RegisterBank *OpBank = RBI.getRegBank(MO.getReg(), MRI, TRI);
    if (OpBank->getID() != AMDGPU::SGPRRegBankID)
      return false;
  }
  return true;
}

bool AMDGPUInstructionSelector::selectG_SELECT(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  DebugLoc DL = I.getDebugLoc();

  unsigned DstReg = I.getOperand(0).getReg();
  unsigned Size = RBI.getSizeInBits(DstReg, MRI, TRI);
  assert(Size == 32 || Size == 64);
  if (hasOnlySGPROperands(I, MRI)) {
    unsigned SelectOpcode = Size == 32 ? AMDGPU::S_CSELECT_B32 :
                                         AMDGPU::S_CSELECT_B64;
    // FIXME: Not sure how best to handle SCC.  Since CondReg is signextend
    // from i1 checking the low 32-bits should be fine.
    MachineOperand CondRegLo(getSubOperand64(I.getOperand(1), AMDGPU::sub0));
    MachineInstr *SetSCC = BuildMI(*BB, &I, DL, TII.get(AMDGPU::S_CMP_LG_U32))
            .add(CondRegLo)
            .addImm(0);
    MachineInstr *Select = BuildMI(*BB, &I, DL, TII.get(SelectOpcode), DstReg)
            .add(I.getOperand(2))
            .add(I.getOperand(3));

    bool Ret = constrainSelectedInstRegOperands(*Select, TII, TRI, RBI) |
               constrainSelectedInstRegOperands(*SetSCC, TII, TRI, RBI);
    I.eraseFromParent();
    return Ret;
  }

  assert(Size == 32);
  // FIXME: Support 64-bit compares
  MachineInstr *Select =
      BuildMI(*BB, &I, DL, TII.get(AMDGPU::V_CNDMASK_B32_e64), DstReg)
              .add(I.getOperand(3))
              .add(I.getOperand(2))
              .add(I.getOperand(1));

  bool Ret = constrainSelectedInstRegOperands(*Select, TII, TRI, RBI);
  I.eraseFromParent();
  return Ret;
}

bool AMDGPUInstructionSelector::selectG_STORE(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  DebugLoc DL = I.getDebugLoc();

  // FIXME: Select store instruction based on address space
  MachineInstr *Flat = BuildMI(*BB, &I, DL, TII.get(AMDGPU::FLAT_STORE_DWORD))
          .add(I.getOperand(1))
          .add(I.getOperand(0))
          .addImm(0)  // offset
          .addImm(0)  // glc
          .addImm(0); // slc


  // Now that we selected an opcode, we need to constrain the register
  // operands to use appropriate classes.
  bool Ret = constrainSelectedInstRegOperands(*Flat, TII, TRI, RBI);

  I.eraseFromParent();
  return Ret;
}

bool AMDGPUInstructionSelector::selectG_CONSTANT(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  unsigned DstReg = I.getOperand(0).getReg();
  unsigned Size = RBI.getSizeInBits(DstReg, MRI, TRI);
  const MachineOperand &COp = I.getOperand(1);
  const APInt &Imm = COp.isFPImm() ?
      COp.getFPImm()->getValueAPF().bitcastToAPInt() :
      COp.getCImm()->getValue();
  const RegisterBank *RegBank = RBI.getRegBank(DstReg, MRI, TRI);
  unsigned Opcode = RegBank->getID() == AMDGPU::SGPRRegBankID ?
      AMDGPU::S_MOV_B32 : AMDGPU::V_MOV_B32_e32;

  if (Size == 32) {
    I.setDesc(TII.get(Opcode));
    I.getOperand(1).ChangeToImmediate(Imm.getSExtValue());
    return constrainSelectedInstRegOperands(I, TII, TRI, RBI);
  }

  assert(Size == 64);

  DebugLoc DL = I.getDebugLoc();
  unsigned LoReg = MRI.createVirtualRegister(&AMDGPU::SReg_32RegClass);
  unsigned HiReg = MRI.createVirtualRegister(&AMDGPU::SReg_32RegClass);

  BuildMI(*BB, &I, DL, TII.get(Opcode), LoReg)
          .addImm(Imm.trunc(32).getZExtValue());

  BuildMI(*BB, &I, DL, TII.get(Opcode), HiReg)
          .addImm(Imm.ashr(32).getZExtValue());

  BuildMI(*BB, &I, DL, TII.get(AMDGPU::REG_SEQUENCE), DstReg)
          .addReg(LoReg)
          .addImm(AMDGPU::sub0)
          .addReg(HiReg)
          .addImm(AMDGPU::sub1);
  // We can't call constrainSelectedInstRegOperands here, because it doesn't
  // work for target independent opcodes
  I.eraseFromParent();
  return RBI.constrainGenericRegister(DstReg, AMDGPU::SReg_64RegClass, MRI);
}

static bool isConstant(const MachineInstr &MI) {
  return MI.getOpcode() == TargetOpcode::G_CONSTANT;
}

void AMDGPUInstructionSelector::getAddrModeInfo(const MachineInstr &Load,
    const MachineRegisterInfo &MRI, SmallVectorImpl<GEPInfo> &AddrInfo) const {

  const MachineInstr *PtrMI = MRI.getUniqueVRegDef(Load.getOperand(1).getReg());

  assert(PtrMI);

  if (PtrMI->getOpcode() != TargetOpcode::G_GEP)
    return;

  GEPInfo GEPInfo(*PtrMI);

  for (unsigned i = 1, e = 3; i < e; ++i) {
    const MachineOperand &GEPOp = PtrMI->getOperand(i);
    const MachineInstr *OpDef = MRI.getUniqueVRegDef(GEPOp.getReg());
    assert(OpDef);
    if (isConstant(*OpDef)) {
      // FIXME: Is it possible to have multiple Imm parts?  Maybe if we
      // are lacking other optimizations.
      assert(GEPInfo.Imm == 0);
      GEPInfo.Imm = OpDef->getOperand(1).getCImm()->getSExtValue();
      continue;
    }
    const RegisterBank *OpBank = RBI.getRegBank(GEPOp.getReg(), MRI, TRI);
    if (OpBank->getID() == AMDGPU::SGPRRegBankID)
      GEPInfo.SgprParts.push_back(GEPOp.getReg());
    else
      GEPInfo.VgprParts.push_back(GEPOp.getReg());
  }

  AddrInfo.push_back(GEPInfo);
  getAddrModeInfo(*PtrMI, MRI, AddrInfo);
}

static bool isInstrUniform(const MachineInstr &MI) {
  if (!MI.hasOneMemOperand())
    return false;

  const MachineMemOperand *MMO = *MI.memoperands_begin();
  const Value *Ptr = MMO->getValue();

  // UndefValue means this is a load of a kernel input.  These are uniform.
  // Sometimes LDS instructions have constant pointers.
  // If Ptr is null, then that means this mem operand contains a
  // PseudoSourceValue like GOT.
  if (!Ptr || isa<UndefValue>(Ptr) || isa<Argument>(Ptr) ||
      isa<Constant>(Ptr) || isa<GlobalValue>(Ptr))
    return true;

  const Instruction *I = dyn_cast<Instruction>(Ptr);
  return I && I->getMetadata("amdgpu.uniform");
}

static unsigned getSmrdOpcode(unsigned BaseOpcode, unsigned LoadSize) {

  if (LoadSize == 32)
    return BaseOpcode;

  switch (BaseOpcode) {
  case AMDGPU::S_LOAD_DWORD_IMM:
    switch (LoadSize) {
    case 64:
      return AMDGPU::S_LOAD_DWORDX2_IMM;
    case 128:
      return AMDGPU::S_LOAD_DWORDX4_IMM;
    case 256:
      return AMDGPU::S_LOAD_DWORDX8_IMM;
    case 512:
      return AMDGPU::S_LOAD_DWORDX16_IMM;
    }
    break;
  case AMDGPU::S_LOAD_DWORD_IMM_ci:
    switch (LoadSize) {
    case 64:
      return AMDGPU::S_LOAD_DWORDX2_IMM_ci;
    case 128:
      return AMDGPU::S_LOAD_DWORDX4_IMM_ci;
    case 256:
      return AMDGPU::S_LOAD_DWORDX8_IMM_ci;
    case 512:
      return AMDGPU::S_LOAD_DWORDX16_IMM_ci;
    }
    break;
  case AMDGPU::S_LOAD_DWORD_SGPR:
    switch (LoadSize) {
    case 64:
      return AMDGPU::S_LOAD_DWORDX2_SGPR;
    case 128:
      return AMDGPU::S_LOAD_DWORDX4_SGPR;
    case 256:
      return AMDGPU::S_LOAD_DWORDX8_SGPR;
    case 512:
      return AMDGPU::S_LOAD_DWORDX16_SGPR;
    }
    break;
  }
  llvm_unreachable("Invalid base smrd opcode or size");
}

bool AMDGPUInstructionSelector::hasVgprParts(ArrayRef<GEPInfo> AddrInfo) const {
  for (const GEPInfo &GEPInfo : AddrInfo) {
    if (!GEPInfo.VgprParts.empty())
      return true;
  }
  return false;
}

bool AMDGPUInstructionSelector::selectSMRD(MachineInstr &I,
                                           ArrayRef<GEPInfo> AddrInfo) const {

  if (!I.hasOneMemOperand())
    return false;

  if ((*I.memoperands_begin())->getAddrSpace() != AMDGPUASI.CONSTANT_ADDRESS)
    return false;

  if (!isInstrUniform(I))
    return false;

  if (hasVgprParts(AddrInfo))
    return false;

  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  const SISubtarget &Subtarget = MF->getSubtarget<SISubtarget>();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  unsigned DstReg = I.getOperand(0).getReg();
  const DebugLoc &DL = I.getDebugLoc();
  unsigned Opcode;
  unsigned LoadSize = RBI.getSizeInBits(DstReg, MRI, TRI);

  if (!AddrInfo.empty() && AddrInfo[0].SgprParts.size() == 1) {

    const GEPInfo &GEPInfo = AddrInfo[0];

    unsigned PtrReg = GEPInfo.SgprParts[0];
    int64_t EncodedImm = AMDGPU::getSMRDEncodedOffset(Subtarget, GEPInfo.Imm);
    if (AMDGPU::isLegalSMRDImmOffset(Subtarget, GEPInfo.Imm)) {
      Opcode = getSmrdOpcode(AMDGPU::S_LOAD_DWORD_IMM, LoadSize);

      MachineInstr *SMRD = BuildMI(*BB, &I, DL, TII.get(Opcode), DstReg)
                                 .addReg(PtrReg)
                                 .addImm(EncodedImm)
                                 .addImm(0); // glc
      return constrainSelectedInstRegOperands(*SMRD, TII, TRI, RBI);
    }

    if (Subtarget.getGeneration() == AMDGPUSubtarget::SEA_ISLANDS &&
        isUInt<32>(EncodedImm)) {
      Opcode = getSmrdOpcode(AMDGPU::S_LOAD_DWORD_IMM_ci, LoadSize);
      MachineInstr *SMRD = BuildMI(*BB, &I, DL, TII.get(Opcode), DstReg)
                                   .addReg(PtrReg)
                                   .addImm(EncodedImm)
                                   .addImm(0); // glc
      return constrainSelectedInstRegOperands(*SMRD, TII, TRI, RBI);
    }

    if (isUInt<32>(GEPInfo.Imm)) {
      Opcode = getSmrdOpcode(AMDGPU::S_LOAD_DWORD_SGPR, LoadSize);
      unsigned OffsetReg = MRI.createVirtualRegister(&AMDGPU::SReg_32RegClass);
      BuildMI(*BB, &I, DL, TII.get(AMDGPU::S_MOV_B32), OffsetReg)
              .addImm(GEPInfo.Imm);

      MachineInstr *SMRD = BuildMI(*BB, &I, DL, TII.get(Opcode), DstReg)
                                   .addReg(PtrReg)
                                   .addReg(OffsetReg)
                                   .addImm(0); // glc
      return constrainSelectedInstRegOperands(*SMRD, TII, TRI, RBI);
    }
  }

  unsigned PtrReg = I.getOperand(1).getReg();
  Opcode = getSmrdOpcode(AMDGPU::S_LOAD_DWORD_IMM, LoadSize);
  MachineInstr *SMRD = BuildMI(*BB, &I, DL, TII.get(Opcode), DstReg)
                               .addReg(PtrReg)
                               .addImm(0)
                               .addImm(0); // glc
  return constrainSelectedInstRegOperands(*SMRD, TII, TRI, RBI);
}


bool AMDGPUInstructionSelector::selectG_LOAD(MachineInstr &I) const {
  MachineBasicBlock *BB = I.getParent();
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &MRI = MF->getRegInfo();
  DebugLoc DL = I.getDebugLoc();
  unsigned DstReg = I.getOperand(0).getReg();
  unsigned PtrReg = I.getOperand(1).getReg();
  unsigned LoadSize = RBI.getSizeInBits(DstReg, MRI, TRI);
  unsigned Opcode;

  SmallVector<GEPInfo, 4> AddrInfo;

  getAddrModeInfo(I, MRI, AddrInfo);

  if (selectSMRD(I, AddrInfo)) {
    I.eraseFromParent();
    return true;
  }

  switch (LoadSize) {
  default:
    llvm_unreachable("Load size not supported\n");
  case 32:
    Opcode = AMDGPU::FLAT_LOAD_DWORD;
    break;
  case 64:
    Opcode = AMDGPU::FLAT_LOAD_DWORDX2;
    break;
  }

  MachineInstr *Flat = BuildMI(*BB, &I, DL, TII.get(Opcode))
                               .add(I.getOperand(0))
                               .addReg(PtrReg)
                               .addImm(0)  // offset
                               .addImm(0)  // glc
                               .addImm(0); // slc

  bool Ret = constrainSelectedInstRegOperands(*Flat, TII, TRI, RBI);
  I.eraseFromParent();
  return Ret;
}


bool AMDGPUInstructionSelector::select(MachineInstr &I) const {

  if (!isPreISelGenericOpcode(I.getOpcode())) {
    if (I.isCopy())
      return selectCOPY(I);
    return true;
  }

  switch (I.getOpcode()) {
  default:
    break;
  case TargetOpcode::G_ADD:
    return selectG_ADD(I);
  case TargetOpcode::G_BITCAST:
    return selectCOPY(I);
  case TargetOpcode::G_CONSTANT:
  case TargetOpcode::G_FCONSTANT:
    return selectG_CONSTANT(I);
  case TargetOpcode::G_GEP:
    return selectG_GEP(I);
  case TargetOpcode::G_ICMP:
    return selectG_ICMP(I);
  case TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS:
    return selectG_INTRINSIC_W_SIDE_EFFECTS(I);
  case TargetOpcode::G_LOAD:
    return selectG_LOAD(I);
  case TargetOpcode::G_SELECT:
    return selectG_SELECT(I);
  case TargetOpcode::G_STORE:
    return selectG_STORE(I);
  }
  return false;
}
