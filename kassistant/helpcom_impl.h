#include "helpcom.h"

class HelpCom : virtual public helpcom_skel
{

public:
	HelpCom();

	CORBA::ULong registerApp( const char *name );
	void removeApp( CORBA::ULong id );
	void showDocument( CORBA::ULong id, const char *name );
	void documentHint( CORBA::ULong id, const char *name );
	void activate( CORBA::ULong id );
	void configure();

private:
	// store per registered app: assistantPosition, assistantSize, 
	// browserSize, browserPos, appname, history, currentURL
};
