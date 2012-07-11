#ifndef VK_OAUTHCONNECTION_H
#define VK_OAUTHCONNECTION_H

#include <vk/connection.h>

class QWebPage;

namespace vk {

class OAuthConnectionPrivate;

class OAuthConnection : public Connection
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(OAuthConnection)
	Q_ENUMS(DisplayType)
public:
	enum DisplayType {
		Page,
		Popup,
		Touch,
		Wap
	};
	explicit OAuthConnection(int appId, QObject *parent = 0);
	virtual ~OAuthConnection();

	virtual void connectToHost(const QString &login, const QString &password);
	virtual void disconnectFromHost();
	virtual QNetworkReply *request(const QString &method, const QVariantMap &args = QVariantMap());
	virtual Client::State connectionState() const;
	virtual int uid() const;
signals:
	void authConfirmRequested(QWebPage *page);
protected:
	QScopedPointer<OAuthConnectionPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_loadFinished(bool))
};

} // namespace vk

#endif // VK_OAUTHCONNECTION_H
