;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright 1993-95  Microsoft Corporation.  All Rights Reserved.           *
;                                                                           *
;****************************************************************************

.386p

include basedef.inc
include vmm.inc
include debug.inc
;include ifsmgr.inc

.errndef	SEGNUM

IFE	SEGNUM-1

	SEGB	TEXTEQU	<VXD_LOCKED_CODE_SEG>
	SEGE	TEXTEQU	<VXD_LOCKED_CODE_ENDS>

ELSEIFE	SEGNUM-2

	SEGB	TEXTEQU	<VXD_ICODE_SEG>
	SEGE	TEXTEQU	<VXD_ICODE_ENDS>

ELSEIFE	SEGNUM-3

	SEGB	TEXTEQU	<VXD_PAGEABLE_CODE_SEG>
	SEGE	TEXTEQU	<VXD_PAGEABLE_CODE_ENDS>

ELSEIFE	SEGNUM-4

	SEGB	TEXTEQU	<VXD_STATIC_CODE_SEG>
	SEGE	TEXTEQU	<VXD_STATIC_CODE_ENDS>

ELSEIFE	SEGNUM-5

	SEGB	TEXTEQU	<VXD_DEBUG_ONLY_CODE_SEG>
	SEGE	TEXTEQU	<VXD_DEBUG_ONLY_CODE_ENDS>

ELSEIFE	SEGNUM-6

	SEGB	TEXTEQU	<VXD_PNP_CODE_SEG>
	SEGE	TEXTEQU	<VXD_PNP_CODE_ENDS>

ELSE
	%OUT	Wrong SEGNUM
	.err

ENDIF

BEGSEG			MACRO
SEGB
ENDM

ENDSEG			MACRO
SEGE
ENDM

@RETNAME		MACRO	Name
			LOCAL	txt
IFE	SEGNUM-1

	txt	TEXTEQU	@CATSTR(_LCODE_,Name)

ELSEIFE	SEGNUM-2

	txt	TEXTEQU @CATSTR(_ICODE_,Name)

ELSEIFE	SEGNUM-3

	txt	TEXTEQU @CATSTR(_PCODE_,Name)

ELSEIFE	SEGNUM-4

	txt	TEXTEQU @CATSTR(_SCODE_,Name)

ELSEIFE	SEGNUM-5

	txt	TEXTEQU @CATSTR(_DCODE_,Name)

ELSE

	txt	TEXTEQU @CATSTR(_CCODE_,Name)

ENDIF

			EXITM	txt
ENDM


StartCDecl		MACRO	Name
PUBLIC			@RETNAME(Name)
BEGSEG
@RETNAME(Name)		PROC	NEAR
ENDM

EndCDecl		MACRO	Name
@RETNAME(Name)		ENDP
ENDSEG
ENDM

MakeCDecl		MACRO	Name
StartCDecl		Name
			VxDJmp	Name
EndCDecl		Name
ENDM

MakeStdCall		MACRO	Name, Param
StartCDecl		Name&@&Param
	   		VxDJmp	Name
EndCDecl		Name&@&Param
ENDM

