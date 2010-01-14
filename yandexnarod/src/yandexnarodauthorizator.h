#ifndef YANDEXNARODAUTHORIZATOR_H
#define YANDEXNARODAUTHORIZATOR_H

#include <QNetworkAccessManager>

class YandexNarodAuthorizator : public QObject
{
	Q_OBJECT
public:
	enum Result { Success, Failure, Error };

	explicit YandexNarodAuthorizator(QNetworkAccessManager *parent);
	explicit YandexNarodAuthorizator(QWidget *parent);

	void requestAuthorization();
	void requestAuthorization(const QString &login, const QString &password);

public slots:
	void onRequestFinished(QNetworkReply *reply);

signals:
	void result(YandexNarodAuthorizator::Result, const QString &error = QString());

private:
	enum Stage { Need, TryingNow, Already };
	Stage m_stage;
	QNetworkAccessManager *m_networkManager;
};

#endif // YANDEXNARODAUTHORIZATOR_H
