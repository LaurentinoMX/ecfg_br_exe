# \mak\bf9exe.mak  Created by EDW on 09/01/99 using ...
# \mak\bf9bret.mak Created by IBM WorkFrame/2 MakeMake at 08:46:47 on 08/11/99
#
# The actions included in this make file are:
#  Compile
#  Resource Compile
#  Link

PRD = .
INC = INC
RM2INC = RM2\INC
SRC = SRC
RM2SRC = RM2\SRC
RM2PPK = RM2\SRC\PPK
RM2ICEXT = RM2\SRC\RM2ICEXT
OBJ = OBJ
MAK = MAK
MRI = MRI
DEBUGC = Ti-
DEBUGL = nodebug
#DEBUGC = Ti
#DEBUGL = debug

.SUFFIXES: .RC .res

.all: \
    $(PRD)\BF9BRET.EXE

.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(INC)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(MRI)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(SRC)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(RM2INC)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(RM2SRC)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(RM2PPK)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{$(RM2ICEXT)}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

$(OBJ)\SZNOTEBK.obj: \
    $(RM2ICEXT)\SZNOTEBK.CPP \
    {$(RM2ICEXT);$(INCLUDE);}sznotebk.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\SZNOTEBK.CPP

$(OBJ)\SELENTRY.obj: \
    $(RM2ICEXT)\SELENTRY.CPP \
    {$(RM2ICEXT);$(INCLUDE);}selentry.hpp \
    {$(RM2ICEXT);$(INCLUDE);}keyhndlr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\SELENTRY.CPP

$(OBJ)\POPMSHDR.obj: \
    $(RM2ICEXT)\POPMSHDR.CPP \
    {$(RM2ICEXT);$(INCLUDE);}popmshdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\POPMSHDR.CPP

$(OBJ)\KEYHNDLR.obj: \
    $(RM2ICEXT)\KEYHNDLR.CPP \
    {$(RM2ICEXT);$(INCLUDE);}keyhndlr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\KEYHNDLR.CPP

$(OBJ)\IMMVEHDR.obj: \
    $(RM2ICEXT)\IMMVEHDR.CPP \
    {$(RM2ICEXT);$(INCLUDE);}immvehdr.hpp \
    {$(RM2ICEXT);$(INCLUDE);}immveevt.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\IMMVEHDR.CPP

$(OBJ)\IMMVEEVT.obj: \
    $(RM2ICEXT)\IMMVEEVT.CPP \
    {$(RM2ICEXT);$(INCLUDE);}immveevt.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\IMMVEEVT.CPP

$(OBJ)\ICTRLHDR.obj: \
    $(RM2ICEXT)\ICTRLHDR.CPP \
    {$(RM2ICEXT);$(INCLUDE);}ictrlhdr.hpp \
    {$(RM2ICEXT);$(INCLUDE);}ictrlevt.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\ICTRLHDR.CPP

$(OBJ)\ICTRLEVT.obj: \
    $(RM2ICEXT)\ICTRLEVT.CPP \
    {$(RM2ICEXT);$(INCLUDE);}ictrlevt.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\ICTRLEVT.CPP

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
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PKFEATL.CPP

$(OBJ)\PKFEAT.obj: \
    $(RM2PPK)\PKFEAT.CPP \
    {$(RM2PPK);$(INCLUDE);}euoutil.hpp \
    {$(RM2PPK);$(INCLUDE);}pkfeat.hpp \
    {$(RM2PPK);$(INCLUDE);}uicl.hpp \
    {$(RM2PPK);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PKFEAT.CPP

$(OBJ)\PARRREC.obj: \
    $(RM2PPK)\PARRREC.CPP \
    {$(RM2PPK);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PARRREC.CPP

$(OBJ)\PARR.obj: \
    $(RM2PPK)\PARR.CPP \
    {$(RM2PPK);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\PARR.CPP

$(OBJ)\EUOUTIL.obj: \
    $(RM2PPK)\EUOUTIL.CPP \
    {$(RM2PPK);$(INCLUDE);}uicl.hpp \
    {$(RM2PPK);$(INCLUDE);}euoutil.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2PPK)\EUOUTIL.CPP

$(OBJ)\XPLNW.obj: \
    $(RM2SRC)\XPLNW.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}xplnw.h \
    {$(RM2SRC);$(INCLUDE);}xplnw.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}mytrkhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}xplncntl.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\XPLNW.CPP

$(OBJ)\XPLNCNTL.obj: \
    $(RM2SRC)\XPLNCNTL.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}xplncntl.h \
    {$(RM2SRC);$(INCLUDE);}xplncntl.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\XPLNCNTL.CPP

$(OBJ)\XPLN2L.obj: \
    $(RM2SRC)\XPLN2L.CPP \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\XPLN2L.CPP

$(OBJ)\WW.obj: \
    $(RM2SRC)\WW.CPP \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\WW.CPP

$(OBJ)\SYSTEM.obj: \
    $(RM2SRC)\SYSTEM.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr08.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr19_50.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr95.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}rpodata.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}AF9APPLY.H \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\SYSTEM.CPP

$(OBJ)\SWDW.obj: \
    $(RM2SRC)\SWDW.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myfcshdr.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}swdw.h \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}swdw.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\SWDW.CPP

$(OBJ)\RPOINFO.obj: \
    $(RM2SRC)\RPOINFO.CPP \
    {$(RM2SRC);$(INCLUDE);}af9apply.h \
    {$(RM2SRC);$(INCLUDE);}rpoinfo.hpp \
    {$(RM2SRC);$(INCLUDE);}rpoinfo.h \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}rpodata.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\RPOINFO.CPP

$(OBJ)\RPODATA.obj: \
    $(RM2SRC)\RPODATA.CPP \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}rpodata.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\RPODATA.CPP

$(OBJ)\RPO2WC.obj: \
    $(RM2SRC)\RPO2WC.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2wc.h \
    {$(RM2SRC);$(INCLUDE);}rpo2wc.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mytrkhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myfcshdr.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\RPO2WC.CPP

$(OBJ)\RPO2L.obj: \
    $(RM2SRC)\RPO2L.CPP \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\RPO2L.CPP

$(OBJ)\PRFW4.obj: \
    $(RM2SRC)\PRFW4.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}prfw4.h \
    {$(RM2SRC);$(INCLUDE);}prfw4.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}msgthrd.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\PRFW4.CPP

$(OBJ)\PRF.obj: \
    $(RM2SRC)\PRF.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.h \
    {$(RM2SRC);$(INCLUDE);}prf.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\PRF.CPP

$(OBJ)\RPO2.obj: \
    $(RM2SRC)\RPO2.CPP \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\RPO2.CPP

$(OBJ)\PCOL6.obj: \
    $(RM2SRC)\PCOL6.CPP \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\PCOL6.CPP

$(OBJ)\PARRSTR.obj: \
    $(RM2SRC)\PARRSTR.CPP \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\PARRSTR.CPP

$(OBJ)\MYXCPHDR.obj: \
    $(RM2SRC)\MYXCPHDR.CPP \
    {$(RM2SRC);$(INCLUDE);}itrace.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MYXCPHDR.CPP

$(OBJ)\MYTRKHDR.obj: \
    $(RM2SRC)\MYTRKHDR.CPP \
    {$(RM2SRC);$(INCLUDE);}mytrkhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MYTRKHDR.CPP

$(OBJ)\MYTIMERS.obj: \
    $(RM2SRC)\MYTIMERS.CPP \
    {$(RM2SRC);$(INCLUDE);}mytimers.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MYTIMERS.CPP

$(OBJ)\MYPTRHDR.obj: \
    $(RM2SRC)\MYPTRHDR.CPP \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MYPTRHDR.CPP

$(OBJ)\MYFCSHDR.obj: \
    $(RM2SRC)\MYFCSHDR.CPP \
    {$(RM2SRC);$(INCLUDE);}myfcshdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MYFCSHDR.CPP

$(OBJ)\MSGTHRD.obj: \
    $(RM2SRC)\MSGTHRD.CPP \
    {$(RM2SRC);$(INCLUDE);}msgthrd.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MSGTHRD.CPP

$(OBJ)\MMW2.obj: \
    $(RM2SRC)\MMW2.CPP \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}mmw2.hpp \
    {$(RM2SRC);$(INCLUDE);}mmw2.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MMW2.CPP

$(OBJ)\MINMXHDR.obj: \
    $(RM2SRC)\MINMXHDR.CPP \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MINMXHDR.CPP

$(OBJ)\MFEATL.obj: \
    $(RM2SRC)\MFEATL.CPP \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MFEATL.CPP

$(OBJ)\MFEAT.obj: \
    $(RM2SRC)\MFEAT.CPP \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MFEAT.CPP

$(OBJ)\MAINW.obj: \
    $(RM2SRC)\MAINW.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}prfw4.h \
    {$(RM2SRC);$(INCLUDE);}prfw4.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}ialctls.hpp \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}euogmain.hpp \
    {$(RM2SRC);$(INCLUDE);}euogmain.h \
    {$(RM2SRC);$(INCLUDE);}swdw.hpp \
    {$(RM2SRC);$(INCLUDE);}pprvw.hpp \
    {$(RM2SRC);$(INCLUDE);}pprvw.h \
    {$(RM2SRC);$(INCLUDE);}rpo2wc.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2wc.h \
    {$(RM2SRC);$(INCLUDE);}rpoinfo.h \
    {$(RM2SRC);$(INCLUDE);}rpoinfo.hpp \
    {$(RM2SRC);$(INCLUDE);}rpodata.hpp \
    {$(RM2SRC);$(INCLUDE);}bklogw.hpp \
    {$(RM2SRC);$(INCLUDE);}bklogw.h \
    {$(RM2SRC);$(INCLUDE);}mmw2.hpp \
    {$(RM2SRC);$(INCLUDE);}mmw2.h \
    {$(RM2SRC);$(INCLUDE);}xplnw.hpp \
    {$(RM2SRC);$(INCLUDE);}xplnw.h \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}alvfile.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}af9compl.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}ub9host.h \
    {$(RM2SRC);$(INCLUDE);}af9apply.h \
    {$(RM2SRC);$(INCLUDE);}msgthrd.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}swdw.h \
    {$(RM2SRC);$(INCLUDE);}mytrkhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myfcshdr.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}xplncntl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MAINW.CPP

$(OBJ)\MACHL.obj: \
    $(RM2SRC)\MACHL.CPP \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MACHL.CPP

$(OBJ)\MACH.obj: \
    $(RM2SRC)\MACH.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr08.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr95.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\MACH.CPP

$(OBJ)\IPRINTER.obj: \
    $(RM2SRC)\IPRINTER.CPP \
    {$(RM2SRC);$(INCLUDE);}iprinter.h \
    {$(RM2SRC);$(INCLUDE);}iprinter.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\IPRINTER.CPP

$(OBJ)\IALCTLS.obj: \
    $(RM2SRC)\IALCTLS.CPP \
    {$(RM2SRC);$(INCLUDE);}ialctls.hpp \
    {$(RM2SRC);$(INCLUDE);}alctls.h \
    {$(RM2SRC);$(INCLUDE);}istattxt.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\IALCTLS.CPP

$(OBJ)\FEATL.obj: \
    $(RM2SRC)\FEATL.CPP \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\FEATL.CPP

$(OBJ)\EUOPRES.obj: \
    $(RM2SRC)\EUOPRES.CPP \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}AF9APPLY.H \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\EUOPRES.CPP

$(OBJ)\EUOPIPRT.obj: \
    $(RM2SRC)\EUOPIPRT.CPP \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}iprinter.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\EUOPIPRT.CPP

$(OBJ)\PPRVW.obj: \
    $(RM2SRC)\PPRVW.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}pprvw.hpp \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}pprvw.h \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}ialctls.hpp \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}AF9APPLY.H \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\PPRVW.CPP

$(OBJ)\CFRV.obj: \
    $(RM2SRC)\CFRV.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFRV.CPP

$(OBJ)\CFRSW.obj: \
    $(RM2SRC)\CFRSW.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrfq.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFRSW.CPP

$(OBJ)\CFRFQ.obj: \
    $(RM2SRC)\CFRFQ.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrfq.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFRFQ.CPP

$(OBJ)\CFREC.obj: \
    $(RM2SRC)\CFREC.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFREC.CPP

$(OBJ)\CFR96.obj: \
    $(RM2SRC)\CFR96.CPP \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR96.CPP

$(OBJ)\CFR95.obj: \
    $(RM2SRC)\CFR95.CPP \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr95.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR95.CPP

$(OBJ)\CFR54.obj: \
    $(RM2SRC)\CFR54.CPP \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr95.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR54.CPP

$(OBJ)\CFR19_50.obj: \
    $(RM2SRC)\CFR19_50.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrfq.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr19_50.hpp \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR19_50.CPP

$(OBJ)\CFR10.obj: \
    $(RM2SRC)\CFR10.CPP \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR10.CPP

$(OBJ)\CFR08.obj: \
    $(RM2SRC)\CFR08.CPP \
    {$(RM2SRC);$(INCLUDE);}cfrfq.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr08.hpp \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR08.CPP

$(OBJ)\CFR05.obj: \
    $(RM2SRC)\CFR05.CPP \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\CFR05.CPP

$(OBJ)\BLFEATL.obj: \
    $(RM2SRC)\BLFEATL.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr19_50.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.h \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\BLFEATL.CPP

$(OBJ)\BKLOGW.obj: \
    $(RM2SRC)\BKLOGW.CPP \
    {$(RM2SRC);$(INCLUDE);}bklogw.hpp \
    {$(RM2SRC);$(INCLUDE);}bklogw.h \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}AF9APPLY.H \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\BKLOGW.CPP

$(OBJ)\BASEW.obj: \
    $(RM2SRC)\BASEW.CPP \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\BASEW.CPP

$(OBJ)\ALVFILE.obj: \
    $(RM2SRC)\ALVFILE.CPP \
    {$(RM2SRC);$(INCLUDE);}alvfile.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\ALVFILE.CPP

$(OBJ)\EUOGMAIN.obj: \
    $(RM2SRC)\EUOGMAIN.CPP \
    {$(RM2SRC);$(INCLUDE);}euogmain.hpp \
    {$(RM2SRC);$(INCLUDE);}euophlp.h \
    {$(RM2SRC);$(INCLUDE);}euogmain.h \
    {$(RM2SRC);$(INCLUDE);}xplnw.h \
    {$(RM2SRC);$(INCLUDE);}euomsg.h \
    {$(RM2SRC);$(INCLUDE);}euopids.h \
    {$(RM2SRC);$(INCLUDE);}uicl.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2.hpp \
    {$(RM2SRC);$(INCLUDE);}xpln2l.hpp \
    {$(RM2SRC);$(INCLUDE);}xplnw.hpp \
    {$(RM2SRC);$(INCLUDE);}prf.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2l.hpp \
    {$(RM2SRC);$(INCLUDE);}system.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}pkfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}feat.hpp \
    {$(RM2SRC);$(INCLUDE);}featl.hpp \
    {$(RM2SRC);$(INCLUDE);}mainw.hpp \
    {$(RM2SRC);$(INCLUDE);}basew.hpp \
    {$(RM2SRC);$(INCLUDE);}mytrkhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}xplncntl.hpp \
    {$(RM2SRC);$(INCLUDE);}rpo2.hpp \
    {$(RM2SRC);$(INCLUDE);}machl.hpp \
    {$(RM2SRC);$(INCLUDE);}mfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}blfeatl.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrsw.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr10.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr05.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrv.hpp \
    {$(RM2SRC);$(INCLUDE);}pcol6.hpp \
    {$(RM2SRC);$(INCLUDE);}parr.hpp \
    {$(RM2SRC);$(INCLUDE);}euoutil.hpp \
    {$(RM2SRC);$(INCLUDE);}euodef.h \
    {$(RM2SRC);$(INCLUDE);}mfeat.hpp \
    {$(RM2SRC);$(INCLUDE);}mach.hpp \
    {$(RM2SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(RM2SRC);$(INCLUDE);}ww.hpp \
    {$(RM2SRC);$(INCLUDE);}euopres.hpp \
    {$(RM2SRC);$(INCLUDE);}euopiprt.hpp \
    {$(RM2SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}af9main.hpp \
    {$(RM2SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(RM2SRC);$(INCLUDE);}cfrec.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr54.hpp \
    {$(RM2SRC);$(INCLUDE);}cfr96.hpp \
    {$(RM2SRC);$(INCLUDE);}afccpuls.hpp \
    {$(RM2SRC);$(INCLUDE);}AF9APPLY.H \
    {$(RM2SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2SRC)\EUOGMAIN.CPP

$(OBJ)\AFCV2031.obj: \
    $(SRC)\AFCV2031.CPP \
    {$(SRC);$(INCLUDE);}afcv2031.hpp \
    {$(SRC);$(INCLUDE);}afcv2031.h \
    {$(SRC);$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCV2031.CPP

$(OBJ)\AFCSYS.obj: \
    $(SRC)\AFCSYS.CPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCSYS.CPP

$(OBJ)\AFCSAVE.obj: \
    $(SRC)\AFCSAVE.CPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCMCROS.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9ORDRS.HPP \
    {$(SRC);$(INCLUDE);}AF9SYSTM.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCSAVE.CPP

$(OBJ)\AFCPROD.obj: \
    $(SRC)\AFCPROD.CPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCPROD.CPP

$(OBJ)\AFCPARSE.obj: \
    $(SRC)\AFCPARSE.CPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCMCROS.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCPARSE.CPP

$(OBJ)\AFCMES.obj: \
    $(SRC)\AFCMES.CPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCMES.CPP

$(OBJ)\AFCLSTFL.obj: \
    $(SRC)\AFCLSTFL.CPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCLSTFL.CPP

$(OBJ)\AFCLOAD.obj: \
    $(SRC)\AFCLOAD.CPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCMCROS.H \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9ORDRS.HPP \
    {$(SRC);$(INCLUDE);}AF9SYSTM.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCLOAD.CPP

$(OBJ)\AFCGETFL.obj: \
    $(SRC)\AFCGETFL.CPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCGETFL.CPP

$(OBJ)\AFCGENCF.obj: \
    $(SRC)\AFCGENCF.CPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCMCROS.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9ORDRS.HPP \
    {$(SRC);$(INCLUDE);}AF9SYSTM.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCGENCF.CPP

$(OBJ)\AFCCPULS.obj: \
    $(SRC)\AFCCPULS.CPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCCPULS.CPP

$(OBJ)\AFCCNVRT.obj: \
    $(SRC)\AFCCNVRT.CPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCCNVRT.CPP

$(OBJ)\AFCCFRST.obj: \
    $(SRC)\AFCCFRST.CPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AF9APPLY.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCCFRST.CPP

$(OBJ)\AFCBKEND.obj: \
    $(SRC)\AFCBKEND.CPP \
    {$(SRC);$(INCLUDE);}AFCBKEND.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9ORDRS.HPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCBKEND.CPP

$(OBJ)\AFCABPRD.obj: \
    $(SRC)\AFCABPRD.CPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AF9APPLY.H \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AFCABPRD.CPP

$(OBJ)\AF9SYSTM.obj: \
    $(SRC)\AF9SYSTM.CPP \
    {$(SRC);$(INCLUDE);}af9systm.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9sysed.hpp \
    {$(SRC);$(INCLUDE);}afcprods.hpp \
    {$(SRC);$(INCLUDE);}afcbkend.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9SYSTM.CPP

$(OBJ)\AF9SYSED.obj: \
    $(SRC)\AF9SYSED.CPP \
    {$(SRC);$(INCLUDE);}af9sysed.hpp \
    {$(SRC);$(INCLUDE);}af9systm.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9SYSED.CPP

$(OBJ)\AF9SELDT.obj: \
    $(SRC)\AF9SELDT.CPP \
    {$(SRC);$(INCLUDE);}af9seldt.hpp \
    {$(SRC);$(INCLUDE);}af9ordrs.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9SELDT.CPP

$(OBJ)\AF9RTSRC.obj: \
    $(SRC)\AF9RTSRC.CPP \
    {$(SRC);$(INCLUDE);}af9rtsrc.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}ub9host.h \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9RTSRC.CPP

$(OBJ)\AF9RECON.obj: \
    $(SRC)\AF9RECON.CPP \
    {$(SRC);$(INCLUDE);}af9recon.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9RECON.CPP

$(OBJ)\AF9PTION.obj: \
    $(SRC)\AF9PTION.CPP \
    {$(SRC);$(INCLUDE);}af9ption.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9PTION.CPP

$(OBJ)\AF9PROFL.obj: \
    $(SRC)\AF9PROFL.CPP \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9ordrs.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9PROFL.CPP

$(OBJ)\AF9ORDRS.obj: \
    $(SRC)\AF9ORDRS.CPP \
    {$(SRC);$(INCLUDE);}af9ordrs.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9systm.hpp \
    {$(SRC);$(INCLUDE);}af9seldt.hpp \
    {$(SRC);$(INCLUDE);}afcprods.hpp \
    {$(SRC);$(INCLUDE);}afcbkend.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9ORDRS.CPP

$(OBJ)\AF9NAME.obj: \
    $(SRC)\AF9NAME.CPP \
    {$(SRC);$(INCLUDE);}af9name.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}afcbkend.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9NAME.CPP

$(OBJ)\AF9MAIN.obj: \
    $(SRC)\AF9MAIN.CPP \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}afcbkend.hpp \
    {$(SRC);$(INCLUDE);}af9ordrs.hpp \
    {$(SRC);$(INCLUDE);}af9systm.hpp \
    {$(SRC);$(INCLUDE);}af9detl.hpp \
    {$(SRC);$(INCLUDE);}af9name.hpp \
    {$(SRC);$(INCLUDE);}af9ption.hpp \
    {$(SRC);$(INCLUDE);}af9local.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}af9rtsrc.hpp \
    {$(SRC);$(INCLUDE);}af9recon.hpp \
    {$(SRC);$(INCLUDE);}afcv2031.hpp \
    {$(SRC);$(INCLUDE);}ub9host.h \
    {$(SRC);$(INCLUDE);}mainw.hpp \
    {$(SRC);$(INCLUDE);}mainw.h \
    {$(SRC);$(INCLUDE);}prf.hpp \
    {$(SRC);$(INCLUDE);}prfw4.hpp \
    {$(SRC);$(INCLUDE);}bridge.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP \
    {$(SRC);$(INCLUDE);}AFCPRODS.HPP \
    {$(SRC);$(INCLUDE);}uicl.hpp \
    {$(SRC);$(INCLUDE);}basew.hpp \
    {$(SRC);$(INCLUDE);}myxcphdr.hpp \
    {$(SRC);$(INCLUDE);}ww.hpp \
    {$(SRC);$(INCLUDE);}euopres.hpp \
    {$(SRC);$(INCLUDE);}euopiprt.hpp \
    {$(SRC);$(INCLUDE);}minmxhdr.hpp \
    {$(SRC);$(INCLUDE);}msgthrd.hpp \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}myptrhdr.hpp \
    {$(SRC);$(INCLUDE);}euoutil.hpp \
    {$(SRC);$(INCLUDE);}myusrhdr.hpp \
    {$(SRC);$(INCLUDE);}system.hpp \
    {$(SRC);$(INCLUDE);}machl.hpp \
    {$(SRC);$(INCLUDE);}pkfeatl.hpp \
    {$(SRC);$(INCLUDE);}featl.hpp \
    {$(SRC);$(INCLUDE);}mfeatl.hpp \
    {$(SRC);$(INCLUDE);}blfeatl.hpp \
    {$(SRC);$(INCLUDE);}cfrsw.hpp \
    {$(SRC);$(INCLUDE);}cfr10.hpp \
    {$(SRC);$(INCLUDE);}cfr05.hpp \
    {$(SRC);$(INCLUDE);}xpln2l.hpp \
    {$(SRC);$(INCLUDE);}rpo2l.hpp \
    {$(SRC);$(INCLUDE);}cfrv.hpp \
    {$(SRC);$(INCLUDE);}pcol6.hpp \
    {$(SRC);$(INCLUDE);}parr.hpp \
    {$(SRC);$(INCLUDE);}mach.hpp \
    {$(SRC);$(INCLUDE);}pkfeat.hpp \
    {$(SRC);$(INCLUDE);}feat.hpp \
    {$(SRC);$(INCLUDE);}mfeat.hpp \
    {$(SRC);$(INCLUDE);}euodef.h \
    {$(SRC);$(INCLUDE);}xpln2.hpp \
    {$(SRC);$(INCLUDE);}rpo2.hpp \
    {$(SRC);$(INCLUDE);}cfrec.hpp \
    {$(SRC);$(INCLUDE);}cfr54.hpp \
    {$(SRC);$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9MAIN.CPP

$(OBJ)\AF9LOCAL.obj: \
    $(SRC)\AF9LOCAL.CPP \
    {$(SRC);$(INCLUDE);}af9local.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9LOCAL.CPP

$(OBJ)\AF9DTOBJ.obj: \
    $(SRC)\AF9DTOBJ.CPP \
    {$(SRC);$(INCLUDE);}af9dtobj.hpp \
    {$(SRC);$(INCLUDE);}afcprods.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}AF9MAIN.HPP \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCCPULS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9DTOBJ.CPP

$(OBJ)\AF9DETL.obj: \
    $(SRC)\AF9DETL.CPP \
    {$(SRC);$(INCLUDE);}af9detl.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9dtobj.hpp \
    {$(SRC);$(INCLUDE);}afcprods.hpp \
    {$(SRC);$(INCLUDE);}afcbkend.hpp \
    {$(SRC);$(INCLUDE);}af9profl.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H \
    {$(SRC);$(INCLUDE);}AFCSYS.HPP \
    {$(SRC);$(INCLUDE);}AFCFEAT.HPP \
    {$(SRC);$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9DETL.CPP

$(OBJ)\AF9COMPL.obj: \
    $(SRC)\AF9COMPL.CPP \
    {$(SRC);$(INCLUDE);}af9compl.hpp \
    {$(SRC);$(INCLUDE);}af9main.hpp \
    {$(SRC);$(INCLUDE);}af9apply.h \
    {$(SRC);$(INCLUDE);}afccpuls.hpp \
    {$(SRC);$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(SRC)\AF9COMPL.CPP

$(OBJ)\USERHDR.obj: \
    $(RM2ICEXT)\USERHDR.CPP \
    {$(RM2ICEXT);$(INCLUDE);}userhdr.hpp
    @echo " Compile "
    icc.exe /DNO_STRICT /Tx /Gm /$(DEBUGC) /Gd /Fo"$(OBJ)\%|fF.obj" /C $(RM2ICEXT)\USERHDR.CPP

$(PRD)\BF9BRET.EXE: \
    $(OBJ)\SZNOTEBK.obj \
    $(SRC)\af9apply.res \
    $(OBJ)\USERHDR.obj \
    $(OBJ)\AF9COMPL.obj \
    $(OBJ)\AF9DETL.obj \
    $(OBJ)\AF9DTOBJ.obj \
    $(OBJ)\AF9LOCAL.obj \
    $(OBJ)\AF9MAIN.obj \
    $(OBJ)\AF9NAME.obj \
    $(OBJ)\AF9ORDRS.obj \
    $(OBJ)\AF9PROFL.obj \
    $(OBJ)\AF9PTION.obj \
    $(OBJ)\AF9RECON.obj \
    $(OBJ)\AF9RTSRC.obj \
    $(OBJ)\AF9SELDT.obj \
    $(OBJ)\AF9SYSED.obj \
    $(OBJ)\AF9SYSTM.obj \
    $(OBJ)\AFCABPRD.obj \
    $(OBJ)\AFCBKEND.obj \
    $(OBJ)\AFCCFRST.obj \
    $(OBJ)\AFCCNVRT.obj \
    $(OBJ)\AFCCPULS.obj \
    $(OBJ)\AFCGENCF.obj \
    $(OBJ)\AFCGETFL.obj \
    $(OBJ)\AFCLOAD.obj \
    $(OBJ)\AFCLSTFL.obj \
    $(OBJ)\AFCMES.obj \
    $(OBJ)\AFCPARSE.obj \
    $(OBJ)\AFCPROD.obj \
    $(OBJ)\AFCSAVE.obj \
    $(OBJ)\AFCSYS.obj \
    $(OBJ)\AFCV2031.obj \
    $(OBJ)\EUOGMAIN.obj \
    $(OBJ)\ALVFILE.obj \
    $(OBJ)\BASEW.obj \
    $(OBJ)\BKLOGW.obj \
    $(OBJ)\BLFEATL.obj \
    $(OBJ)\CFR05.obj \
    $(OBJ)\CFR08.obj \
    $(OBJ)\CFR10.obj \
    $(OBJ)\CFR19_50.obj \
    $(OBJ)\CFR54.obj \
    $(OBJ)\CFR95.obj \
    $(OBJ)\CFR96.obj \
    $(OBJ)\CFREC.obj \
    $(OBJ)\CFRFQ.obj \
    $(OBJ)\CFRSW.obj \
    $(OBJ)\CFRV.obj \
    $(OBJ)\PPRVW.obj \
    $(OBJ)\EUOPIPRT.obj \
    $(OBJ)\EUOPRES.obj \
    $(OBJ)\FEATL.obj \
    $(OBJ)\IALCTLS.obj \
    $(OBJ)\IPRINTER.obj \
    $(OBJ)\MACH.obj \
    $(OBJ)\MACHL.obj \
    $(OBJ)\MAINW.obj \
    $(OBJ)\MFEAT.obj \
    $(OBJ)\MFEATL.obj \
    $(OBJ)\MINMXHDR.obj \
    $(OBJ)\MMW2.obj \
    $(OBJ)\MSGTHRD.obj \
    $(OBJ)\MYFCSHDR.obj \
    $(OBJ)\MYPTRHDR.obj \
    $(OBJ)\MYTIMERS.obj \
    $(OBJ)\MYTRKHDR.obj \
    $(OBJ)\MYXCPHDR.obj \
    $(OBJ)\PARRSTR.obj \
    $(OBJ)\PCOL6.obj \
    $(OBJ)\RPO2.obj \
    $(OBJ)\PRF.obj \
    $(OBJ)\PRFW4.obj \
    $(OBJ)\RPO2L.obj \
    $(OBJ)\RPO2WC.obj \
    $(OBJ)\RPODATA.obj \
    $(OBJ)\RPOINFO.obj \
    $(OBJ)\SWDW.obj \
    $(OBJ)\SYSTEM.obj \
    $(OBJ)\WW.obj \
    $(OBJ)\XPLN2L.obj \
    $(OBJ)\XPLNCNTL.obj \
    $(OBJ)\XPLNW.obj \
    $(OBJ)\EUOUTIL.obj \
    $(OBJ)\PARR.obj \
    $(OBJ)\PARRREC.obj \
    $(OBJ)\PKFEAT.obj \
    $(OBJ)\PKFEATL.obj \
    $(OBJ)\ICTRLEVT.obj \
    $(OBJ)\ICTRLHDR.obj \
    $(OBJ)\IMMVEEVT.obj \
    $(OBJ)\IMMVEHDR.obj \
    $(OBJ)\KEYHNDLR.obj \
    $(OBJ)\POPMSHDR.obj \
    $(OBJ)\SELENTRY.obj \
    {$(LIB)}advapi32.lib \
    {$(LIB)}cppwm35.lib \
    {$(LIB)}user32.lib \
    {$(LIB)}gdi32.lib \
    {$(LIB)}comdlg32.lib
    @echo " Link "
    icc.exe @<<
    /Tdp  /DNO_STRICT 
     /Tx
     /$(DEBUGC) /Gm /Gd
      /B" /$(DEBUGL) /nobrowse /pmtype:pm /code:RX /data:RW"
     /B" /def"
     /B" /def:advapi32.lib"
     /B" /def:cppwm35.lib"
     /B" /def:user32.lib"
     /B" /def:gdi32.lib"
     /B" /def:comdlg32.lib"
     /Fe$(PRD)\BF9BRET.EXE
     advapi32.lib
     cppwm35.lib
     user32.lib
     gdi32.lib
     comdlg32.lib
     $(OBJ)\SZNOTEBK.obj
     $(SRC)\af9apply.res
     $(OBJ)\USERHDR.obj
     $(OBJ)\AF9COMPL.obj
     $(OBJ)\AF9DETL.obj
     $(OBJ)\AF9DTOBJ.obj
     $(OBJ)\AF9LOCAL.obj
     $(OBJ)\AF9MAIN.obj
     $(OBJ)\AF9NAME.obj
     $(OBJ)\AF9ORDRS.obj
     $(OBJ)\AF9PROFL.obj
     $(OBJ)\AF9PTION.obj
     $(OBJ)\AF9RECON.obj
     $(OBJ)\AF9RTSRC.obj
     $(OBJ)\AF9SELDT.obj
     $(OBJ)\AF9SYSED.obj
     $(OBJ)\AF9SYSTM.obj
     $(OBJ)\AFCABPRD.obj
     $(OBJ)\AFCBKEND.obj
     $(OBJ)\AFCCFRST.obj
     $(OBJ)\AFCCNVRT.obj
     $(OBJ)\AFCCPULS.obj
     $(OBJ)\AFCGENCF.obj
     $(OBJ)\AFCGETFL.obj
     $(OBJ)\AFCLOAD.obj
     $(OBJ)\AFCLSTFL.obj
     $(OBJ)\AFCMES.obj
     $(OBJ)\AFCPARSE.obj
     $(OBJ)\AFCPROD.obj
     $(OBJ)\AFCSAVE.obj
     $(OBJ)\AFCSYS.obj
     $(OBJ)\AFCV2031.obj
     $(OBJ)\EUOGMAIN.obj
     $(OBJ)\ALVFILE.obj
     $(OBJ)\BASEW.obj
     $(OBJ)\BKLOGW.obj
     $(OBJ)\BLFEATL.obj
     $(OBJ)\CFR05.obj
     $(OBJ)\CFR08.obj
     $(OBJ)\CFR10.obj
     $(OBJ)\CFR19_50.obj
     $(OBJ)\CFR54.obj
     $(OBJ)\CFR95.obj
     $(OBJ)\CFR96.obj
     $(OBJ)\CFREC.obj
     $(OBJ)\CFRFQ.obj
     $(OBJ)\CFRSW.obj
     $(OBJ)\CFRV.obj
     $(OBJ)\PPRVW.obj
     $(OBJ)\EUOPIPRT.obj
     $(OBJ)\EUOPRES.obj
     $(OBJ)\FEATL.obj
     $(OBJ)\IALCTLS.obj
     $(OBJ)\IPRINTER.obj
     $(OBJ)\MACH.obj
     $(OBJ)\MACHL.obj
     $(OBJ)\MAINW.obj
     $(OBJ)\MFEAT.obj
     $(OBJ)\MFEATL.obj
     $(OBJ)\MINMXHDR.obj
     $(OBJ)\MMW2.obj
     $(OBJ)\MSGTHRD.obj
     $(OBJ)\MYFCSHDR.obj
     $(OBJ)\MYPTRHDR.obj
     $(OBJ)\MYTIMERS.obj
     $(OBJ)\MYTRKHDR.obj
     $(OBJ)\MYXCPHDR.obj
     $(OBJ)\PARRSTR.obj
     $(OBJ)\PCOL6.obj
     $(OBJ)\RPO2.obj
     $(OBJ)\PRF.obj
     $(OBJ)\PRFW4.obj
     $(OBJ)\RPO2L.obj
     $(OBJ)\RPO2WC.obj
     $(OBJ)\RPODATA.obj
     $(OBJ)\RPOINFO.obj
     $(OBJ)\SWDW.obj
     $(OBJ)\SYSTEM.obj
     $(OBJ)\WW.obj
     $(OBJ)\XPLN2L.obj
     $(OBJ)\XPLNCNTL.obj
     $(OBJ)\XPLNW.obj
     $(OBJ)\EUOUTIL.obj
     $(OBJ)\PARR.obj
     $(OBJ)\PARRREC.obj
     $(OBJ)\PKFEAT.obj
     $(OBJ)\PKFEATL.obj
     $(OBJ)\ICTRLEVT.obj
     $(OBJ)\ICTRLHDR.obj
     $(OBJ)\IMMVEEVT.obj
     $(OBJ)\IMMVEHDR.obj
     $(OBJ)\KEYHNDLR.obj
     $(OBJ)\POPMSHDR.obj
     $(OBJ)\SELENTRY.obj
<<
     dllrname $(PRD)\BF9BRET.EXE cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i 
