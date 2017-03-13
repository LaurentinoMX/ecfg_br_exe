# e:\base\mak\bf9bret.mak
# Created by IBM WorkFrame/2 MakeMake at 13:08:51 on 01/06/99
#
# The actions included in this make file are:
#  Compile
#  Resource Compile
#  Link

.SUFFIXES: .RC .res 

.all: \
    e:\base\BF9BRET.EXE

.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\INC}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\mri}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\src}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\Rm2\inc}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\Rm2\src}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\Rm2\src\ppk}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

{e:\base\Rm2\src\rm2icext}.RC.res:
    @echo " Resource Compile "
    irc.exe %s

.\SZNOTEBK.obj: \
    e:\base\Rm2\src\rm2icext\SZNOTEBK.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}sznotebk.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\SZNOTEBK.CPP

.\SELENTRY.obj: \
    e:\base\Rm2\src\rm2icext\SELENTRY.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}selentry.hpp \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}keyhndlr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\SELENTRY.CPP

.\POPMSHDR.obj: \
    e:\base\Rm2\src\rm2icext\POPMSHDR.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}popmshdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\POPMSHDR.CPP

.\KEYHNDLR.obj: \
    e:\base\Rm2\src\rm2icext\KEYHNDLR.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}keyhndlr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\KEYHNDLR.CPP

.\IMMVEHDR.obj: \
    e:\base\Rm2\src\rm2icext\IMMVEHDR.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}immvehdr.hpp \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}immveevt.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\IMMVEHDR.CPP

.\IMMVEEVT.obj: \
    e:\base\Rm2\src\rm2icext\IMMVEEVT.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}immveevt.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\IMMVEEVT.CPP

.\ICTRLHDR.obj: \
    e:\base\Rm2\src\rm2icext\ICTRLHDR.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}ictrlhdr.hpp \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}ictrlevt.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\ICTRLHDR.CPP

.\ICTRLEVT.obj: \
    e:\base\Rm2\src\rm2icext\ICTRLEVT.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}ictrlevt.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\ICTRLEVT.CPP

.\PKFEATL.obj: \
    e:\base\Rm2\src\ppk\PKFEATL.CPP \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ppk\PKFEATL.CPP

.\PKFEAT.obj: \
    e:\base\Rm2\src\ppk\PKFEAT.CPP \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ppk\PKFEAT.CPP

.\PARRREC.obj: \
    e:\base\Rm2\src\ppk\PARRREC.CPP \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ppk\PARRREC.CPP

.\PARR.obj: \
    e:\base\Rm2\src\ppk\PARR.CPP \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ppk\PARR.CPP

.\EUOUTIL.obj: \
    e:\base\Rm2\src\ppk\EUOUTIL.CPP \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src\ppk;$(INCLUDE);}euoutil.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ppk\EUOUTIL.CPP

.\XPLNW.obj: \
    e:\base\Rm2\src\XPLNW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.h \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mytrkhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplncntl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\XPLNW.CPP

.\XPLNCNTL.obj: \
    e:\base\Rm2\src\XPLNCNTL.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplncntl.h \
    {e:\base\Rm2\src;$(INCLUDE);}xplncntl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\XPLNCNTL.CPP

.\XPLN2L.obj: \
    e:\base\Rm2\src\XPLN2L.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\XPLN2L.CPP

.\WW.obj: \
    e:\base\Rm2\src\WW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\WW.CPP

.\SYSTEM.obj: \
    e:\base\Rm2\src\SYSTEM.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr08.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr19_50.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr95.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpodata.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\SYSTEM.CPP

.\SWDW.obj: \
    e:\base\Rm2\src\SWDW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myfcshdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}swdw.h \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}swdw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\SWDW.CPP

.\RPOINFO.obj: \
    e:\base\Rm2\src\RPOINFO.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}af9apply.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpoinfo.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpoinfo.h \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpodata.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\RPOINFO.CPP

.\RPODATA.obj: \
    e:\base\Rm2\src\RPODATA.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpodata.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\RPODATA.CPP

.\RPO2WC.obj: \
    e:\base\Rm2\src\RPO2WC.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2wc.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2wc.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mytrkhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myfcshdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\RPO2WC.CPP

.\RPO2L.obj: \
    e:\base\Rm2\src\RPO2L.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\RPO2L.CPP

.\RPO2.obj: \
    e:\base\Rm2\src\RPO2.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\RPO2.CPP

.\PRFW4.obj: \
    e:\base\Rm2\src\PRFW4.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prfw4.h \
    {e:\base\Rm2\src;$(INCLUDE);}prfw4.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}msgthrd.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\PRFW4.CPP

.\PRF.obj: \
    e:\base\Rm2\src\PRF.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.h \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\PRF.CPP

.\PPRVW.obj: \
    e:\base\Rm2\src\PPRVW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pprvw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}pprvw.h \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ialctls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\PPRVW.CPP

.\PCOL6.obj: \
    e:\base\Rm2\src\PCOL6.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\PCOL6.CPP

.\PARRSTR.obj: \
    e:\base\Rm2\src\PARRSTR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\PARRSTR.CPP

.\MYXCPHDR.obj: \
    e:\base\Rm2\src\MYXCPHDR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}itrace.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MYXCPHDR.CPP

.\MYTRKHDR.obj: \
    e:\base\Rm2\src\MYTRKHDR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}mytrkhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MYTRKHDR.CPP

.\MYTIMERS.obj: \
    e:\base\Rm2\src\MYTIMERS.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}mytimers.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MYTIMERS.CPP

.\MYPTRHDR.obj: \
    e:\base\Rm2\src\MYPTRHDR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MYPTRHDR.CPP

.\MYFCSHDR.obj: \
    e:\base\Rm2\src\MYFCSHDR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}myfcshdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MYFCSHDR.CPP

.\MSGTHRD.obj: \
    e:\base\Rm2\src\MSGTHRD.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}msgthrd.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MSGTHRD.CPP

.\MMW2.obj: \
    e:\base\Rm2\src\MMW2.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}mmw2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mmw2.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MMW2.CPP

.\MINMXHDR.obj: \
    e:\base\Rm2\src\MINMXHDR.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MINMXHDR.CPP

.\MFEATL.obj: \
    e:\base\Rm2\src\MFEATL.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MFEATL.CPP

.\MFEAT.obj: \
    e:\base\Rm2\src\MFEAT.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MFEAT.CPP

.\MAINW.obj: \
    e:\base\Rm2\src\MAINW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prfw4.h \
    {e:\base\Rm2\src;$(INCLUDE);}prfw4.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ialctls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}euogmain.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euogmain.h \
    {e:\base\Rm2\src;$(INCLUDE);}swdw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pprvw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pprvw.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2wc.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2wc.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpoinfo.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpoinfo.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpodata.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}bklogw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}bklogw.h \
    {e:\base\Rm2\src;$(INCLUDE);}mmw2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mmw2.h \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.h \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}alvfile.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9compl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}ub9host.h \
    {e:\base\Rm2\src;$(INCLUDE);}af9apply.h \
    {e:\base\Rm2\src;$(INCLUDE);}msgthrd.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}swdw.h \
    {e:\base\Rm2\src;$(INCLUDE);}mytrkhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myfcshdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplncntl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MAINW.CPP

.\MACHL.obj: \
    e:\base\Rm2\src\MACHL.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MACHL.CPP

.\MACH.obj: \
    e:\base\Rm2\src\MACH.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr08.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr95.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\MACH.CPP

.\IPRINTER.obj: \
    e:\base\Rm2\src\IPRINTER.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}iprinter.h \
    {e:\base\Rm2\src;$(INCLUDE);}iprinter.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\IPRINTER.CPP

.\IALCTLS.obj: \
    e:\base\Rm2\src\IALCTLS.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}ialctls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}alctls.h \
    {e:\base\Rm2\src;$(INCLUDE);}istattxt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\IALCTLS.CPP

.\FEATL.obj: \
    e:\base\Rm2\src\FEATL.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\FEATL.CPP

.\EUOPRES.obj: \
    e:\base\Rm2\src\EUOPRES.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\EUOPRES.CPP

.\EUOPIPRT.obj: \
    e:\base\Rm2\src\EUOPIPRT.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}iprinter.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\EUOPIPRT.CPP

.\EUOGMAIN.obj: \
    e:\base\Rm2\src\EUOGMAIN.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}euogmain.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euophlp.h \
    {e:\base\Rm2\src;$(INCLUDE);}euogmain.h \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.h \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}euopids.h \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplnw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mytrkhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xplncntl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\EUOGMAIN.CPP

.\CFRV.obj: \
    e:\base\Rm2\src\CFRV.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFRV.CPP

.\CFRSW.obj: \
    e:\base\Rm2\src\CFRSW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrfq.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFRSW.CPP

.\CFRFQ.obj: \
    e:\base\Rm2\src\CFRFQ.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrfq.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFRFQ.CPP

.\CFREC.obj: \
    e:\base\Rm2\src\CFREC.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFREC.CPP

.\CFR96.obj: \
    e:\base\Rm2\src\CFR96.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR96.CPP

.\CFR95.obj: \
    e:\base\Rm2\src\CFR95.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr95.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR95.CPP

.\CFR54.obj: \
    e:\base\Rm2\src\CFR54.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr95.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR54.CPP

.\CFR19_50.obj: \
    e:\base\Rm2\src\CFR19_50.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrfq.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr19_50.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR19_50.CPP

.\CFR10.obj: \
    e:\base\Rm2\src\CFR10.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR10.CPP

.\CFR08.obj: \
    e:\base\Rm2\src\CFR08.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfrfq.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr08.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euomsg.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR08.CPP

.\CFR05.obj: \
    e:\base\Rm2\src\CFR05.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\CFR05.CPP

.\BLFEATL.obj: \
    e:\base\Rm2\src\BLFEATL.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr19_50.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.h \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\BLFEATL.CPP

.\BKLOGW.obj: \
    e:\base\Rm2\src\BKLOGW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}bklogw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}bklogw.h \
    {e:\base\Rm2\src;$(INCLUDE);}prf.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mainw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}ww.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopres.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}af9main.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euodef.h \
    {e:\base\Rm2\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mach.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}system.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr96.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}machl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}featl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\Rm2\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\Rm2\src;$(INCLUDE);}parr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}feat.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}rpo2.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\BKLOGW.CPP

.\BASEW.obj: \
    e:\base\Rm2\src\BASEW.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}uicl.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}basew.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\Rm2\src;$(INCLUDE);}myusrhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\BASEW.CPP

.\ALVFILE.obj: \
    e:\base\Rm2\src\ALVFILE.CPP \
    {e:\base\Rm2\src;$(INCLUDE);}alvfile.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\ALVFILE.CPP

.\AFCV2031.obj: \
    e:\base\src\AFCV2031.CPP \
    {e:\base\src;$(INCLUDE);}afcv2031.hpp \
    {e:\base\src;$(INCLUDE);}afcv2031.h \
    {e:\base\src;$(INCLUDE);}af9apply.h
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCV2031.CPP

.\AFCSYS.obj: \
    e:\base\src\AFCSYS.CPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCSYS.CPP

.\AFCSAVE.obj: \
    e:\base\src\AFCSAVE.CPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCMCROS.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9ORDRS.HPP \
    {e:\base\src;$(INCLUDE);}AF9SYSTM.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCSAVE.CPP

.\AFCPROD.obj: \
    e:\base\src\AFCPROD.CPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCPROD.CPP

.\AFCPARSE.obj: \
    e:\base\src\AFCPARSE.CPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCMCROS.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCPARSE.CPP

.\AFCMES.obj: \
    e:\base\src\AFCMES.CPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCMES.CPP

.\AFCLSTFL.obj: \
    e:\base\src\AFCLSTFL.CPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCLSTFL.CPP

.\AFCLOAD.obj: \
    e:\base\src\AFCLOAD.CPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCMCROS.H \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9ORDRS.HPP \
    {e:\base\src;$(INCLUDE);}AF9SYSTM.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCLOAD.CPP

.\AFCGETFL.obj: \
    e:\base\src\AFCGETFL.CPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCGETFL.CPP

.\AFCGENCF.obj: \
    e:\base\src\AFCGENCF.CPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCMCROS.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9ORDRS.HPP \
    {e:\base\src;$(INCLUDE);}AF9SYSTM.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCGENCF.CPP

.\AFCCPULS.obj: \
    e:\base\src\AFCCPULS.CPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCCPULS.CPP

.\AFCCNVRT.obj: \
    e:\base\src\AFCCNVRT.CPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCCNVRT.CPP

.\AFCCFRST.obj: \
    e:\base\src\AFCCFRST.CPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCCFRST.CPP

.\AFCBKEND.obj: \
    e:\base\src\AFCBKEND.CPP \
    {e:\base\src;$(INCLUDE);}AFCBKEND.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9ORDRS.HPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCBKEND.CPP

.\AFCABPRD.obj: \
    e:\base\src\AFCABPRD.CPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AF9APPLY.H \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AFCABPRD.CPP

.\AF9SYSTM.obj: \
    e:\base\src\AF9SYSTM.CPP \
    {e:\base\src;$(INCLUDE);}af9systm.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9sysed.hpp \
    {e:\base\src;$(INCLUDE);}afcprods.hpp \
    {e:\base\src;$(INCLUDE);}afcbkend.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9SYSTM.CPP

.\AF9SYSED.obj: \
    e:\base\src\AF9SYSED.CPP \
    {e:\base\src;$(INCLUDE);}af9sysed.hpp \
    {e:\base\src;$(INCLUDE);}af9systm.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9SYSED.CPP

.\AF9SELDT.obj: \
    e:\base\src\AF9SELDT.CPP \
    {e:\base\src;$(INCLUDE);}af9seldt.hpp \
    {e:\base\src;$(INCLUDE);}af9ordrs.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9SELDT.CPP

.\AF9RTSRC.obj: \
    e:\base\src\AF9RTSRC.CPP \
    {e:\base\src;$(INCLUDE);}af9rtsrc.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}ub9host.h \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9RTSRC.CPP

.\AF9RECON.obj: \
    e:\base\src\AF9RECON.CPP \
    {e:\base\src;$(INCLUDE);}af9recon.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9RECON.CPP

.\AF9PTION.obj: \
    e:\base\src\AF9PTION.CPP \
    {e:\base\src;$(INCLUDE);}af9ption.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9PTION.CPP

.\AF9PROFL.obj: \
    e:\base\src\AF9PROFL.CPP \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9ordrs.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9PROFL.CPP

.\AF9ORDRS.obj: \
    e:\base\src\AF9ORDRS.CPP \
    {e:\base\src;$(INCLUDE);}af9ordrs.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9systm.hpp \
    {e:\base\src;$(INCLUDE);}af9seldt.hpp \
    {e:\base\src;$(INCLUDE);}afcprods.hpp \
    {e:\base\src;$(INCLUDE);}afcbkend.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9ORDRS.CPP

.\AF9NAME.obj: \
    e:\base\src\AF9NAME.CPP \
    {e:\base\src;$(INCLUDE);}af9name.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}afcbkend.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9NAME.CPP

.\AF9MAIN.obj: \
    e:\base\src\AF9MAIN.CPP \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}afcbkend.hpp \
    {e:\base\src;$(INCLUDE);}af9ordrs.hpp \
    {e:\base\src;$(INCLUDE);}af9systm.hpp \
    {e:\base\src;$(INCLUDE);}af9detl.hpp \
    {e:\base\src;$(INCLUDE);}af9name.hpp \
    {e:\base\src;$(INCLUDE);}af9ption.hpp \
    {e:\base\src;$(INCLUDE);}af9local.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}af9rtsrc.hpp \
    {e:\base\src;$(INCLUDE);}af9recon.hpp \
    {e:\base\src;$(INCLUDE);}afcv2031.hpp \
    {e:\base\src;$(INCLUDE);}ub9host.h \
    {e:\base\src;$(INCLUDE);}mainw.hpp \
    {e:\base\src;$(INCLUDE);}mainw.h \
    {e:\base\src;$(INCLUDE);}prf.hpp \
    {e:\base\src;$(INCLUDE);}prfw4.hpp \
    {e:\base\src;$(INCLUDE);}bridge.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP \
    {e:\base\src;$(INCLUDE);}AFCPRODS.HPP \
    {e:\base\src;$(INCLUDE);}uicl.hpp \
    {e:\base\src;$(INCLUDE);}basew.hpp \
    {e:\base\src;$(INCLUDE);}myxcphdr.hpp \
    {e:\base\src;$(INCLUDE);}ww.hpp \
    {e:\base\src;$(INCLUDE);}euopres.hpp \
    {e:\base\src;$(INCLUDE);}euopiprt.hpp \
    {e:\base\src;$(INCLUDE);}minmxhdr.hpp \
    {e:\base\src;$(INCLUDE);}msgthrd.hpp \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}myptrhdr.hpp \
    {e:\base\src;$(INCLUDE);}euoutil.hpp \
    {e:\base\src;$(INCLUDE);}myusrhdr.hpp \
    {e:\base\src;$(INCLUDE);}system.hpp \
    {e:\base\src;$(INCLUDE);}machl.hpp \
    {e:\base\src;$(INCLUDE);}pkfeatl.hpp \
    {e:\base\src;$(INCLUDE);}featl.hpp \
    {e:\base\src;$(INCLUDE);}mfeatl.hpp \
    {e:\base\src;$(INCLUDE);}blfeatl.hpp \
    {e:\base\src;$(INCLUDE);}cfrsw.hpp \
    {e:\base\src;$(INCLUDE);}cfr10.hpp \
    {e:\base\src;$(INCLUDE);}cfr05.hpp \
    {e:\base\src;$(INCLUDE);}xpln2l.hpp \
    {e:\base\src;$(INCLUDE);}rpo2l.hpp \
    {e:\base\src;$(INCLUDE);}cfrv.hpp \
    {e:\base\src;$(INCLUDE);}pcol6.hpp \
    {e:\base\src;$(INCLUDE);}parr.hpp \
    {e:\base\src;$(INCLUDE);}mach.hpp \
    {e:\base\src;$(INCLUDE);}pkfeat.hpp \
    {e:\base\src;$(INCLUDE);}feat.hpp \
    {e:\base\src;$(INCLUDE);}mfeat.hpp \
    {e:\base\src;$(INCLUDE);}euodef.h \
    {e:\base\src;$(INCLUDE);}xpln2.hpp \
    {e:\base\src;$(INCLUDE);}rpo2.hpp \
    {e:\base\src;$(INCLUDE);}cfrec.hpp \
    {e:\base\src;$(INCLUDE);}cfr54.hpp \
    {e:\base\src;$(INCLUDE);}cfr96.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9MAIN.CPP

.\AF9LOCAL.obj: \
    e:\base\src\AF9LOCAL.CPP \
    {e:\base\src;$(INCLUDE);}af9local.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9LOCAL.CPP

.\AF9DTOBJ.obj: \
    e:\base\src\AF9DTOBJ.CPP \
    {e:\base\src;$(INCLUDE);}af9dtobj.hpp \
    {e:\base\src;$(INCLUDE);}afcprods.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}AF9MAIN.HPP \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCCPULS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9DTOBJ.CPP

.\AF9DETL.obj: \
    e:\base\src\AF9DETL.CPP \
    {e:\base\src;$(INCLUDE);}af9detl.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9dtobj.hpp \
    {e:\base\src;$(INCLUDE);}afcprods.hpp \
    {e:\base\src;$(INCLUDE);}afcbkend.hpp \
    {e:\base\src;$(INCLUDE);}af9profl.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H \
    {e:\base\src;$(INCLUDE);}AFCSYS.HPP \
    {e:\base\src;$(INCLUDE);}AFCFEAT.HPP \
    {e:\base\src;$(INCLUDE);}AFCCFRST.HPP
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9DETL.CPP

.\AF9COMPL.obj: \
    e:\base\src\AF9COMPL.CPP \
    {e:\base\src;$(INCLUDE);}af9compl.hpp \
    {e:\base\src;$(INCLUDE);}af9main.hpp \
    {e:\base\src;$(INCLUDE);}af9apply.h \
    {e:\base\src;$(INCLUDE);}afccpuls.hpp \
    {e:\base\src;$(INCLUDE);}AFCINCLS.H
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\src\AF9COMPL.CPP

.\USERHDR.obj: \
    e:\base\Rm2\src\rm2icext\USERHDR.CPP \
    {e:\base\Rm2\src\rm2icext;$(INCLUDE);}userhdr.hpp
    @echo " Compile "
    icc.exe /Tx /Gm /Ti /Gd /Fo".\%|fF.obj" /C e:\base\Rm2\src\rm2icext\USERHDR.CPP

e:\base\BF9BRET.EXE: \
    .\SZNOTEBK.obj \
    e:\base\src\af9apply.res \
    .\USERHDR.obj \
    .\AF9COMPL.obj \
    .\AF9DETL.obj \
    .\AF9DTOBJ.obj \
    .\AF9LOCAL.obj \
    .\AF9MAIN.obj \
    .\AF9NAME.obj \
    .\AF9ORDRS.obj \
    .\AF9PROFL.obj \
    .\AF9PTION.obj \
    .\AF9RECON.obj \
    .\AF9RTSRC.obj \
    .\AF9SELDT.obj \
    .\AF9SYSED.obj \
    .\AF9SYSTM.obj \
    .\AFCABPRD.obj \
    .\AFCBKEND.obj \
    .\AFCCFRST.obj \
    .\AFCCNVRT.obj \
    .\AFCCPULS.obj \
    .\AFCGENCF.obj \
    .\AFCGETFL.obj \
    .\AFCLOAD.obj \
    .\AFCLSTFL.obj \
    .\AFCMES.obj \
    .\AFCPARSE.obj \
    .\AFCPROD.obj \
    .\AFCSAVE.obj \
    .\AFCSYS.obj \
    .\AFCV2031.obj \
    .\ALVFILE.obj \
    .\BASEW.obj \
    .\BKLOGW.obj \
    .\BLFEATL.obj \
    .\CFR05.obj \
    .\CFR08.obj \
    .\CFR10.obj \
    .\CFR19_50.obj \
    .\CFR54.obj \
    .\CFR95.obj \
    .\CFR96.obj \
    .\CFREC.obj \
    .\CFRFQ.obj \
    .\CFRSW.obj \
    .\CFRV.obj \
    .\EUOGMAIN.obj \
    .\EUOPIPRT.obj \
    .\EUOPRES.obj \
    .\FEATL.obj \
    .\IALCTLS.obj \
    .\IPRINTER.obj \
    .\MACH.obj \
    .\MACHL.obj \
    .\MAINW.obj \
    .\MFEAT.obj \
    .\MFEATL.obj \
    .\MINMXHDR.obj \
    .\MMW2.obj \
    .\MSGTHRD.obj \
    .\MYFCSHDR.obj \
    .\MYPTRHDR.obj \
    .\MYTIMERS.obj \
    .\MYTRKHDR.obj \
    .\MYXCPHDR.obj \
    .\PARRSTR.obj \
    .\PCOL6.obj \
    .\PPRVW.obj \
    .\PRF.obj \
    .\PRFW4.obj \
    .\RPO2.obj \
    .\RPO2L.obj \
    .\RPO2WC.obj \
    .\RPODATA.obj \
    .\RPOINFO.obj \
    .\SWDW.obj \
    .\SYSTEM.obj \
    .\WW.obj \
    .\XPLN2L.obj \
    .\XPLNCNTL.obj \
    .\XPLNW.obj \
    .\EUOUTIL.obj \
    .\PARR.obj \
    .\PARRREC.obj \
    .\PKFEAT.obj \
    .\PKFEATL.obj \
    .\ICTRLEVT.obj \
    .\ICTRLHDR.obj \
    .\IMMVEEVT.obj \
    .\IMMVEHDR.obj \
    .\KEYHNDLR.obj \
    .\POPMSHDR.obj \
    .\SELENTRY.obj \
    {$(LIB)}cppwm35.lib \
    {$(LIB)}user32.lib \
    {$(LIB)}gdi32.lib \
    {$(LIB)}comdlg32.lib
    @echo " Link "
    icc.exe @<<
    /Tdp 
     /Tx 
     /Ti /Gm /Gd 
      /B" /de /nobrowse /pmtype:pm /code:RX /data:RW"
     /B" /def"
     /B" /def:cppwm35.lib"
     /B" /def:user32.lib"
     /B" /def:gdi32.lib"
     /B" /def:comdlg32.lib"
     /Fee:\base\BF9BRET.EXE 
     cppwm35.lib 
     user32.lib 
     gdi32.lib 
     comdlg32.lib 
     .\SZNOTEBK.obj
     e:\base\src\af9apply.res
     .\USERHDR.obj
     .\AF9COMPL.obj
     .\AF9DETL.obj
     .\AF9DTOBJ.obj
     .\AF9LOCAL.obj
     .\AF9MAIN.obj
     .\AF9NAME.obj
     .\AF9ORDRS.obj
     .\AF9PROFL.obj
     .\AF9PTION.obj
     .\AF9RECON.obj
     .\AF9RTSRC.obj
     .\AF9SELDT.obj
     .\AF9SYSED.obj
     .\AF9SYSTM.obj
     .\AFCABPRD.obj
     .\AFCBKEND.obj
     .\AFCCFRST.obj
     .\AFCCNVRT.obj
     .\AFCCPULS.obj
     .\AFCGENCF.obj
     .\AFCGETFL.obj
     .\AFCLOAD.obj
     .\AFCLSTFL.obj
     .\AFCMES.obj
     .\AFCPARSE.obj
     .\AFCPROD.obj
     .\AFCSAVE.obj
     .\AFCSYS.obj
     .\AFCV2031.obj
     .\ALVFILE.obj
     .\BASEW.obj
     .\BKLOGW.obj
     .\BLFEATL.obj
     .\CFR05.obj
     .\CFR08.obj
     .\CFR10.obj
     .\CFR19_50.obj
     .\CFR54.obj
     .\CFR95.obj
     .\CFR96.obj
     .\CFREC.obj
     .\CFRFQ.obj
     .\CFRSW.obj
     .\CFRV.obj
     .\EUOGMAIN.obj
     .\EUOPIPRT.obj
     .\EUOPRES.obj
     .\FEATL.obj
     .\IALCTLS.obj
     .\IPRINTER.obj
     .\MACH.obj
     .\MACHL.obj
     .\MAINW.obj
     .\MFEAT.obj
     .\MFEATL.obj
     .\MINMXHDR.obj
     .\MMW2.obj
     .\MSGTHRD.obj
     .\MYFCSHDR.obj
     .\MYPTRHDR.obj
     .\MYTIMERS.obj
     .\MYTRKHDR.obj
     .\MYXCPHDR.obj
     .\PARRSTR.obj
     .\PCOL6.obj
     .\PPRVW.obj
     .\PRF.obj
     .\PRFW4.obj
     .\RPO2.obj
     .\RPO2L.obj
     .\RPO2WC.obj
     .\RPODATA.obj
     .\RPOINFO.obj
     .\SWDW.obj
     .\SYSTEM.obj
     .\WW.obj
     .\XPLN2L.obj
     .\XPLNCNTL.obj
     .\XPLNW.obj
     .\EUOUTIL.obj
     .\PARR.obj
     .\PARRREC.obj
     .\PKFEAT.obj
     .\PKFEATL.obj
     .\ICTRLEVT.obj
     .\ICTRLHDR.obj
     .\IMMVEEVT.obj
     .\IMMVEHDR.obj
     .\KEYHNDLR.obj
     .\POPMSHDR.obj
     .\SELENTRY.obj
<<
