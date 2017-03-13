/*-------------------------------------------------------------+
|
| This program will
|     read in a number of stems FILE1. -- FILEXX.
|     Loop for each stem
|       write stem to disk to a file called NBHWMES EXTDATA A
|       sendfile NBHWMES EXTDATA A to a specific id
|     End /* do loop */
| Note that this EXEC should be uploaded with an A3 filemode
| so that it will be deleted after execution.
+--------------------------------------------------------------*/

Exit_Code = 0
CP set msg off
Call File_Processing

Do Cnt = 1 To Num_Of_Stems While Exit_Code = 0

  'Pipe (StageSep !)',
  '  Stem FILE'Cnt'.',
  '! > NBHWMES EXTDATA A'

  If Rc = 0 Then Do
    'SENDFILE NBHWMES EXTDATA A TO PWSPROGA AT HQVMIC1 (NOLOG NOTYPE'
    IF Rc = 0 Then Address Command 'ERASE NBHWMES EXTDATA A'
  End /* Then */
  Else Exit_Code = 99

End /* Do */

'VMFCLEAR'
If Exit_Code = 0 Then Say 'ALL FILES HAVE BEEN SENT'
Else Say 'ERROR PROCESSING FILE: FILE'Cnt'.'

'VMFCLEAR'

Exit Exit_Code

File_Processing:

/*-----------------------------------------------------------+
| Define all stems here.
| Set variable Num_Of_Stems = total number of stems defined
+------------------------------------------------------------*/
