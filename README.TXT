
       The code is in CMVC. Extract all the files in the following
       three (3) components from the desired release.  These
       files should be extracted into a directory called "D:\BASE".
       		Components:
		===========
		ecfg_br_exe_src
		ecfg_br_mri_src
		ecfg_br_src_src

       Note: The workframe objects are setup to work with "D:\BASE"
             as the install directory. If installing to a different
             drive or directory, you will need to make changes to the
             workframe (select View -> Settings -> Target to specify
             changes) or use the "Quick ..." instructions in the next
	     section.

The deliverables:
       1. bf9en_us.hlp
       2. bf9en_us.dll
       3. bf9bret.exe
       4. rmcomppk.exe

1.1 Quick way to make all deliverables together:
================================================
1.  Extract all source code from the three components listed above into a directory
    called '\BASE'.  This directory must branch directory from a root drive.
    Example:  'D:\BASE'.
2.  From a command-line prompt, CD to the base retrieval directory.
    Example:  'CD \BASE'
3.  Run the 'rebuild.bat' batfile from the command prompt to rebuild all the files
    in all four base retrieval deliverables.
    OR
    Run the 'build.bat' batfile from the command prompt to build only the files
    in the 4 delieverables that have changed since your last compilation.
Notes:
	- DLLRNAME is executed on the bf9bret.exe and rmcomppk.exe to rename all
	  references to the our renamed-versions of the VisualAge delivered DLL.
	- This method of building the deliverables still uses the VisualAge *.iwp
         files.  So, for an example, if you need to compile bf9bret.exe with
         debugging on, you need to update the bf9bret.iwp project file as
         usual and then run 'build.bat' or 'rebuild.bat'
	- The following additional files are need to run Base Retrieval.  Extract
	  them from the listed CMVC component and into the same directory that
	  bf9bret.exe was built into:
                (Base Retrieval data files)
		br_exe CMVC component:
			CPULOOK.TBL
			FEATURE.$$F
			NOCOMP.TBL
			RM2XMIT.REX
                (VisualAge library support files)
		valib_exe CMVC component:
			ECGWM35I.DLL
			ECGWOB3I.DLL
			ECGWOR3U.DLL
			ECGWOU3I.DLL
		(Host interface cpf files)
		vhi_exe CMVC component:
			UB9HOST1.EXE
		vhi_mri_en_us_exe CMVC component:
			LOCALE\UB9ENUS1.DLL
			LOCALE\UB9ENUS1.HLP
              	bhi_exe CMVC component:
			UB9HOST3.EXE
		bhi_mri_en_us_exe CMVC component:
			LOCALE\UB9ENUS3.DLL
			LOCALE\UB9ENUS3.HLP



1.2 How to make the deliverables individually using VisualAge Workframe:
========================================================================
       1. bf9en_us.hlp
       ~~~~~~~~~~~~~~~
       A. Run D:\BASE\HLP\MAKEHELP.BAT
       B. Open the "Ipfprj.iwp" workframe object in the VAC directory.
       C. Right click within the dialog and select "Rebuild all" in the popup menu.
       D. After compiling finished and project is unlocked, execute this command
          COPY D:\BASE\MRI\BF9EN_US.HLP D:\BASE\LOCALE
       E. Deliverable is in D:\BASE\LOCALE .

       Note: The MAKEHELP.BAT creates a single ipf file and copies it to the proper
             location. The ipf files are in HELP directory.

       2. bf9en_us.dll
       ~~~~~~~~~~~~~~~
       A. Open the "Resprj.iwp" workframe object in the VAC directory.
       B. Right click within the dialog and select "Rebuild all" in the popup menu.
       C. Deliverable is in D:\BASE\LOCALE .

       3. rmcomppk.exe ( tool for building product knowledge file - FEATURE.$$F )
       ~~~~~~~~~~~~~~~
       A. Open the "rmcomppk.iwp" workframe object in the RM2 directory.
       B. Right click within the dialog and select "Rebuild all" in the popup menu.
       C. Run D:\BASE\BRDLLR.BAT
       D. Deliverable is in D:\BASE .

       Note: 	1. The source files are in RM2\SRC\PPK directory
	
       4. bf9bret.exe
       ~~~~~~~~~~~~~~~
       A. Open the "Bf9bret.iwp" workframe object in the VAC directory.
       B. Right click within the dialog and select "Rebuild all" in the popup menu.
       C. Run D:\BASE\BRDLLR.BAT
       D. Deliverable is in D:\BASE .

       Note: 	1. The source files are in SRC directory.
		2. The ecg*.dll files are renamed VisualAge C++ dlls where 'cpp' has
		   been replaced by 'ecg'. These files were created by using dllrname
		   on the original dll files.
		3. See the note above for the list of additional files needed to
		   run Base Retrieval.



