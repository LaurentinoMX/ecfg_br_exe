# \mak\rmcomppk.mak  Created by EDW on 10/13/99 using ...
# \mak\rmcomppk.mak  Created by IBM WorkFrame/2 MakeMake at 10:44:49 on 10/07/99
#
# The actions included in this make file are:
#  Compile
#  Link

RM2PPK = RM2\SRC\PPK
OBJ = OBJ\PPK
DEBUGC = Ti-
DEBUGL = nodebug
#DEBUGC = Ti
#DEBUGL = debug

.SUFFIXES:

.all: \
    .\RMCOMPPK.EXE

$(OBJ)\PKFEATL.obj: \
    $(RM2PPK)\PKFEATL.CPP \
    {$(RM2PPK);$(INCLUDE);}euoutil.hpp \
    {$(RM2PPK);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2PPK);$(INCLUDE);}euomsg.h \
    {$(RM2PPK);$(INCLUDE);}uicl.hpp \
    {$(RM2PPK);$(INCLUDE);}pkfeat.hpp \
    {$(RM2PPK);$(INCLUDE);}parr.hpp \
    {$(RM2PPK);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PKFEATL.CPP

$(OBJ)\PKFEAT.obj: \
    $(RM2PPK)\PKFEAT.CPP \
    {$(RM2PPK);$(INCLUDE);}euoutil.hpp \
    {$(RM2PPK);$(INCLUDE);}pkfeat.hpp \
    {$(RM2PPK);$(INCLUDE);}uicl.hpp \
    {$(RM2PPK);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PKFEAT.CPP

$(OBJ)\PARRREC.obj: \
    $(RM2PPK)\PARRREC.CPP \
    {$(RM2PPK);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PARRREC.CPP

$(OBJ)\PARR.obj: \
    $(RM2PPK)\PARR.CPP \
    {$(RM2PPK);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PARR.CPP

$(OBJ)\EUOUTIL.obj: \
    $(RM2PPK)\EUOUTIL.CPP \
    {$(RM2PPK);$(INCLUDE);}uicl.hpp \
    {$(RM2PPK);$(INCLUDE);}euoutil.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\EUOUTIL.CPP

$(OBJ)\RMCOMPPK.obj: \
    $(RM2PPK)\mainsrc\RMCOMPPK.CPP \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}pkfeatl.hpp \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}pkfeat.hpp \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}parr.hpp \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}euoutil.hpp \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}euodef.h \
    {$(RM2PPK)\mainsrc;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /$(DEBUGC) /Gm /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\mainsrc\RMCOMPPK.CPP

.\RMCOMPPK.EXE: \
    $(OBJ)\PKFEATL.obj \
    $(OBJ)\RMCOMPPK.obj \
    $(OBJ)\EUOUTIL.obj \
    $(OBJ)\PARR.obj \
    $(OBJ)\PARRREC.obj \
    $(OBJ)\PKFEAT.obj \
    {$(LIB)}CPPWM35.LIB
    @echo " Link "
    icc.exe @<<
    /Tdp  /DNO_STRICT 
     /Tx /$(DEBUGC)
     /Gm /Gd
      /B" /$(DEBUGL) /nobrowse /pmtype:vio /code:RX /data:RW"
     /B" /def"
     /B" /def:"
     /B" /def:CPPWM35.LIB"
     /Fe.\RMCOMPPK.EXE
     CPPWM35.LIB
     $(OBJ)\PKFEATL.obj
     $(OBJ)\RMCOMPPK.obj
     $(OBJ)\EUOUTIL.obj
     $(OBJ)\PARR.obj
     $(OBJ)\PARRREC.obj
     $(OBJ)\PKFEAT.obj
<<

     dllrname .\RMCOMPPK.EXE cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i 
