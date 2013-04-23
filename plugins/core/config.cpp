#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QVariantHash>
#include <QVariantMap>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaProperty>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QTimer>

#define CONFIG_FILE QStringLiteral("/qutim/config.json")

static void populateConfig(QVariantHash &data, QString &prefix, const QJsonObject &object)
{
    if (!prefix.isEmpty())
        prefix += QLatin1Char('.');
    const auto size = prefix.size();

    for (auto it = object.begin(); it != object.end(); ++it) {
        prefix += it.key();
        if (it.value().isObject()) {
            populateConfig(data, prefix, it.value().toObject());
        } else {
            data.insert(prefix, it.value().toVariant());
        }
        prefix.resize(size);
    }
}

static QVariantHash loadConfig()
{
    const auto path = QStandardPaths::locate(QStandardPaths::ConfigLocation, CONFIG_FILE);

    if (path.isEmpty())
        return QVariantHash();

    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        qWarning("Can't access \"%s\"", qPrintable(file.fileName()));
        return QVariantHash();
    }
    auto document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        qWarning("File \"%s\" is not valid json object", qPrintable(file.fileName()));
        return QVariantHash();
    }

    QVariantHash data;
    QString prefix;
    populateConfig(data, prefix, document.object());
    return data;
}

static void saveConfig(const QVariantHash &config)
{
    const auto path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
            + CONFIG_FILE;
    const auto tmpPath = path + QStringLiteral(".tmp");

    QFile file(tmpPath);
    if (!file.open(QFile::WriteOnly)) {
        qWarning("Can't access \"%s\"", qPrintable(file.fileName()));
        return;
    }

    QVector<QPair<QString, QVariant>> entries;
    entries.reserve(config.size());
    for (auto it = config.begin(); it != config.end(); ++it)
        entries << qMakePair(it.key(), it.value());
    qSort(entries.begin(), entries.end(),
          [] (const QPair<QString, QVariant> &first, const QPair<QString, QVariant> &second) {
        return first.first < second.first;
    });

    QList<QJsonObject> objectStack;
    QStringList pathStack;

    objectStack << QJsonObject();

    for (auto &entry : entries) {
        const auto paths = entry.first.split(QLatin1Char('.'));
        int lastIndex;
        for (lastIndex = 0; lastIndex < qMin(paths.size() - 1, pathStack.size()); ++lastIndex) {
            if (pathStack[lastIndex] != paths[lastIndex])
                break;
        }
        while (pathStack.size() > lastIndex) {
            auto object = objectStack.takeLast();
            objectStack.last().insert(pathStack.takeLast(), object);
        }
        for (int i = lastIndex; i + 1 < paths.size(); ++i) {
            objectStack << QJsonObject();
            pathStack << paths[i];
        }
        objectStack.last().insert(paths.last(), QJsonValue::fromVariant(entry.second));
    }
    while (!pathStack.isEmpty()) {
        auto object = objectStack.takeLast();
        objectStack.last().insert(pathStack.takeLast(), object);
    }

    auto object = objectStack.last();
    auto json = QJsonDocument(object).toJson();

    int error = file.write(json);
    file.close();

    if (error < 0 || error != json.size()) {
        qWarning("Error while writing \"%s\": %s",
                 qPrintable(file.fileName()),
                 qPrintable(file.errorString()));
    } else {
        auto backupPath = path + QStringLiteral(".backup");
        QFile original(path);
        if (!original.rename(backupPath)) {
            qWarning("Can not rename \"%s\" to \"%s\"",
                     qPrintable(original.fileName()),
                     qPrintable(backupPath));
            return;
        }
        if (!file.rename(path)) {
            qWarning("Can not rename \"%s\" to \"%s\"",
                     qPrintable(file.fileName()),
                     qPrintable(path));
            original.rename(path);
            return;
        }
        if (!original.remove()) {
            qWarning("Can not remove \"%s\"",
                     qPrintable(original.fileName()));
        }
    }
}

class ConfigData
{
public:
    ConfigData()
    {
        path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                + CONFIG_FILE;

        config = loadConfig();
        lastModified = QFileInfo(path).lastModified();

        watcher.addPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/qutim");
        QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged,
                         [this] (const QString &dirPath) {
            if (!QFileInfo(dirPath).exists()) {
                QDir().mkpath(dirPath);
                watcher.addPath(dirPath);
            }

            auto date = QFileInfo(path).lastModified();
            if (date != lastModified) {
                lastModified = date;
                updateConfig();
            }
        });

        timer.setInterval(500);
        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [this] () {
            saveConfig(config);
            lastModified = QFileInfo(path).lastModified();
        });
    }

    void updateConfig()
    {
        auto tmpConfig = loadConfig();
        qSwap(config, tmpConfig);
        for (auto it = config.begin(); it != config.end(); ++it) {
            if (it.value() != tmpConfig.value(it.key())) {
                for (auto object : configObjects)
                    object->onValueChanged(it.key(), it.value());
            }
        }
    }

    void updateConfig(const QString &name, const QVariant &newValue)
    {
        QVariant &value = config[name];
        if (value != newValue) {
            value = newValue;
            for (auto object : configObjects)
                object->onValueChanged(name, value);

            if (!timer.isActive())
                timer.start();
        }
    }

    QString path;
    QFileSystemWatcher watcher;
    QVariantHash config;
    QDateTime lastModified;
    QSet<Config*> configObjects;
    QTimer timer;
};

static QWeakPointer<ConfigData> weakConfig;

Config::Config(QObject *parent)
    : QObject(parent)
{
    if (weakConfig) {
        m_data = weakConfig.toStrongRef();
    } else {
        m_data.reset(new ConfigData);
        weakConfig = m_data.toWeakRef();
    }
    m_data->configObjects.insert(this);
}

Config::~Config()
{
    m_data->configObjects.remove(this);
}

QString Config::path() const
{
    return m_path;
}

void Config::setPath(const QString &path)
{
    if (m_path != path) {
        m_path = path;
        emit pathChanged(path);
    }
}

void Config::classBegin()
{
}

void Config::componentComplete()
{
    auto meta = metaObject();
    auto configMeta = &Config::staticMetaObject;

    static const auto startIndex = configMeta->indexOfMethod("propery_changed_hook_000()");
    Q_ASSERT(startIndex >= 0);

    const int startPropertyIndex = configMeta->propertyCount();

    for (int i = startPropertyIndex; i < meta->propertyCount(); ++i) {
        auto property = meta->property(i);
        auto slot = meta->method(startIndex + i - startPropertyIndex);
        auto name = m_path + '.' + property.name();

        m_properties[name] = property;
        m_propertiesNames << name;

        connect(this, property.notifySignal(), this, slot);
    }
}


void Config::onPropertyChanged(int index)
{
    auto meta = metaObject();
    auto configMeta = &Config::staticMetaObject;

    auto property = meta->property(configMeta->propertyCount() + index);
    auto value = property.read(this);

    m_data->updateConfig(m_propertiesNames[index], value);

    qDebug() << "Changed property" << index << property.name() << "to value" << value;
}

void Config::onValueChanged(const QString &key, const QVariant &value)
{
    auto it = m_properties.find(key);
    if (it != m_properties.end())
        it.value().write(this, value);
}
