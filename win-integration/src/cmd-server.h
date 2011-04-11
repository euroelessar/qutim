#include "winint.h"
#include <QTcpServer>
#include <QMap>

class WININT_EXPORTS CmdServer : public QObject
{
	Q_OBJECT

public:
	void registerHandler(const QString &prefix, QObject *receiver);
	static CmdServer *instance();

signals:

public slots:
	void onNewConnection();
	void onRcvDestroyed(QObject* obj);

private:
	CmdServer();
	QMap<QString, QObject*> handlers_;
	QTcpServer listenner_;
};
