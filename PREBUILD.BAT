@SET INCLUDE=%INCLUDE%;.\INC
@del objp\*.obj
@del lib\bf9profl.lib
@del lib\*.exp
@del bf9profl.dll
@if not exist OBJP\*.* md OBJP
@nmake -f mak\bf9profl.mak
@dllrname bf9profl.dll cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i
