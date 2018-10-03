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

#include "AMDGPU.h"
#include "AMDGPULegalizerInfo.h"
#include "AMDGPUTargetMachine.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace LegalizeActions;
using namespace LegalityPredicates;

AMDGPULegalizerInfo::AMDGPULegalizerInfo(const GCNSubtarget &ST,
                                         const GCNTargetMachine &TM) {
  using namespace TargetOpcode;

  auto GetAddrSpacePtr = [&TM](unsigned AS) {
    return LLT::pointer(AS, TM.getPointerSizeInBits(AS));
  };

  const LLT S1 = LLT::scalar(1);
  const LLT V2S16 = LLT::vector(2, 16);
  const LLT V4S32 = LLT::vector(4, 32);

  const LLT S32 = LLT::scalar(32);
  const LLT S64 = LLT::scalar(64);
  const LLT S128 = LLT::scalar(128);
  const LLT S512 = LLT::scalar(512);

  const LLT GlobalPtr = GetAddrSpacePtr(AMDGPUAS::GLOBAL_ADDRESS);
  const LLT ConstantPtr = GetAddrSpacePtr(AMDGPUAS::CONSTANT_ADDRESS);
  const LLT LocalPtr = GetAddrSpacePtr(AMDGPUAS::LOCAL_ADDRESS);
  const LLT FlatPtr = GetAddrSpacePtr(AMDGPUAS::FLAT_ADDRESS);
  const LLT PrivatePtr = GetAddrSpacePtr(AMDGPUAS::PRIVATE_ADDRESS);
  const LLT P7 = GetAddrSpacePtr(7);

  const LLT AddrSpaces[] = {
    GlobalPtr,
    ConstantPtr,
    LocalPtr,
    FlatPtr,
    PrivatePtr
  };

  setAction({G_ADD, S32}, Legal);
  setAction({G_ASHR, S32}, Legal);
  setAction({G_SUB, S32}, Legal);
  setAction({G_MUL, S32}, Legal);
  setAction({G_AND, S32}, Legal);
  setAction({G_OR, S32}, Legal);
  setAction({G_XOR, S32}, Legal);

	getActionDefinitionsBuilder(G_ANYEXT)
      .legalIf([=](const LegalityQuery &Query) {
          const LLT &Ty0 = Query.Types[0];
          const LLT &Ty1 = Query.Types[1];
          return Ty0.getSizeInBits() % 32 == 0 &&
                 Ty1.getSizeInBits() % 32 == 0 &&
                 Ty0.getSizeInBits() <= 512 &&
                 Ty1.getSizeInBits() <= 512;
        });

  setAction({G_BITCAST, V2S16}, Legal);
  setAction({G_BITCAST, 1, S32}, Legal);

  setAction({G_BITCAST, S32}, Legal);
  setAction({G_BITCAST, 1, V2S16}, Legal);

  setAction({G_BITCAST, S128}, Legal);
  setAction({G_BITCAST, 1, V4S32}, Legal);

  setAction({G_BRCOND, S1}, Legal);

  getActionDefinitionsBuilder(G_FCONSTANT)
    .legalFor({S32, S64});

  // G_IMPLICIT_DEF is a no-op so we can make it legal for any value type that
  // can fit in a register.
  // FIXME: We need to legalize several more operations before we can add
  // a test case for size > 512.
  getActionDefinitionsBuilder(G_IMPLICIT_DEF)
    .legalIf([=](const LegalityQuery &Query) {
        return Query.Types[0].getSizeInBits() <= 512;
    })
     .widenScalarToNextPow2(0)
    .clampScalar(0, S1, S512);

  getActionDefinitionsBuilder(G_CONSTANT)
    .legalFor({S1, S32, S64});

  // FIXME: i1 operands to intrinsics should always be legal, but other i1
  // values may not be legal.  We need to figure out how to distinguish
  // between these two scenarios.
  setAction({G_CONSTANT, S1}, Legal);

  setAction({G_FADD, S32}, Legal);

  setAction({G_FCMP, S1}, Legal);
  setAction({G_FCMP, 1, S32}, Legal);
  setAction({G_FCMP, 1, S64}, Legal);

  setAction({G_FMUL, S32}, Legal);

  setAction({G_ZEXT, S64}, Legal);
  setAction({G_ZEXT, 1, S32}, Legal);

  setAction({G_FPTOSI, S32}, Legal);
  setAction({G_FPTOSI, 1, S32}, Legal);

  setAction({G_SITOFP, S32}, Legal);
  setAction({G_SITOFP, 1, S32}, Legal);

  setAction({G_FPTOUI, S32}, Legal);
  setAction({G_FPTOUI, 1, S32}, Legal);

  for (LLT PtrTy : AddrSpaces) {
    LLT IdxTy = LLT::scalar(PtrTy.getSizeInBits());
    setAction({G_GEP, PtrTy}, Legal);
    setAction({G_GEP, 1, IdxTy}, Legal);
  }

  setAction({G_GEP, P7}, Legal);
  setAction({G_GEP, 1, S32}, Legal);

  setAction({G_ICMP, S1}, Legal);
  setAction({G_ICMP, 1, S32}, Legal);

  getActionDefinitionsBuilder(G_INTTOPTR)
    .legalIf([](const LegalityQuery &Query) {
      return true;
    });
  setAction({G_PHI, S32}, Legal);

  setAction({G_PTRTOINT, S32}, Legal);
  setAction({G_PTRTOINT, 1, P7}, Legal);

//  setAction({G_PTRTOINT, S32}, Legal);
//  setAction({G_PTRTOINT, 1, P7}, Legal);

  getActionDefinitionsBuilder({G_LOAD, G_STORE})
    .legalIf([=, &ST](const LegalityQuery &Query) {
        const LLT &Ty0 = Query.Types[0];

        if (Query.Opcode == G_LOAD && Ty0.isPointer())
          return false;

        // TODO: Decompose private loads into 4-byte components.
        // TODO: Illegal flat loads on SI
        switch (Ty0.getSizeInBits()) {
        case 32:
        case 64:
        case 128:
          return true;

        case 96:
          // XXX hasLoadX3
          return (ST.getGeneration() >= AMDGPUSubtarget::SEA_ISLANDS);

        case 256:
        case 512:
          // TODO: constant loads
        default:
          return false;
        }
      })
    .customIf([](const LegalityQuery &Query) {
      // TODO: Tablegen can't handle loads/stores of pointer types so we
      // need to custom lower these.
      return Query.Opcode == G_LOAD && Query.Types[0].isPointer();
    });



  setAction({G_SELECT, S32}, Legal);
  setAction({G_SELECT, 1, S1}, Legal);

  setAction({G_SHL, S32}, Legal);


  // FIXME: When RegBankSelect inserts copies, it will only create new
  // registers with scalar types.  This means we can end up with
  // G_LOAD/G_STORE/G_GEP instruction with scalar types for their pointer
  // operands.  In assert builds, the instruction selector will assert
  // if it sees a generic instruction which isn't legal, so we need to
  // tell it that scalar types are legal for pointer operands
  setAction({G_GEP, S64}, Legal);

  for (unsigned Op : {G_EXTRACT_VECTOR_ELT, G_INSERT_VECTOR_ELT}) {
    getActionDefinitionsBuilder(Op)
      .legalIf([=](const LegalityQuery &Query) {
          const LLT &VecTy = Op == G_INSERT_VECTOR_ELT ? Query.Types[0] :
                                                        Query.Types[1];
          const LLT &IdxTy = Query.Types[2];
          return VecTy.getSizeInBits() % 32 == 0 &&
            VecTy.getSizeInBits() <= 512 &&
            IdxTy.getSizeInBits() == 32;
        });
  }

  // FIXME: Doesn't handle extract of illegal sizes.
  getActionDefinitionsBuilder({G_EXTRACT, G_INSERT})
    .legalIf([=](const LegalityQuery &Query) {
        const LLT &Ty0 = Query.Types[0];
        const LLT &Ty1 = Query.Types[1];
        return (Ty0.getSizeInBits() % 32 == 0) &&
               (Ty1.getSizeInBits() % 32 == 0);
      });

  // Merge/Unmerge
  for (unsigned Op : {G_MERGE_VALUES, G_UNMERGE_VALUES}) {
    unsigned BigTyIdx = Op == G_MERGE_VALUES ? 0 : 1;
    unsigned LitTyIdx = Op == G_MERGE_VALUES ? 1 : 0;

    getActionDefinitionsBuilder(Op)
      .legalIf([=](const LegalityQuery &Query) {
          const LLT &BigTy = Query.Types[BigTyIdx];
          const LLT &LitTy = Query.Types[LitTyIdx];
          return BigTy.getSizeInBits() % 32 == 0 &&
                 LitTy.getSizeInBits() % 32 == 0 &&
                 BigTy.getSizeInBits() <= 512;
        })
      // Any vectors left are the wrong size. Scalarize them.
      .fewerElementsIf([](const LegalityQuery &Query) { return true; },
                       [](const LegalityQuery &Query) {
                         return std::make_pair(
                           0, Query.Types[0].getElementType());
                       })
      .fewerElementsIf([](const LegalityQuery &Query) { return true; },
                       [](const LegalityQuery &Query) {
                         return std::make_pair(
                           1, Query.Types[1].getElementType());
                       });

  }

  computeTables();
  verify(*ST.getInstrInfo());
}

bool AMDGPULegalizerInfo::legalizeCustom(MachineInstr &MI,
                                         MachineRegisterInfo &MRI,
                                         MachineIRBuilder &MIRBuilder) const {
  MIRBuilder.setInstr(MI);
  MachineFunction &MF = *MI.getParent()->getParent();
  switch (MI.getOpcode()) {
  default:
    // No idea what to do.
    return false;
  case TargetOpcode::G_LOAD: {
    unsigned DstReg = MI.getOperand(0).getReg();
    LLT LoadTy = MRI.getType(DstReg);
    if (!LoadTy.isPointer())
      return false;
    unsigned LoadSize = LoadTy.getSizeInBits();
    assert(LoadSize % 32 == 0);

    LLT IntTy = LLT::scalar(LoadSize);
    unsigned IntReg = MRI.createGenericVirtualRegister(IntTy);
    MIRBuilder.buildInstr(TargetOpcode::G_LOAD, IntReg,
                          MI.getOperand(1).getReg())->cloneMemRefs(MF, MI);
    MIRBuilder.buildCast(DstReg, IntReg);
    MI.eraseFromParent();
    return true;
  }
  }
}

bool AMDGPULegalizerInfo::legalizeIntrinsic(MachineInstr &MI,
                                            MachineRegisterInfo &MRI,
                                            MachineIRBuilder &MIRBuilder) const {
  unsigned IntrinsicID = MI.getOpcode() == TargetOpcode::G_INTRINSIC ?
    MI.getOperand(1).getIntrinsicID() : MI.getOperand(0).getIntrinsicID();

  switch (IntrinsicID) {
  // All intrinsics are legal by default.
  default:
    return true;
  case AMDGPUIntrinsic::SI_load_const: {
    MachineFunction &MF = MIRBuilder.getMF();
    MIRBuilder.setInstr(MI);
    unsigned DstReg = MI.getOperand(0).getReg();
    unsigned RsrcReg = MI.getOperand(2).getReg();
    unsigned AddrReg = 0;
    LLT IntTy = LLT::scalar(128);
    unsigned IntReg = MRI.createGenericVirtualRegister(IntTy);
    MIRBuilder.buildCast(IntReg, RsrcReg);

    LLT PtrTy = LLT::pointer(AMDGPUAS::BUFFER_RSRC, 128);
    unsigned PtrReg = MRI.createGenericVirtualRegister(PtrTy);
    MIRBuilder.buildCast(PtrReg, IntReg);

    if (MI.getOperand(3).isReg()) {
      AddrReg = MRI.createGenericVirtualRegister(PtrTy);
      MIRBuilder.buildGEP(AddrReg, PtrReg, MI.getOperand(3).getReg());
    } else {
      unsigned AddrReg = 0;
      MIRBuilder.materializeGEP(AddrReg, PtrReg, LLT::scalar(32),
                                MI.getOperand(3).getImm());
    }

    MachineMemOperand *MMO = MF.getMachineMemOperand(
        MachinePointerInfo(),
        MachineMemOperand::MOLoad | MachineMemOperand::MODereferenceable |
            MachineMemOperand::MOInvariant,
        MRI.getType(DstReg).getSizeInBits() / 8, 4);

    MIRBuilder.buildLoad(DstReg, AddrReg, *MMO);
    MI.eraseFromParent();
    return true;
  }
  }
}
