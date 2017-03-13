# \base\mak\cksmcfr.mak
# Created by IBM WorkFrame/2 MakeMake at 11:48:08 on 09/30/98
#
# The actions included in this make file are:
#  Compile
#  Link

.SUFFIXES:

.all: \
    e:\base\cksmcfr.exe

.\CKSMCFR.obj: \
    e:\base\rm2\src\cksm\CKSMCFR.CPP
    @echo " Compile "
    icc.exe /Tx /Gm /Fo".\%|fF.obj" /C e:\base\rm2\src\cksm\CKSMCFR.CPP

e:\base\cksmcfr.exe: \
    .\CKSMCFR.obj \
    {$(LIB)}cppwm35.lib
    @echo " Link "
    icc.exe @<<
    /Tdp 
     /Tx 
     /Gm 
      /B" /nodebug /nobrowse /pmtype:pm /code:RX /data:RW"
     /B" /def"
     /B" /def:cppwm35.lib"
     /Fee:\base\cksmcfr.exe 
     cppwm35.lib 
     .\CKSMCFR.obj
<<
