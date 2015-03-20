#ifndef QUTIM_SDK_0_3_QUICKPLUGINWRAPPER_H
#define QUTIM_SDK_0_3_QUICKPLUGINWRAPPER_H

#include <QObject>
#include <QPointer>

namespace qutim_sdk_0_3 {

class QuickPluginWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QObject* object READ object NOTIFY objectChanged)

public:
    explicit QuickPluginWrapper(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QObject *object() const;

signals:
    void nameChanged(const QString &name);
    void objectChanged(QObject *object);

private:
    QString m_name;
    QPointer<QObject> m_object;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKPLUGINWRAPPER_H
