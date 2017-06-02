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
  const LLT S32 = LLT::scalar(32);
  const LLT S64 = LLT::scalar(64);
  const LLT P1 = LLT::pointer(1, 64);
  const LLT P2 = LLT::pointer(2, 64);

  setAction({G_ADD, S32}, Legal);

  setAction({G_BITCAST, V2S16}, Legal);
  setAction({G_BITCAST, 1, S32}, Legal);
  
  setAction({G_BITCAST, S32}, Legal);
  setAction({G_BITCAST, 1, V2S16}, Legal);

  // FIXME: i1 operands to intrinsics should always be legal, but other i1
  // values may not be legal.  We need to figure out how to distinguish
  // between these two scenarios.
  setAction({G_CONSTANT, S1}, Legal);
  setAction({G_CONSTANT, S32}, Legal);
  setAction({G_CONSTANT, S64}, Legal);

  setAction({G_FCONSTANT, S32}, Legal);

  setAction({G_GEP, P1}, Legal);
  setAction({G_GEP, P2}, Legal);
  setAction({G_GEP, 1, S64}, Legal);

  setAction({G_ICMP, S1}, Legal);
  setAction({G_ICMP, 1, S32}, Legal);

  setAction({G_LOAD, P1}, Legal);
  setAction({G_LOAD, P2}, Legal);
  setAction({G_LOAD, S32}, Legal);
  setAction({G_LOAD, 1, P1}, Legal);
  setAction({G_LOAD, 1, P2}, Legal);

  setAction({G_SELECT, S32}, Legal);
  setAction({G_SELECT, 1, S1}, Legal);

  setAction({G_STORE, S32}, Legal);
  setAction({G_STORE, 1, P1}, Legal);

  // FIXME: When RegBankSelect inserts copies, it will only create new
  // registers with scalar types.  This means we can end up with
  // G_LOAD/G_STORE/G_GEP instruction with scalar types for their pointer
  // operands.  In assert builds, the instruction selector will assert
  // if it sees a generic instruction which isn't legal, so we need to
  // tell it that scalar types are legal for pointer operands
  setAction({G_GEP, S64}, Legal);
  setAction({G_LOAD, 1, S64}, Legal);
  setAction({G_STORE, 1, S64}, Legal);

  computeTables();
}
