;RUN: llc < %s -march=r600 -mtriple=r600-- -verify-machineinstrs | FileCheck -check-prefixes=CHECK,R600 %s

; Address space 7 for amdgcn is defined as 128-bits and the AsmPrinter crashes
; on pointers > 64 bits, so we need to put the r600 test for address space
; 7 in a separate file.  The other r600 only tests are moved here to avoid
; similar problems in the future.

; CHECK-LABEL: nullptr6:
; R600-NEXT: .long 0
@nullptr6 = global i32 addrspace(6)* addrspacecast (i32* null to i32 addrspace(6)*)

; CHECK-LABEL: nullptr7:
; R600-NEXT: .long 0
@nullptr7 = global i32 addrspace(7)* addrspacecast (i32* null to i32 addrspace(7)*)

; CHECK-LABEL: nullptr8:
; R600-NEXT: .long 0
@nullptr8 = global i32 addrspace(8)* addrspacecast (i32* null to i32 addrspace(8)*)

; CHECK-LABEL: nullptr9:
; R600-NEXT: .long 0
@nullptr9 = global i32 addrspace(9)* addrspacecast (i32* null to i32 addrspace(9)*)

; CHECK-LABEL: nullptr10:
; R600-NEXT: .long 0
@nullptr10 = global i32 addrspace(10)* addrspacecast (i32* null to i32 addrspace(10)*)

; CHECK-LABEL: nullptr11:
; R600-NEXT: .long 0
@nullptr11 = global i32 addrspace(11)* addrspacecast (i32* null to i32 addrspace(11)*)

; CHECK-LABEL: nullptr12:
; R600-NEXT: .long 0
@nullptr12 = global i32 addrspace(12)* addrspacecast (i32* null to i32 addrspace(12)*)

; CHECK-LABEL: nullptr13:
; R600-NEXT: .long 0
@nullptr13 = global i32 addrspace(13)* addrspacecast (i32* null to i32 addrspace(13)*)

; CHECK-LABEL: nullptr14:
; R600-NEXT: .long 0
@nullptr14 = global i32 addrspace(14)* addrspacecast (i32* null to i32 addrspace(14)*)

; CHECK-LABEL: nullptr15:
; R600-NEXT: .long 0
@nullptr15 = global i32 addrspace(15)* addrspacecast (i32* null to i32 addrspace(15)*)

; CHECK-LABEL: nullptr16:
; R600-NEXT: .long 0
@nullptr16 = global i32 addrspace(16)* addrspacecast (i32* null to i32 addrspace(16)*)

; CHECK-LABEL: nullptr17:
; R600-NEXT: .long 0
@nullptr17 = global i32 addrspace(17)* addrspacecast (i32* null to i32 addrspace(17)*)

; CHECK-LABEL: nullptr18:
; R600-NEXT: .long 0
@nullptr18 = global i32 addrspace(18)* addrspacecast (i32* null to i32 addrspace(18)*)

; CHECK-LABEL: nullptr19:
; R600-NEXT: .long 0
@nullptr19 = global i32 addrspace(19)* addrspacecast (i32* null to i32 addrspace(19)*)

; CHECK-LABEL: nullptr20:
; R600-NEXT: .long 0
@nullptr20 = global i32 addrspace(20)* addrspacecast (i32* null to i32 addrspace(20)*)

; CHECK-LABEL: nullptr21:
; R600-NEXT: .long 0
@nullptr21 = global i32 addrspace(21)* addrspacecast (i32* null to i32 addrspace(21)*)

; CHECK-LABEL: nullptr22:
; R600-NEXT: .long 0
@nullptr22 = global i32 addrspace(22)* addrspacecast (i32* null to i32 addrspace(22)*)

; CHECK-LABEL: nullptr23:
; R600-NEXT: .long 0
@nullptr23 = global i32 addrspace(23)* addrspacecast (i32* null to i32 addrspace(23)*)

