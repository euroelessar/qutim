#ifndef QUTIM_SDK_0_3_QUICKCONFIG_H
#define QUTIM_SDK_0_3_QUICKCONFIG_H

#include <QObject>
#include <QQmlParserStatus>
#include "config.h"

namespace qutim_sdk_0_3 {

class QuickConfig : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
public:
    explicit QuickConfig(QObject *parent = 0);
    
    void setPath(const QString &path);
    QString path() const;
    
    void setGroup(const QString &group);
    QString group() const;
    
    void classBegin();
    void componentComplete();
    
public slots:
    QVariant value(const QString &name, const QVariant &defaultValue);
    void setValue(const QString &name, const QVariant &value);
    
    void beginGroup(const QString &name);
    void endGroup();
    
signals:
    void pathChanged();
    void groupChanged();
    
private:
    Config m_config;
    QString m_path;
    QString m_group;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKCONFIG_H
