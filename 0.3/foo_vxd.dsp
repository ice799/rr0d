# Microsoft Developer Studio Project File - Name="foo_vxd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=foo_vxd - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "foo_vxd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "foo_vxd.mak" CFG="foo_vxd - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "foo_vxd - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "foo_vxd - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "foo_vxd - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /nologo CFG="Release""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Release\foo_vxd.VxD"
# PROP BASE Bsc_Name "Release\foo_vxd.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /nologo CFG="Release""
# PROP Rebuild_Opt "/a"
# PROP Target_File "Release\foo_vxd.VxD"
# PROP Bsc_Name "Release\foo_vxd.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "foo_vxd - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /nologo /k"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Debug\foo_vxd.vxd"
# PROP BASE Bsc_Name "Debug\foo_vxd.bsc"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /nologo /k CFG="Debug""
# PROP Rebuild_Opt "/a"
# PROP Target_File "Debug\foo_vxd.VxD"
# PROP Bsc_Name "Debug\foo_vxd.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "foo_vxd - Win32 Release"
# Name "foo_vxd - Win32 Debug"

!IF  "$(CFG)" == "foo_vxd - Win32 Release"

!ELSEIF  "$(CFG)" == "foo_vxd - Win32 Debug"

!ENDIF 

# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\breakpoint.h
# End Source File
# Begin Source File

SOURCE=.\buffering.h
# End Source File
# Begin Source File

SOURCE=.\command.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\disasm.h
# End Source File
# Begin Source File

SOURCE=.\font_256.h
# End Source File
# Begin Source File

SOURCE=.\foo_vxd.h
# End Source File
# Begin Source File

SOURCE=.\idt.h
# End Source File
# Begin Source File

SOURCE=.\keyboard.h
# End Source File
# Begin Source File

SOURCE=.\page.h
# End Source File
# Begin Source File

SOURCE=.\var_globale.h
# End Source File
# Begin Source File

SOURCE=.\video.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\foo_vxd.rc
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\breakpoint.c
# End Source File
# Begin Source File

SOURCE=.\buffering.c
# End Source File
# Begin Source File

SOURCE=.\command.c
# End Source File
# Begin Source File

SOURCE=.\disasmbak.c
# End Source File
# Begin Source File

SOURCE=.\idt.c
# End Source File
# Begin Source File

SOURCE=.\keyboard.c
# End Source File
# Begin Source File

SOURCE=.\page.c
# End Source File
# Begin Source File

SOURCE=.\Spy.c
# End Source File
# Begin Source File

SOURCE=.\SpyAsm.asm
# End Source File
# Begin Source File

SOURCE=.\video.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\makefile
# End Source File
# End Target
# End Project
