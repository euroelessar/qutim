#ifndef QUICKCONFIG_H
#define QUICKCONFIG_H

#include <QObject>
#include <qutim/config.h>

class QuickConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
	Q_PROPERTY(QObject* object READ object WRITE setObject NOTIFY objectChanged)
	Q_FLAGS(ValueFlag ValueFlags)
public:
	enum ValueFlag { Normal = 0x00, Crypted = 0x01 };
	Q_DECLARE_FLAGS(ValueFlags, ValueFlag)
	
    explicit QuickConfig(QObject *parent = 0);
    
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &def = QVariant(), ValueFlags type = Normal);
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value, ValueFlags type = Normal);
	QString path() const;
    void setPath(const QString &path);
	QObject *object() const;
    void setObject(QObject *object);

signals:
	void pathChanged(const QString &path);
	void objectChanged(QObject *object);
    
private:
	QObject *m_object;
    QString m_path;
    qutim_sdk_0_3::Config m_config;
};

#endif // QUICKCONFIG_H
