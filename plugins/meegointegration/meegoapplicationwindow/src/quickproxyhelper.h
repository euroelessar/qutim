#ifndef MEEGOINTEGRATION_QUICKPROXYHELPER_H
#define MEEGOINTEGRATION_QUICKPROXYHELPER_H

#include <QObject>
#include <qutim/networkproxy.h>
#include <qutim/dataforms.h>

namespace MeegoIntegration {

class QuickProxyHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *account READ account WRITE setAccount NOTIFY accountChanged)
public:
    explicit QuickProxyHelper(QObject *parent = 0);

    QObject *account() const;
    void setAccount(QObject *account);
    
    Ureen::Config config() const;
    
public slots:
    int count();
    QString name(int index);
    QString description(int index);
    QVariant settings(int index);
    void save(const QVariant &itemData, int index, bool disabled, bool useGlobal);
    QVariantMap load();

signals:
    void accountChanged(QObject *account);

private:
    Ureen::Account *m_account;
    QList<Ureen::NetworkProxyInfo *> m_infos;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_QUICKPROXYHELPER_H
