#ifndef VAVATARMANAGEMENT_H
#define VAVATARMANAGEMENT_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class VavatarManagement : public QObject
{
    Q_OBJECT
public:
    VavatarManagement(const QString &account_name,
                      const QString &profile_name, QObject *parent = 0);
    ~VavatarManagement();
    void requestForAvatar(const QString &buddy_id, const QString &avatar_url);

private slots:
    void replyFinished(QNetworkReply*);

signals:
    void avatarDownloaded(const QString &, const QByteArray &);

private:
    QString m_account_name;
    QString m_profile_name;
    QHash<QString,QNetworkAccessManager*> m_request_list;
};

#endif // VAVATARMANAGEMENT_H
