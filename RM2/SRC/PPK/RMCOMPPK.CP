
#include "pkfeatl.hpp"

int main(int argc,char **argv)
{
 IString strF("FEATURE.TXT")
        ,strG("GENERIC.TXT")
        ,strO("FEATURE.$$F");
 ProductKnowledge PK;

 switch (argc) {
   case 1:
     break;
   case 4:
   case 3:
     strF = IString(argv[1]);
     strG = IString(argv[2]);
     if (argc == 4)
         strO = IString(argv[3]);
     break;
   default:
     goto usage;
   }
 cout << "reading  '" << strF << "' and '" << strG << "'" << endl;
 if (PK.buildNew((char *)strF,(char *)strG)) {
   cout << "writing file '" << strO << "'" << endl;
   if (PK.save((char *)strO))
     cout << "'" << strO << "' successfully built" << endl;
   else  cout << "unable to write file '" << strO << "'" << endl;
   }
 else  cout << "unable to read file '" << strF << "' or '" << strG << "'" << endl;
 goto getout;
//#ifdef __DEBUG_ALLOC__
//   _dump_allocated(16);
//#endif
usage:
    cout << "Program creates RM/2's compiled Product Knowledge file, " << endl
         << "from the given Feature and Generic Product Knowledge files." << endl
         << "Usage : COMPILEPK  Featue-File Generic-File [OutPut File]" << endl
         << "        where 'Output File' defaults to 'FEATURE.$$F." << endl
         << "Or use no parms for defaults:" << endl
         << "  'FEATURE.TXT', 'GENERIC.TXT' and 'FEATURE.$$F." << endl;
getout:
  exit(0);
}

