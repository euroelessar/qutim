#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QMetaProperty>
#include <QQmlParserStatus>
#include <QSharedPointer>

#define ADD_METHOD(a) void propery_changed_hook_ ## a() { onPropertyChanged(0 ## a); }

#define ADD_8_METHODS(Prefix) \
    ADD_METHOD(Prefix ## 0) \
    ADD_METHOD(Prefix ## 1) \
    ADD_METHOD(Prefix ## 2) \
    ADD_METHOD(Prefix ## 3) \
    ADD_METHOD(Prefix ## 4) \
    ADD_METHOD(Prefix ## 5) \
    ADD_METHOD(Prefix ## 6) \
    ADD_METHOD(Prefix ## 7)

#define ADD_64_METHODS(Prefix) \
    ADD_8_METHODS(Prefix ## 0) \
    ADD_8_METHODS(Prefix ## 1) \
    ADD_8_METHODS(Prefix ## 2) \
    ADD_8_METHODS(Prefix ## 3) \
    ADD_8_METHODS(Prefix ## 4) \
    ADD_8_METHODS(Prefix ## 5) \
    ADD_8_METHODS(Prefix ## 6) \
    ADD_8_METHODS(Prefix ## 7)

class ConfigData;

class Config : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit Config(QObject *parent = 0);
    ~Config();

    QString path() const;
    void setPath(const QString &path);

    virtual void classBegin();
    virtual void componentComplete();

signals:
    void pathChanged(const QString &path);

private slots:
    ADD_64_METHODS(0)

private:
    void onPropertyChanged(int index);
    void onValueChanged(const QString &key, const QVariant &value);

    QString m_path;
    QHash<QString, QMetaProperty> m_properties;
    QStringList m_propertiesNames;
    QSharedPointer<ConfigData> m_data;

    friend class ConfigData;
};

#undef ADD_64_METHODS
#undef ADD_8_METHODS
#undef ADD_METHOD

#endif // CONFIG_H
