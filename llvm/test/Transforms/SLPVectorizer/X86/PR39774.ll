; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -slp-vectorizer -S < %s -mtriple=x86_64-unknown-linux-gnu -mcpu=skylake -slp-threshold=-7 | FileCheck %s --check-prefixes=ALL,CHECK
; RUN: opt -slp-vectorizer -S < %s -mtriple=x86_64-unknown-linux-gnu -mcpu=skylake -slp-threshold=-8 -slp-min-tree-size=6 | FileCheck %s --check-prefixes=ALL,FORCE_REDUCTION

define void @Test(i32) {
; CHECK-LABEL: @Test(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br label [[LOOP:%.*]]
; CHECK:       loop:
; CHECK-NEXT:    [[TMP1:%.*]] = phi <2 x i32> [ [[TMP15:%.*]], [[LOOP]] ], [ zeroinitializer, [[ENTRY:%.*]] ]
; CHECK-NEXT:    [[SHUFFLE:%.*]] = shufflevector <2 x i32> [[TMP1]], <2 x i32> undef, <8 x i32> <i32 0, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1>
; CHECK-NEXT:    [[TMP2:%.*]] = extractelement <8 x i32> [[SHUFFLE]], i32 1
<<<<<<< HEAD
; CHECK-NEXT:    [[TMP3:%.*]] = add <8 x i32> [[SHUFFLE]], <i32 0, i32 55, i32 285, i32 1240, i32 1496, i32 8555, i32 12529, i32 13685>
=======
; CHECK-NEXT:    [[TMP3:%.*]] = add <8 x i32> <i32 0, i32 55, i32 285, i32 1240, i32 1496, i32 8555, i32 12529, i32 13685>, [[SHUFFLE]]
>>>>>>> release/8.x
; CHECK-NEXT:    [[VAL_1:%.*]] = and i32 [[TMP2]], undef
; CHECK-NEXT:    [[VAL_2:%.*]] = and i32 [[VAL_1]], [[TMP0:%.*]]
; CHECK-NEXT:    [[VAL_3:%.*]] = and i32 [[VAL_2]], [[TMP0]]
; CHECK-NEXT:    [[VAL_4:%.*]] = and i32 [[VAL_3]], [[TMP0]]
; CHECK-NEXT:    [[VAL_5:%.*]] = and i32 [[VAL_4]], [[TMP0]]
; CHECK-NEXT:    [[VAL_7:%.*]] = and i32 [[VAL_5]], undef
; CHECK-NEXT:    [[VAL_8:%.*]] = and i32 [[VAL_7]], [[TMP0]]
; CHECK-NEXT:    [[VAL_9:%.*]] = and i32 [[VAL_8]], [[TMP0]]
; CHECK-NEXT:    [[VAL_10:%.*]] = and i32 [[VAL_9]], [[TMP0]]
; CHECK-NEXT:    [[VAL_12:%.*]] = and i32 [[VAL_10]], undef
; CHECK-NEXT:    [[VAL_13:%.*]] = and i32 [[VAL_12]], [[TMP0]]
; CHECK-NEXT:    [[VAL_14:%.*]] = and i32 [[VAL_13]], [[TMP0]]
; CHECK-NEXT:    [[VAL_15:%.*]] = and i32 [[VAL_14]], [[TMP0]]
; CHECK-NEXT:    [[VAL_16:%.*]] = and i32 [[VAL_15]], [[TMP0]]
; CHECK-NEXT:    [[VAL_17:%.*]] = and i32 [[VAL_16]], [[TMP0]]
; CHECK-NEXT:    [[VAL_19:%.*]] = and i32 [[VAL_17]], undef
; CHECK-NEXT:    [[VAL_21:%.*]] = and i32 [[VAL_19]], undef
; CHECK-NEXT:    [[VAL_22:%.*]] = and i32 [[VAL_21]], [[TMP0]]
; CHECK-NEXT:    [[VAL_23:%.*]] = and i32 [[VAL_22]], [[TMP0]]
; CHECK-NEXT:    [[VAL_24:%.*]] = and i32 [[VAL_23]], [[TMP0]]
; CHECK-NEXT:    [[VAL_25:%.*]] = and i32 [[VAL_24]], [[TMP0]]
; CHECK-NEXT:    [[VAL_26:%.*]] = and i32 [[VAL_25]], [[TMP0]]
; CHECK-NEXT:    [[VAL_27:%.*]] = and i32 [[VAL_26]], [[TMP0]]
; CHECK-NEXT:    [[VAL_28:%.*]] = and i32 [[VAL_27]], [[TMP0]]
; CHECK-NEXT:    [[VAL_29:%.*]] = and i32 [[VAL_28]], [[TMP0]]
; CHECK-NEXT:    [[VAL_30:%.*]] = and i32 [[VAL_29]], [[TMP0]]
; CHECK-NEXT:    [[VAL_31:%.*]] = and i32 [[VAL_30]], [[TMP0]]
; CHECK-NEXT:    [[VAL_32:%.*]] = and i32 [[VAL_31]], [[TMP0]]
; CHECK-NEXT:    [[VAL_33:%.*]] = and i32 [[VAL_32]], [[TMP0]]
; CHECK-NEXT:    [[VAL_35:%.*]] = and i32 [[VAL_33]], undef
; CHECK-NEXT:    [[VAL_36:%.*]] = and i32 [[VAL_35]], [[TMP0]]
; CHECK-NEXT:    [[VAL_37:%.*]] = and i32 [[VAL_36]], [[TMP0]]
; CHECK-NEXT:    [[VAL_38:%.*]] = and i32 [[VAL_37]], [[TMP0]]
; CHECK-NEXT:    [[VAL_40:%.*]] = and i32 [[VAL_38]], undef
; CHECK-NEXT:    [[RDX_SHUF:%.*]] = shufflevector <8 x i32> [[TMP3]], <8 x i32> undef, <8 x i32> <i32 4, i32 5, i32 6, i32 7, i32 undef, i32 undef, i32 undef, i32 undef>
; CHECK-NEXT:    [[BIN_RDX:%.*]] = and <8 x i32> [[TMP3]], [[RDX_SHUF]]
; CHECK-NEXT:    [[RDX_SHUF1:%.*]] = shufflevector <8 x i32> [[BIN_RDX]], <8 x i32> undef, <8 x i32> <i32 2, i32 3, i32 undef, i32 undef, i32 undef, i32 undef, i32 undef, i32 undef>
; CHECK-NEXT:    [[BIN_RDX2:%.*]] = and <8 x i32> [[BIN_RDX]], [[RDX_SHUF1]]
; CHECK-NEXT:    [[RDX_SHUF3:%.*]] = shufflevector <8 x i32> [[BIN_RDX2]], <8 x i32> undef, <8 x i32> <i32 1, i32 undef, i32 undef, i32 undef, i32 undef, i32 undef, i32 undef, i32 undef>
; CHECK-NEXT:    [[BIN_RDX4:%.*]] = and <8 x i32> [[BIN_RDX2]], [[RDX_SHUF3]]
; CHECK-NEXT:    [[TMP4:%.*]] = extractelement <8 x i32> [[BIN_RDX4]], i32 0
; CHECK-NEXT:    [[OP_EXTRA:%.*]] = and i32 [[TMP4]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA5:%.*]] = and i32 [[OP_EXTRA]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA6:%.*]] = and i32 [[OP_EXTRA5]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA7:%.*]] = and i32 [[OP_EXTRA6]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA8:%.*]] = and i32 [[OP_EXTRA7]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA9:%.*]] = and i32 [[OP_EXTRA8]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA10:%.*]] = and i32 [[OP_EXTRA9]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA11:%.*]] = and i32 [[OP_EXTRA10]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA12:%.*]] = and i32 [[OP_EXTRA11]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA13:%.*]] = and i32 [[OP_EXTRA12]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA14:%.*]] = and i32 [[OP_EXTRA13]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA15:%.*]] = and i32 [[OP_EXTRA14]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA16:%.*]] = and i32 [[OP_EXTRA15]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA17:%.*]] = and i32 [[OP_EXTRA16]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA18:%.*]] = and i32 [[OP_EXTRA17]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA19:%.*]] = and i32 [[OP_EXTRA18]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA20:%.*]] = and i32 [[OP_EXTRA19]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA21:%.*]] = and i32 [[OP_EXTRA20]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA22:%.*]] = and i32 [[OP_EXTRA21]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA23:%.*]] = and i32 [[OP_EXTRA22]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA24:%.*]] = and i32 [[OP_EXTRA23]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA25:%.*]] = and i32 [[OP_EXTRA24]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA26:%.*]] = and i32 [[OP_EXTRA25]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA27:%.*]] = and i32 [[OP_EXTRA26]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA28:%.*]] = and i32 [[OP_EXTRA27]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA29:%.*]] = and i32 [[OP_EXTRA28]], [[TMP0]]
; CHECK-NEXT:    [[OP_EXTRA30:%.*]] = and i32 [[OP_EXTRA29]], [[TMP0]]
; CHECK-NEXT:    [[VAL_42:%.*]] = and i32 [[VAL_40]], undef
; CHECK-NEXT:    [[TMP5:%.*]] = insertelement <2 x i32> undef, i32 [[OP_EXTRA30]], i32 0
<<<<<<< HEAD
; CHECK-NEXT:    [[TMP6:%.*]] = insertelement <2 x i32> [[TMP5]], i32 14910, i32 1
; CHECK-NEXT:    [[TMP7:%.*]] = insertelement <2 x i32> undef, i32 [[TMP2]], i32 0
; CHECK-NEXT:    [[TMP8:%.*]] = insertelement <2 x i32> [[TMP7]], i32 [[TMP2]], i32 1
=======
; CHECK-NEXT:    [[TMP6:%.*]] = insertelement <2 x i32> [[TMP5]], i32 [[TMP2]], i32 1
; CHECK-NEXT:    [[TMP7:%.*]] = insertelement <2 x i32> undef, i32 [[TMP2]], i32 0
; CHECK-NEXT:    [[TMP8:%.*]] = insertelement <2 x i32> [[TMP7]], i32 14910, i32 1
>>>>>>> release/8.x
; CHECK-NEXT:    [[TMP9:%.*]] = and <2 x i32> [[TMP6]], [[TMP8]]
; CHECK-NEXT:    [[TMP10:%.*]] = add <2 x i32> [[TMP6]], [[TMP8]]
; CHECK-NEXT:    [[TMP11:%.*]] = shufflevector <2 x i32> [[TMP9]], <2 x i32> [[TMP10]], <2 x i32> <i32 0, i32 3>
; CHECK-NEXT:    [[TMP12:%.*]] = extractelement <2 x i32> [[TMP11]], i32 0
; CHECK-NEXT:    [[TMP13:%.*]] = insertelement <2 x i32> undef, i32 [[TMP12]], i32 0
; CHECK-NEXT:    [[TMP14:%.*]] = extractelement <2 x i32> [[TMP11]], i32 1
; CHECK-NEXT:    [[TMP15]] = insertelement <2 x i32> [[TMP13]], i32 [[TMP14]], i32 1
; CHECK-NEXT:    br label [[LOOP]]
;
; FORCE_REDUCTION-LABEL: @Test(
; FORCE_REDUCTION-NEXT:  entry:
; FORCE_REDUCTION-NEXT:    br label [[LOOP:%.*]]
; FORCE_REDUCTION:       loop:
; FORCE_REDUCTION-NEXT:    [[TMP1:%.*]] = phi <2 x i32> [ [[TMP13:%.*]], [[LOOP]] ], [ zeroinitializer, [[ENTRY:%.*]] ]
; FORCE_REDUCTION-NEXT:    [[SHUFFLE:%.*]] = shufflevector <2 x i32> [[TMP1]], <2 x i32> undef, <4 x i32> <i32 0, i32 1, i32 1, i32 1>
; FORCE_REDUCTION-NEXT:    [[TMP2:%.*]] = extractelement <4 x i32> [[SHUFFLE]], i32 1
<<<<<<< HEAD
; FORCE_REDUCTION-NEXT:    [[TMP3:%.*]] = add <4 x i32> [[SHUFFLE]], <i32 0, i32 55, i32 285, i32 1240>
=======
; FORCE_REDUCTION-NEXT:    [[TMP3:%.*]] = add <4 x i32> <i32 0, i32 55, i32 285, i32 1240>, [[SHUFFLE]]
>>>>>>> release/8.x
; FORCE_REDUCTION-NEXT:    [[VAL_1:%.*]] = and i32 [[TMP2]], undef
; FORCE_REDUCTION-NEXT:    [[VAL_2:%.*]] = and i32 [[VAL_1]], [[TMP0:%.*]]
; FORCE_REDUCTION-NEXT:    [[VAL_3:%.*]] = and i32 [[VAL_2]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_4:%.*]] = and i32 [[VAL_3]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_5:%.*]] = and i32 [[VAL_4]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_7:%.*]] = and i32 [[VAL_5]], undef
; FORCE_REDUCTION-NEXT:    [[VAL_8:%.*]] = and i32 [[VAL_7]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_9:%.*]] = and i32 [[VAL_8]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_10:%.*]] = and i32 [[VAL_9]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_12:%.*]] = and i32 [[VAL_10]], undef
; FORCE_REDUCTION-NEXT:    [[VAL_13:%.*]] = and i32 [[VAL_12]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_14:%.*]] = and i32 [[VAL_13]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_15:%.*]] = and i32 [[VAL_14]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_16:%.*]] = and i32 [[VAL_15]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_17:%.*]] = and i32 [[VAL_16]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_19:%.*]] = and i32 [[VAL_17]], undef
; FORCE_REDUCTION-NEXT:    [[VAL_20:%.*]] = add i32 [[TMP2]], 1496
; FORCE_REDUCTION-NEXT:    [[VAL_21:%.*]] = and i32 [[VAL_19]], [[VAL_20]]
; FORCE_REDUCTION-NEXT:    [[VAL_22:%.*]] = and i32 [[VAL_21]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_23:%.*]] = and i32 [[VAL_22]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_24:%.*]] = and i32 [[VAL_23]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_25:%.*]] = and i32 [[VAL_24]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_26:%.*]] = and i32 [[VAL_25]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_27:%.*]] = and i32 [[VAL_26]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_28:%.*]] = and i32 [[VAL_27]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_29:%.*]] = and i32 [[VAL_28]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_30:%.*]] = and i32 [[VAL_29]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_31:%.*]] = and i32 [[VAL_30]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_32:%.*]] = and i32 [[VAL_31]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_33:%.*]] = and i32 [[VAL_32]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_34:%.*]] = add i32 [[TMP2]], 8555
; FORCE_REDUCTION-NEXT:    [[VAL_35:%.*]] = and i32 [[VAL_33]], [[VAL_34]]
; FORCE_REDUCTION-NEXT:    [[VAL_36:%.*]] = and i32 [[VAL_35]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_37:%.*]] = and i32 [[VAL_36]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[RDX_SHUF:%.*]] = shufflevector <4 x i32> [[TMP3]], <4 x i32> undef, <4 x i32> <i32 2, i32 3, i32 undef, i32 undef>
; FORCE_REDUCTION-NEXT:    [[BIN_RDX:%.*]] = and <4 x i32> [[TMP3]], [[RDX_SHUF]]
; FORCE_REDUCTION-NEXT:    [[RDX_SHUF1:%.*]] = shufflevector <4 x i32> [[BIN_RDX]], <4 x i32> undef, <4 x i32> <i32 1, i32 undef, i32 undef, i32 undef>
; FORCE_REDUCTION-NEXT:    [[BIN_RDX2:%.*]] = and <4 x i32> [[BIN_RDX]], [[RDX_SHUF1]]
; FORCE_REDUCTION-NEXT:    [[TMP4:%.*]] = extractelement <4 x i32> [[BIN_RDX2]], i32 0
; FORCE_REDUCTION-NEXT:    [[TMP5:%.*]] = and i32 [[TMP4]], [[VAL_20]]
; FORCE_REDUCTION-NEXT:    [[TMP6:%.*]] = and i32 [[TMP5]], [[VAL_34]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA:%.*]] = and i32 [[TMP6]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA3:%.*]] = and i32 [[OP_EXTRA]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA4:%.*]] = and i32 [[OP_EXTRA3]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA5:%.*]] = and i32 [[OP_EXTRA4]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA6:%.*]] = and i32 [[OP_EXTRA5]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA7:%.*]] = and i32 [[OP_EXTRA6]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA8:%.*]] = and i32 [[OP_EXTRA7]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA9:%.*]] = and i32 [[OP_EXTRA8]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA10:%.*]] = and i32 [[OP_EXTRA9]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA11:%.*]] = and i32 [[OP_EXTRA10]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA12:%.*]] = and i32 [[OP_EXTRA11]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA13:%.*]] = and i32 [[OP_EXTRA12]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA14:%.*]] = and i32 [[OP_EXTRA13]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA15:%.*]] = and i32 [[OP_EXTRA14]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA16:%.*]] = and i32 [[OP_EXTRA15]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA17:%.*]] = and i32 [[OP_EXTRA16]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA18:%.*]] = and i32 [[OP_EXTRA17]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA19:%.*]] = and i32 [[OP_EXTRA18]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA20:%.*]] = and i32 [[OP_EXTRA19]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA21:%.*]] = and i32 [[OP_EXTRA20]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA22:%.*]] = and i32 [[OP_EXTRA21]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA23:%.*]] = and i32 [[OP_EXTRA22]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA24:%.*]] = and i32 [[OP_EXTRA23]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA25:%.*]] = and i32 [[OP_EXTRA24]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA26:%.*]] = and i32 [[OP_EXTRA25]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA27:%.*]] = and i32 [[OP_EXTRA26]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA28:%.*]] = and i32 [[OP_EXTRA27]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[OP_EXTRA29:%.*]] = and i32 [[OP_EXTRA28]], [[TMP2]]
; FORCE_REDUCTION-NEXT:    [[VAL_38:%.*]] = and i32 [[VAL_37]], [[TMP0]]
; FORCE_REDUCTION-NEXT:    [[VAL_39:%.*]] = add i32 [[TMP2]], 12529
; FORCE_REDUCTION-NEXT:    [[VAL_40:%.*]] = and i32 [[OP_EXTRA29]], [[VAL_39]]
; FORCE_REDUCTION-NEXT:    [[VAL_41:%.*]] = add i32 [[TMP2]], 13685
; FORCE_REDUCTION-NEXT:    [[TMP7:%.*]] = insertelement <2 x i32> undef, i32 [[VAL_40]], i32 0
; FORCE_REDUCTION-NEXT:    [[TMP8:%.*]] = insertelement <2 x i32> [[TMP7]], i32 [[TMP2]], i32 1
; FORCE_REDUCTION-NEXT:    [[TMP9:%.*]] = insertelement <2 x i32> undef, i32 [[VAL_41]], i32 0
; FORCE_REDUCTION-NEXT:    [[TMP10:%.*]] = insertelement <2 x i32> [[TMP9]], i32 14910, i32 1
; FORCE_REDUCTION-NEXT:    [[TMP11:%.*]] = and <2 x i32> [[TMP8]], [[TMP10]]
; FORCE_REDUCTION-NEXT:    [[TMP12:%.*]] = add <2 x i32> [[TMP8]], [[TMP10]]
; FORCE_REDUCTION-NEXT:    [[TMP13]] = shufflevector <2 x i32> [[TMP11]], <2 x i32> [[TMP12]], <2 x i32> <i32 0, i32 3>
; FORCE_REDUCTION-NEXT:    br label [[LOOP]]
;
entry:
  br label %loop

loop:
  %local_4_39.us = phi i32 [ %val_42, %loop ], [ 0, %entry ]
  %local_8_43.us = phi i32 [ %val_43, %loop ], [ 0, %entry ]
  %val_0 = add i32 %local_4_39.us, 0
  %val_1 = and i32 %local_8_43.us, %val_0
  %val_2 = and i32 %val_1, %0
  %val_3 = and i32 %val_2, %0
  %val_4 = and i32 %val_3, %0
  %val_5 = and i32 %val_4, %0
  %val_6 = add i32 %local_8_43.us, 55
  %val_7 = and i32 %val_5, %val_6
  %val_8 = and i32 %val_7, %0
  %val_9 = and i32 %val_8, %0
  %val_10 = and i32 %val_9, %0
  %val_11 = add i32 %local_8_43.us, 285
  %val_12 = and i32 %val_10, %val_11
  %val_13 = and i32 %val_12, %0
  %val_14 = and i32 %val_13, %0
  %val_15 = and i32 %val_14, %0
  %val_16 = and i32 %val_15, %0
  %val_17 = and i32 %val_16, %0
  %val_18 = add i32 %local_8_43.us, 1240
  %val_19 = and i32 %val_17, %val_18
  %val_20 = add i32 %local_8_43.us, 1496
  %val_21 = and i32 %val_19, %val_20
  %val_22 = and i32 %val_21, %0
  %val_23 = and i32 %val_22, %0
  %val_24 = and i32 %val_23, %0
  %val_25 = and i32 %val_24, %0
  %val_26 = and i32 %val_25, %0
  %val_27 = and i32 %val_26, %0
  %val_28 = and i32 %val_27, %0
  %val_29 = and i32 %val_28, %0
  %val_30 = and i32 %val_29, %0
  %val_31 = and i32 %val_30, %0
  %val_32 = and i32 %val_31, %0
  %val_33 = and i32 %val_32, %0
  %val_34 = add i32 %local_8_43.us, 8555
  %val_35 = and i32 %val_33, %val_34
  %val_36 = and i32 %val_35, %0
  %val_37 = and i32 %val_36, %0
  %val_38 = and i32 %val_37, %0
  %val_39 = add i32 %local_8_43.us, 12529
  %val_40 = and i32 %val_38, %val_39
  %val_41 = add i32 %local_8_43.us, 13685
  %val_42 = and i32 %val_40, %val_41
  %val_43 = add i32 %local_8_43.us, 14910
  br label %loop
}
