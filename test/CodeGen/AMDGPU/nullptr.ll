;RUN: llc < %s -march=amdgcn -mtriple=amdgcn-- -verify-machineinstrs | FileCheck -check-prefixes=CHECK,GCN %s

%struct.S = type { i32 addrspace(5)*, i32 addrspace(1)*, i32 addrspace(4)*, i32 addrspace(3)*, i32*, i32 addrspace(2)*}

; CHECK-LABEL: nullptr_priv:
; CHECK-NEXT: .long 0
@nullptr_priv = global i32 addrspace(5)* addrspacecast (i32* null to i32 addrspace(5)*)

; CHECK-LABEL: nullptr_glob:
; GCN-NEXT: .quad 0
; R600-NEXT: .long 0
@nullptr_glob = global i32 addrspace(1)* addrspacecast (i32* null to i32 addrspace(1)*)

; CHECK-LABEL: nullptr_const:
; GCN-NEXT: .quad 0
; R600-NEXT: .long 0
@nullptr_const = global i32 addrspace(4)* addrspacecast (i32* null to i32 addrspace(4)*)

; CHECK-LABEL: nullptr_local:
; CHECK-NEXT: .long -1
@nullptr_local = global i32 addrspace(3)* addrspacecast (i32* null to i32 addrspace(3)*)

; CHECK-LABEL: nullptr_region:
; CHECK-NEXT: .long -1
@nullptr_region = global i32 addrspace(2)* addrspacecast (i32* null to i32 addrspace(2)*)

; CHECK-LABEL: structWithPointers:
; CHECK-NEXT: .long 0
; GCN-NEXT:   .zero 4
; GCN-NEXT:   .quad 0
; R600-NEXT:  .long 0
; GCN-NEXT:   .quad 0
; R600-NEXT:  .long 0
; CHECK-NEXT: .long -1
; GCN-NEXT:   .zero 4
; GCN-NEXT:   .quad 0
; R600-NEXT:  .long 0
; CHECK-NEXT: .long -1
; GCN-NEXT:   .zero 4
@structWithPointers = addrspace(1) global %struct.S {
  i32 addrspace(5)* addrspacecast (i32* null to i32 addrspace(5)*),
  i32 addrspace(1)* addrspacecast (i32* null to i32 addrspace(1)*),
  i32 addrspace(4)* addrspacecast (i32* null to i32 addrspace(4)*),
  i32 addrspace(3)* addrspacecast (i32* null to i32 addrspace(3)*),
  i32* null,
  i32 addrspace(2)* addrspacecast (i32* null to i32 addrspace(2)*)}, align 4
