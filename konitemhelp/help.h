#include <kapp.h>
#include <kprocess.h>
#include <qapp.h>
#include <qstring.h>

class onItemHelp : QObject {
	Q_OBJECT

public:
	onItemHelp();
	void highlight( QString widgetIdentifier );
	void openHelp( QString path );

private:
	KProcess proc;
	QString *result;

public slots:
	void receivedStdout(KProcess *proc, char *buffer, int buflen);

};
