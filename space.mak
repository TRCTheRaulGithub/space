# Microsoft Developer Studio Generated NMAKE File, Based on space.dsp
!IF "$(CFG)" == ""
CFG=space - Win32 Debug
!MESSAGE No configuration specified. Defaulting to space - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "space - Win32 Release" && "$(CFG)" != "space - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "space.mak" CFG="space - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "space - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "space - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "space - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\space.dll"


CLEAN :
	-@erase "$(INTDIR)\graph.obj"
	-@erase "$(INTDIR)\Objet_Effet.obj"
	-@erase "$(INTDIR)\Objet_Space.obj"
	-@erase "$(INTDIR)\Objet_Transformations.obj"
	-@erase "$(INTDIR)\space.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Winamp_Space.obj"
	-@erase "$(OUTDIR)\space.dll"
	-@erase "$(OUTDIR)\space.exp"
	-@erase "$(OUTDIR)\space.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /Fp"$(INTDIR)\space.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\space.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\space.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\space.pdb" /machine:I386 /out:"$(OUTDIR)\space.dll" /implib:"$(OUTDIR)\space.lib" 
LINK32_OBJS= \
	"$(INTDIR)\graph.obj" \
	"$(INTDIR)\Objet_Effet.obj" \
	"$(INTDIR)\Objet_Space.obj" \
	"$(INTDIR)\Objet_Transformations.obj" \
	"$(INTDIR)\Winamp_Space.obj" \
	"$(INTDIR)\space.res"

"$(OUTDIR)\space.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "space - Win32 Debug"

OUTDIR=.
INTDIR=.

ALL : "..\..\..\progra~1\winamp\plugins\vis_space.dll"


CLEAN :
	-@erase "$(INTDIR)\graph.obj"
	-@erase "$(INTDIR)\Objet_Effet.obj"
	-@erase "$(INTDIR)\Objet_Space.obj"
	-@erase "$(INTDIR)\Objet_Transformations.obj"
	-@erase "$(INTDIR)\space.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Winamp_Space.obj"
	-@erase "$(OUTDIR)\vis_space.exp"
	-@erase "$(OUTDIR)\vis_space.pdb"
	-@erase "..\..\..\progra~1\winamp\plugins\vis_space.dll"

CPP=cl.exe
CPP_PROJ=/MD /Zi /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\space.res" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\space.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=ddraw.lib kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib /dll /incremental:no /pdb:"$(OUTDIR)\vis_space.pdb" /debug /machine:I386 /out:"c:/progra~1/winamp/plugins/vis_space.dll" /implib:"$(OUTDIR)\vis_space.lib" 
LINK32_OBJS= \
	"$(INTDIR)\graph.obj" \
	"$(INTDIR)\Objet_Effet.obj" \
	"$(INTDIR)\Objet_Space.obj" \
	"$(INTDIR)\Objet_Transformations.obj" \
	"$(INTDIR)\Winamp_Space.obj" \
	"$(INTDIR)\space.res"

"..\..\..\progra~1\winamp\plugins\vis_space.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("space.dep")
!INCLUDE "space.dep"
!ELSE 
!MESSAGE Warning: cannot find "space.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "space - Win32 Release" || "$(CFG)" == "space - Win32 Debug"
SOURCE=.\graph.cpp

!IF  "$(CFG)" == "space - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /Fp"$(INTDIR)\space.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\graph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "space - Win32 Debug"

CPP_SWITCHES=/MD /Zi /O2 /Op /Ob1 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /FD /c 

"$(INTDIR)\graph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Objet_Effet.cpp

!IF  "$(CFG)" == "space - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /Fp"$(INTDIR)\space.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Objet_Effet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "space - Win32 Debug"

CPP_SWITCHES=/MD /Zd /Od /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /FD /c 

"$(INTDIR)\Objet_Effet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Objet_Space.cpp

!IF  "$(CFG)" == "space - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /Fp"$(INTDIR)\space.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Objet_Space.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "space - Win32 Debug"

CPP_SWITCHES=/MD /Zi /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /FD /c 

"$(INTDIR)\Objet_Space.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Objet_Transformations.cpp

"$(INTDIR)\Objet_Transformations.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Winamp_Space.cpp

!IF  "$(CFG)" == "space - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /Fp"$(INTDIR)\space.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Winamp_Space.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "space - Win32 Debug"

CPP_SWITCHES=/MD /Zi /O2 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPACE_EXPORTS" /FD /c 

"$(INTDIR)\Winamp_Space.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\space.rc

"$(INTDIR)\space.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

