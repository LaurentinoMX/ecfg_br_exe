@echo off
rem ===============================================================
rem  Batch file for building Base Retrieval code (BUILD NORMAL)
rem
rem  Note: The project file for this project assumes that 
rem        source files reside on the D:\ drive.  Therefore, if 
rem        the source files are located on any other drive,
rem        please make the changes in the ipfprj.iwp, resprj.iwp,
rem        bf9bret.iwp, rmcomppk.iwp using VisualAge C++ WorkFrame IDE.
rem        The source files MUST be in a directory named 'base'
rem        that branches directly from a root drive.
rem        Example:  'd:\base'.
rem
rem  Output exe file:                    base\bf9bret.exe
rem  Output dll file:                    base\locale\bf9en_us.dll
rem  Output hlp file:                    base\locale\bf9en_us.hlp
rem  Output product knowledge file tool: base\rmcomppk.exe
rem                 
rem ===============================================================

@start pbuild.bat
@if not exist locale\*.* md locale
@if not exist obj\*.* md obj
@if not exist obj\ppk\*.* md obj\ppk
@set INCLUDE=%INCLUDE%;inc;hlp;.
@hlp\ipfpw32.exe hlp\95brdrv.ipf hlp\bf9en_us.ipf
@copy hlp\bf9en_us.ipf mri
@iwfwbl35.exe /project "\base\vac\ipfprj.iwp" /toolName "Build normal"
@copy \base\mri\bf9en_us.hlp \base\locale
@iwfwbl35.exe /project "\base\vac\resprj.iwp" /toolName "Build normal"
@iwfwbl35.exe /project "\base\vac\bf9bret.iwp" /toolName "Build normal"
@iwfwbl35.exe /project "\base\rm2\rmcomppk.iwp" /toolName "Build normal"
@dllrname bf9bret.exe cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i
@dllrname rmcomppk.exe cppwm35i=ecgwm35i cppwob3i=ecgwob3i cppwou3i=ecgwou3i
