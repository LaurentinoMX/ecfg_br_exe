# \base\mak\rmcomppk.mak
# Created by IBM WorkFrame/2 MakeMake at 15:45:13 on 05/28/97
#
# The actions included in this make file are:
#  Compile
#  Link

.SUFFIXES:

.all: \
    D:\base\rmcomppk.exe

.\PKFEATL.obj: \
    D:\base\rm2\src\ppk\PKFEATL.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euoutil.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}pkfeatl.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euomsg.h \
    {D:\base\rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}pkfeat.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}parr.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\PKFEATL.CPP

.\PKFEAT.obj: \
    D:\base\rm2\src\ppk\PKFEAT.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euoutil.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}pkfeat.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\PKFEAT.CPP

.\PARRREC.obj: \
    D:\base\rm2\src\ppk\PARRREC.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\PARRREC.CPP

.\PARR.obj: \
    D:\base\rm2\src\ppk\PARR.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\PARR.CPP

.\RMCOMPPK.obj: \
    D:\base\rm2\src\ppk\RMCOMPPK.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}pkfeatl.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}pkfeat.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}parr.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euoutil.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euodef.h \
    {D:\base\rm2\src\ppk;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\RMCOMPPK.CPP

.\EUOUTIL.obj: \
    D:\base\rm2\src\ppk\EUOUTIL.CPP \
    {D:\base\rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {D:\base\rm2\src\ppk;$(INCLUDE);}euoutil.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C D:\base\rm2\src\ppk\EUOUTIL.CPP

D:\base\rmcomppk.exe: \
    .\PKFEATL.obj \
    .\EUOUTIL.obj \
    .\RMCOMPPK.obj \
    .\PARR.obj \
    .\PARRREC.obj \
    .\PKFEAT.obj \
    {$(LIB)}cppwm35.lib \
    {$(LIB)}ALCTLS.lib \
    {$(LIB)}ICEXT.lib
    @echo " Link "
    icc.exe @<<
    /Tdp 
     /Tx 
     /Gm 
      /B" /nodebug /nobrowse /pmtype:pm /code:RX /data:RW"
     /B" /def"
     /B" /def:cppwm35.lib"
     /B" /def:ALCTLS.lib"
     /B" /def:ICEXT.lib"
     /FeD:\base\rmcomppk.exe 
     cppwm35.lib 
     ALCTLS.lib 
     ICEXT.lib 
     .\PKFEATL.obj
     .\EUOUTIL.obj
     .\RMCOMPPK.obj
     .\PARR.obj
     .\PARRREC.obj
     .\PKFEAT.obj
<<
