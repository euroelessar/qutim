#ifndef QUICKCONFIG_H
#define QUICKCONFIG_H

#include <QObject>
#include <qutim/config.h>

class QuickConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    
public:
    explicit QuickConfig(QObject *parent = 0);
    
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &def = QVariant());
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    QString path() const;
    void setPath(const QString &path);

signals:
    void pathChanged(QString path);
    
private:
    QString m_path;
    qutim_sdk_0_3::Config m_config;
};

#endif // QUICKCONFIG_H
