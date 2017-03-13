# \base\mak\BF9PROFL.MAK
# Created by IBM WorkFrame/2 MakeMake at 01:14:16 on 01/25/02
# Manually modified by EDW at 16:19:01 on 01/27/02
# All modifications to this file should be made manually. 
# DO NOT REGENERATE USING IBM WorkFrame/2 MakeMake
#
# To turn debugging on, uncomment the 'debug on' version of the 
# DEBUGC and DEBUGL flags below and comment out the 'debug off' 
# version of the DEBUGC and DEBUGL flags below.
#
# The actions included in this make file are:
#  Compile
#  Make exp and lib files
#  Link

PRD = .
SRC = SRC
OBJ = OBJP
BRLIB = LIB

# Flags that turn debugging off
DEBUGC = Ti-
DEBUGL = nodebug

#Flags that turn debugging on
#DEBUGC = Ti+
#DEBUGL = debug

.SUFFIXES:

.all: \
    $(PRD)\BF9PROFL.DLL

$(OBJ)\AF9RECPF.OBJ: \
    $(SRC)\AF9RECPF.CPP \
    {$(SRC);$(INCLUDE);}af9recpf.hpp \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9RECPF.CPP

$(OBJ)\AF9PTION.OBJ: \
    $(SRC)\AF9PTION.CPP \
    {$(SRC);$(INCLUDE);}af9ption.hpp \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9PTION.CPP

$(OBJ)\AF9PROFL.OBJ: \
    $(SRC)\AF9PROFL.CPP \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9PROFL.CPP

$(OBJ)\AF9PFDLG.OBJ: \
    $(SRC)\AF9PFDLG.CPP \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9rtsrc.hpp \
    {$(SRC);$(INCLUDE);}af9recpf.hpp \
    {$(SRC);$(INCLUDE);}af9ption.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9PFDLG.CPP

$(OBJ)\AF9PFAPI.OBJ: \
    $(SRC)\AF9PFAPI.CPP \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}bf9profl.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9PFAPI.CPP

$(OBJ)\AF9RTSRC.OBJ: \
    $(SRC)\AF9RTSRC.CPP \
    {$(SRC);$(INCLUDE);}af9rtsrc.hpp \
    {$(SRC);$(INCLUDE);}af9pfdlg.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}ub9host.h \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /DBR_PROFILE /Wcnd- /$(DEBUGC) /Tx /Gm /Gd /Ge- /Fo"$(OBJ)\%|fF.OBJ" /C $(SRC)\AF9RTSRC.CPP

$(BRLIB)\BF9PROFL.EXP: \
    $(BRLIB)\BF9PROFL.DEF \
    $(OBJ)\AF9RTSRC.OBJ \
    $(OBJ)\AF9PFAPI.OBJ \
    $(OBJ)\AF9PFDLG.OBJ \
    $(OBJ)\AF9PROFL.OBJ \
    $(OBJ)\AF9PTION.OBJ \
    $(OBJ)\AF9RECPF.OBJ
    @echo " Make exp and lib files "
    ilib.exe /Gi:$(BRLIB)\BF9PROFL $(BRLIB)\BF9PROFL.DEF $(OBJ)\AF9RECPF.OBJ $(OBJ)\AF9PTION.OBJ $(OBJ)\AF9PROFL.OBJ $(OBJ)\AF9PFDLG.OBJ $(OBJ)\AF9PFAPI.OBJ $(OBJ)\AF9RTSRC.OBJ

$(PRD)\BF9PROFL.DLL: \
    $(OBJ)\AF9RECPF.OBJ \
    $(BRLIB)\BF9PROFL.EXP \
    $(OBJ)\AF9RTSRC.OBJ \
    $(OBJ)\AF9PFAPI.OBJ \
    $(OBJ)\AF9PFDLG.OBJ \
    $(OBJ)\AF9PROFL.OBJ \
    $(OBJ)\AF9PTION.OBJ \
    {$(LIB)}advapi32.lib \
    {$(LIB)}cppwm35.lib \
    {$(LIB)}user32.lib \
    {$(LIB)}gdi32.lib \
    {$(LIB)}comdlg32.lib
    @echo " Link "
    icc.exe @<<
    /DNO_STRICT /DBR_PROFILE /Tdp 
     /Wcnd- 
     /$(DEBUGC) 
     /Tx 
     /Gm /Gd /Ge- 
      /B" /$(DEBUGL) /nobrowse /pmtype:pm /code:RX /data:RW /dll"
     /B" /def"
     /B" /def:advapi32.lib"
     /B" /def:cppwm35.lib"
     /B" /def:user32.lib"
     /B" /def:gdi32.lib"
     /B" /def:comdlg32.lib"
     /FeBF9PROFL.DLL 
     advapi32.lib 
     cppwm35.lib 
     user32.lib 
     gdi32.lib 
     comdlg32.lib 
     $(OBJ)\AF9RECPF.OBJ
     $(BRLIB)\BF9PROFL.EXP
     $(OBJ)\AF9RTSRC.OBJ
     $(OBJ)\AF9PFAPI.OBJ
     $(OBJ)\AF9PFDLG.OBJ
     $(OBJ)\AF9PROFL.OBJ
     $(OBJ)\AF9PTION.OBJ
<<
