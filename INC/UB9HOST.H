/* UB9HOST.H */

// These are definitions for the command protocol for driving
// the HLLAPI host application

#ifndef UB9HOST_H
#define UB9HOST_H

// Command Keywords
#define STR_TagLocale                   "LOCALE"
#define STR_TagFiles                    "FILES"
#define STR_TagFunction                 "FUNCTION"
#define STR_TagSendTo                   "SENDTO"
#define STR_TagSend                     "SEND"
#define STR_TagUpload                   "UPLOAD"
#define STR_TagDownload                 "DOWNLOAD"
#define STR_TagDirectory                "DIRECTORY"
#define STR_TagGetName                  "GETNAME"
#define STR_TagName                     "NAME"

// Output Keywords
#define STR_TagReturn                   "RETURN"

// Function Keywords
#define STR_TagRetrieve                 "RETRIEVE"
#define STR_TagSysbase                  "SYSBASE"

// Option Keywords
#define STR_TagS390                     "S/390"
#define STR_TagDelete                   "DELETE"

// Filetype Keywords
#define STR_TagAscii                    "ASCII"
#define STR_TagBinary                   "BINARY"

#endif
