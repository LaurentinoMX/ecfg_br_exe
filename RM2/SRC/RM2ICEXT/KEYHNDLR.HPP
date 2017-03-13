#ifndef _DJL_ISELECTIVEKEYHDR_
#define _DJL_ISELECTIVEKEYHDR_

#include <ikeyhdr.hpp>

class ISelectiveKeyHandler : public IKeyboardHandler
{
  protected:
    IString  * theLimitString;
    Boolean    toCaps;

  Boolean
    characterKeyPress(IKeyboardEvent& keyevt);

  public:
      ISelectiveKeyHandler ( );
      ISelectiveKeyHandler ( const IString & );
      ISelectiveKeyHandler ( const Boolean );
      ISelectiveKeyHandler ( const IString &,
                             const Boolean );

    const IString &
      limitString ( ) const { return *theLimitString; };

    const Boolean
      isCapitalizing ( ) const { return toCaps; };

    void
      limitInputTo ( const IString & );
    void
      capitalize ( const Boolean caps ) { toCaps = caps; };
};
#endif
