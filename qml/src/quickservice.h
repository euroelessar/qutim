#ifndef QUTIM_SDK_0_3_QUICKSERVICE_H
#define QUTIM_SDK_0_3_QUICKSERVICE_H

#include <QObject>
#include <QPointer>

namespace qutim_sdk_0_3 {

class QuickService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QObject* object READ object NOTIFY objectChanged)
public:
    explicit QuickService(QObject *parent = 0);
    ~QuickService();

    QString name() const;
    void setName(const QString &name);

    QObject *object() const;

protected:
    void onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);

signals:
    void nameChanged(const QString &name);
    void objectChanged(QObject *object);

private:
    QString m_name;
    QByteArray m_utf8Name;
    QPointer<QObject> m_object;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKSERVICE_H
