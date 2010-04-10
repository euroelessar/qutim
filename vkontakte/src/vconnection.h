#ifndef VCONNECTION_H
#define VCONNECTION_H
#include "vkontakte_global.h"
#include <QNetworkAccessManager>

class VAccount;
class VConnectionPrivate;
class LIBVKONTAKTE_EXPORT VConnection : public QNetworkAccessManager
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VConnection)
public:
	explicit VConnection(VAccount *account, QObject* parent = 0);
	VConnectionState connectionState() const;
	virtual ~VConnection();
public slots:
	void connectToHost(const QString &passwd);
	void disconnectFromHost();
signals:
	void connectionStateChanged(int state);
protected:
	void setConnectionState(VConnectionState state);
private:
	QScopedPointer<VConnectionPrivate> d_ptr;
};

#endif // VCONNECTION_H
