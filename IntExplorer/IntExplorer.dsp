# Microsoft Developer Studio Project File - Name="IntExplorer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IntExplorer - Win32 Debug_CII
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IntExplorer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IntExplorer.mak" CFG="IntExplorer - Win32 Debug_CII"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IntExplorer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IntExplorer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IntExplorer - Win32 Debug_CII" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cossacks2/IntExplorer", LQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IntExplorer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../_release"
# PROP Intermediate_Dir "../_release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTEXPLORER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTEXPLORER_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 cossacks2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /machine:I386 /out:"c:\COSSACKS2\IntExplorer.dll" /libpath:"../lib" /libpath:"../_release" /MAPINFO:EXPORTS /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IntExplorer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_debug"
# PROP Intermediate_Dir "../_debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTEXPLORER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /ML /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTEXPLORER_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 c:\AC\dmcr.lib wsock32.lib ws2_32.lib ichat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /map:"..\bin\intExplorer.map" /debug /machine:I386 /out:"..\bin\intExplorer.dll" /pdbtype:sept /libpath:"../lib" /libpath:"../_debug" /MAPINFO:EXPORTS /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=..\bat\all.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IntExplorer - Win32 Debug_CII"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "IntExplorer___Win32_Debug_CII"
# PROP BASE Intermediate_Dir "IntExplorer___Win32_Debug_CII"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_debug/IntExplorer"
# PROP Intermediate_Dir "../_debug/IntExplorer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /ML /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INTEXPLORER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "INTEXPLORER_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_COSSACKS2" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib ws2_32.lib ichat.lib cossacks2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"e:\arc_usa\intExplorer.dll" /pdbtype:sept /libpath:"../lib" /libpath:"../_debug" /MAPINFO:EXPORTS /MAPINFO:LINES
# SUBTRACT BASE LINK32 /pdb:none /incremental:no
# ADD LINK32 wsock32.lib ws2_32.lib ichat.lib cossacks2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"d:\c2forGec\intExplorer.dll" /pdbtype:sept /libpath:"../lib" /libpath:"../_debug" /MAPINFO:EXPORTS /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Custom Build - Copying IntExplorer.lib to the ..\lib directory...
InputPath=d:\c2forGec\intExplorer.dll
InputName=intExplorer
SOURCE="$(InputPath)"

"..\lib\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if        not        exist         ..\lib\          md          ..\lib\  
	copy ..\_debug\IntExplorer\$(InputName).lib ..\lib\$(InputName).lib 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "IntExplorer - Win32 Release"
# Name "IntExplorer - Win32 Debug"
# Name "IntExplorer - Win32 Debug_CII"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Bmptool.cpp
# End Source File
# Begin Source File

SOURCE=.\DataXchange.cpp
# End Source File
# Begin Source File

SOURCE=.\DString.cpp
# End Source File
# Begin Source File

SOURCE=.\Explore.cpp
# End Source File
# Begin Source File

SOURCE=.\FnDefine.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOnMap.cpp
# End Source File
# Begin Source File

SOURCE=.\IntExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\ParseRQ.cpp
# End Source File
# Begin Source File

SOURCE=.\PingEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Bmptool.h
# End Source File
# Begin Source File

SOURCE=..\common.h
# End Source File
# Begin Source File

SOURCE=.\DString.h
# End Source File
# Begin Source File

SOURCE=.\GameOnMap.h
# End Source File
# Begin Source File

SOURCE=.\IntExplorer.h
# End Source File
# Begin Source File

SOURCE=.\ParseRQ.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
