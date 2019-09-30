# Microsoft Developer Studio Generated NMAKE File, Based on getic_3ds_plug.dsp
!IF "$(CFG)" == ""
CFG=getic_3ds_plug - Win32 Debug
!MESSAGE No configuration specified. Defaulting to getic_3ds_plug - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "getic_3ds_plug - Win32 Release" && "$(CFG)" != "getic_3ds_plug - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "getic_3ds_plug.mak" CFG="getic_3ds_plug - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "getic_3ds_plug - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "getic_3ds_plug - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "getic_3ds_plug - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\_bin\ged_plugins\getic_3ds_plug.dll"


CLEAN :
	-@erase "$(INTDIR)\getic_3ds_plug.obj"
	-@erase "$(INTDIR)\getic_3ds_plug.pch"
	-@erase "$(INTDIR)\P3dsLoader.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\getic_3ds_plug.exp"
	-@erase "$(OUTDIR)\getic_3ds_plug.lib"
	-@erase "..\_bin\ged_plugins\getic_3ds_plug.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Od /I "..\baselib" /I "..\_include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETIC_MS3D_PLUG_EXPORTS" /D "_CFG_WIN32_WIN" /Fp"$(INTDIR)\getic_3ds_plug.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\getic_3ds_plug.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\getic_3ds_plug.pdb" /machine:I386 /out:"..\_bin\ged_plugins\getic_3ds_plug.dll" /implib:"$(OUTDIR)\getic_3ds_plug.lib" 
LINK32_OBJS= \
	"$(INTDIR)\getic_3ds_plug.obj" \
	"$(INTDIR)\P3dsLoader.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\_bin\ged_plugins\getic_3ds_plug.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "getic_3ds_plug - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\_bin\ged_plugins\getic_3ds_plug.dll" "$(OUTDIR)\getic_3ds_plug.bsc"


CLEAN :
	-@erase "$(INTDIR)\getic_3ds_plug.obj"
	-@erase "$(INTDIR)\getic_3ds_plug.pch"
	-@erase "$(INTDIR)\getic_3ds_plug.sbr"
	-@erase "$(INTDIR)\P3dsLoader.obj"
	-@erase "$(INTDIR)\P3dsLoader.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\getic_3ds_plug.bsc"
	-@erase "$(OUTDIR)\getic_3ds_plug.exp"
	-@erase "$(OUTDIR)\getic_3ds_plug.lib"
	-@erase "$(OUTDIR)\getic_3ds_plug.pdb"
	-@erase "..\_bin\ged_plugins\getic_3ds_plug.dll"
	-@erase "..\_bin\ged_plugins\getic_3ds_plug.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\baselib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETIC_MS3D_PLUG_EXPORTS" /D "_CFG_WIN32_WIN" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\getic_3ds_plug.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\getic_3ds_plug.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\getic_3ds_plug.sbr" \
	"$(INTDIR)\P3dsLoader.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\getic_3ds_plug.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\getic_3ds_plug.pdb" /debug /machine:I386 /out:"..\_bin\ged_plugins\getic_3ds_plug.dll" /implib:"$(OUTDIR)\getic_3ds_plug.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\getic_3ds_plug.obj" \
	"$(INTDIR)\P3dsLoader.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\_bin\ged_plugins\getic_3ds_plug.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("getic_3ds_plug.dep")
!INCLUDE "getic_3ds_plug.dep"
!ELSE 
!MESSAGE Warning: cannot find "getic_3ds_plug.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "getic_3ds_plug - Win32 Release" || "$(CFG)" == "getic_3ds_plug - Win32 Debug"
SOURCE=.\getic_3ds_plug.cpp

!IF  "$(CFG)" == "getic_3ds_plug - Win32 Release"


"$(INTDIR)\getic_3ds_plug.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\getic_3ds_plug.pch"


!ELSEIF  "$(CFG)" == "getic_3ds_plug - Win32 Debug"


"$(INTDIR)\getic_3ds_plug.obj"	"$(INTDIR)\getic_3ds_plug.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\getic_3ds_plug.pch"


!ENDIF 

SOURCE=.\P3dsLoader.cpp

!IF  "$(CFG)" == "getic_3ds_plug - Win32 Release"


"$(INTDIR)\P3dsLoader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\getic_3ds_plug.pch"


!ELSEIF  "$(CFG)" == "getic_3ds_plug - Win32 Debug"


"$(INTDIR)\P3dsLoader.obj"	"$(INTDIR)\P3dsLoader.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\getic_3ds_plug.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "getic_3ds_plug - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Od /I "..\baselib" /I "..\_include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETIC_MS3D_PLUG_EXPORTS" /D "_CFG_WIN32_WIN" /Fp"$(INTDIR)\getic_3ds_plug.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\getic_3ds_plug.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "getic_3ds_plug - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\baselib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GETIC_MS3D_PLUG_EXPORTS" /D "_CFG_WIN32_WIN" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\getic_3ds_plug.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\getic_3ds_plug.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

