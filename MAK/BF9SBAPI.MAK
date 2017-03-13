# Bf9sbapi.mak
# Created by IBM WorkFrame/2 MakeMake at 16:52:08 on 08/12/97
#
# The actions included in this make file are:
#  Compile
#  Make Def file
#  Make exp and lib files
#  Link

.SUFFIXES:

.all: \
    .\Bf9sbapi.dll

.\Bf9sbapi.obj: \
    D:\sb9dll\Bfsrc\Bf9sbapi.c
    @echo " Compile "
    icc.exe /Gm /Gd /Ge- /Gf- /Fo".\%|fF.obj" /C D:\sb9dll\Bfsrc\Bf9sbapi.c

.\Bf9sbapi.def: \
    .\Bf9sbapi.obj
    @echo " Make Def file "
    imakedef.bat Bf9sbapi Bf9sbapi.def .\Bf9sbapi.obj

.\Bf9sbapi.exp: \
    .\Bf9sbapi.def
    @echo " Make exp and lib files "
    ilib.exe /Gi:Bf9sbapi .\Bf9sbapi.def

.\Bf9sbapi.dll: \
    .\Bf9sbapi.exp \
    .\Bf9sbapi.obj \
    {$(LIB)}advapi32.lib
    @echo " Link "
    icc.exe @<<
     /B" /nodebug /nobrowse /pmtype:vio /noe /code:RX /data:RW /dll"
     /B" /def"
     /B" /def:advapi32.lib"
     /B" /nod:Bf9sbapi.lib"
     /FeBf9sbapi.dll 
     advapi32.lib 
     .\Bf9sbapi.exp
     .\Bf9sbapi.obj
<<
