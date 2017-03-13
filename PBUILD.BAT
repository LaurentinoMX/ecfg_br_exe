@SET INCLUDE=%INCLUDE%;.\INC
@if not exist OBJP\*.* md OBJP
@nmake -f mak\bf9profl.mak
@dllrname bf9profl.dll cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i
