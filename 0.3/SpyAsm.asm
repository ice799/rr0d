;******************************************************************************
TITLE CONTROL - ControlDispatch for VxD in C
;******************************************************************************
;

    .386p

;******************************************************************************
;                I N C L U D E S
;******************************************************************************
IFSFN_DELETE Equ 31
IFSFN_OPEN   Equ 36
IFSFN_READ   Equ 0
    .xlist
    include vmm.inc
    include debug.inc
    include ifsmgr.inc

public _Sprintf1

; the following equate makes the VXD dynamically loadable.
SPY_DYNAMIC EQU 1

;============================================================================
;        V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================

DECLARE_VIRTUAL_DEVICE    SPY, 1, 0, SPY_Control, UNDEFINED_DEVICE_ID, \
                        UNDEFINED_INIT_ORDER 

VxD_LOCKED_CODE_SEG

;===========================================================================
;
;   PROCEDURE: SPY_Control
;
;   DESCRIPTION:
;    Device control procedure for the SPY VxD
;
;   ENTRY:
;    EAX = Control call ID
;
;   EXIT:
;    If carry clear then
;        Successful
;    else
;        Control call failed
;
;   USES:
;    EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;============================================================================

BeginProc SPY_Control
    Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, SPY_Dynamic_Init, sCall
    Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, SPY_Dynamic_Exit, sCall
    Control_Dispatch W32_DEVICEIOCONTROL,     SPY_W32_DeviceIOControl, sCall, <ecx, ebx, edx, esi>
    clc
    ret
EndProc SPY_Control

VxD_LOCKED_CODE_ENDS

; *************************************************************************
VxD_LOCKED_DATA_SEG
PrevFunc        Dd      0
VxD_LOCKED_DATA_ENDS
; *************************************************************************

VxD_CODE_SEG
BeginProc       _Sprintf1
        PUSH    EBP
        MOV     EBP, ESP
        VMMCall _Sprintf, <[EBP+8h], [EBP+0Ch], [EBP+10h], [EBP+14h]>
        POP     EBP
        RET
EndProc         _Sprintf1
VxD_CODE_ENDS
    END
