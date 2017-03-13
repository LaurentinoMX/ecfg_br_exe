set include=%include%;..\inc
set lib=%lib%;..\lib

icc /Gm+ /DNO_STRICT /Ti+ ProfileTest.cpp bf9profl.lib
