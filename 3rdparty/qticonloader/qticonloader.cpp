/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/


#include "qticonloader.h"
#include <QtGui/QPixmapCache>

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QLibrary>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>

#ifdef Q_WS_X11

extern "C" {
    struct GConfClient;
    struct GError;
    typedef void (*Ptr_g_type_init)();
    typedef GConfClient* (*Ptr_gconf_client_get_default)();
    typedef char* (*Ptr_gconf_client_get_string)(GConfClient*, const char*, GError **);
    typedef void (*Ptr_g_object_unref)(void *);
    typedef void (*Ptr_g_error_free)(GError *);
    typedef void (*Ptr_g_free)(void*);
    static Ptr_g_type_init p_g_type_init = 0;
    static Ptr_gconf_client_get_default p_gconf_client_get_default = 0;
    static Ptr_gconf_client_get_string p_gconf_client_get_string = 0;
    static Ptr_g_object_unref p_g_object_unref = 0;
    static Ptr_g_error_free p_g_error_free = 0;
    static Ptr_g_free p_g_free = 0;
}


static int kdeVersion()
{
    static int version = qgetenv("KDE_SESSION_VERSION").toInt();
    return version;
}

static QString kdeHome()
{
    static QString kdeHomePath;
    if (kdeHomePath.isEmpty()) {
        kdeHomePath = QFile::decodeName(qgetenv("KDEHOME"));
        if (kdeHomePath.isEmpty()) {
            int kdeSessionVersion = kdeVersion();
            QDir homeDir(QDir::homePath());
            QString kdeConfDir(QLatin1String("/.kde"));
            if (4 == kdeSessionVersion && homeDir.exists(QLatin1String(".kde4")))
                kdeConfDir = QLatin1String("/.kde4");
            kdeHomePath = QDir::homePath() + kdeConfDir;
        }
    }
    return kdeHomePath;
}


static QString systemThemeName()
{
    QString themeName;
#ifdef Q_WS_X11
    // If we are running GNOME we resolve and use GConf. In all other
    // cases we currently use the KDE icon theme
    if (qgetenv("DESKTOP_SESSION") == "gnome" ||
        !qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty()) {

        if (themeName.isEmpty()) {
            // Resolve glib and gconf

            p_g_type_init =              (Ptr_g_type_init)QLibrary::resolve(QLatin1String("gobject-2.0"), 0, "g_type_init");
            p_gconf_client_get_default = (Ptr_gconf_client_get_default)QLibrary::resolve(QLatin1String("gconf-2"), 4, "gconf_client_get_default");
            p_gconf_client_get_string =  (Ptr_gconf_client_get_string)QLibrary::resolve(QLatin1String("gconf-2"), 4, "gconf_client_get_string");
            p_g_object_unref =           (Ptr_g_object_unref)QLibrary::resolve(QLatin1String("gobject-2.0"), 0, "g_object_unref");
            p_g_error_free =             (Ptr_g_error_free)QLibrary::resolve(QLatin1String("glib-2.0"), 0, "g_error_free");
            p_g_free =                   (Ptr_g_free)QLibrary::resolve(QLatin1String("glib-2.0"), 0, "g_free");

            if (p_g_type_init && p_gconf_client_get_default &&
                p_gconf_client_get_string && p_g_object_unref &&
                p_g_error_free && p_g_free) {

                p_g_type_init();
                GConfClient* client = p_gconf_client_get_default();
                GError *err = 0;

                char *str = p_gconf_client_get_string(client, "/desktop/gnome/interface/icon_theme", &err);
                if (!err) {
                    themeName = QString::fromUtf8(str);
                    p_g_free(str);
                }

                p_g_object_unref(client);
                if (err)
                    p_g_error_free (err);

            }
            if (themeName.isEmpty())
                themeName = QLatin1String("gnome");
        }

        if (!themeName.isEmpty())
            return themeName;
    }

    QStringList kdeDirs = QFile::decodeName(getenv("KDEDIRS")).split(QLatin1Char(':'));

    bool kde4 = kdeVersion() >= 4;
    QString defaultPath = kde4 ?
                          QLatin1String("/usr/share/icons/default.kde4") :
                          QLatin1String("/usr/share/icons/default.kde");

    QFileInfo fileInfo(defaultPath);
    QDir dir(fileInfo.canonicalFilePath());
    QString kdeDefault = kde4 ?
                         QString::fromLatin1("oxygen") :
                         QString::fromLatin1("crystalsvg");
    QString defaultTheme = fileInfo.exists() ? dir.dirName() : kdeDefault;

    QSettings settings(kdeHome() + QLatin1String("/share/config/kdeglobals"), QSettings::IniFormat);
    settings.beginGroup(QLatin1String("Icons"));
    themeName = settings.value(QLatin1String("Theme"), defaultTheme).toString();

#endif
    return themeName;
}

struct QIconDirInfo
{
    enum Type { Fixed, Scalable, Threshold };
    QIconDirInfo(const QString &_path = QString()) :
            path(_path),
            size(0),
            maxSize(0),
            minSize(0),
            threshold(0),
            type(Threshold) {}
    QString path;
    short size;
    short maxSize;
    short minSize;
    short threshold;
    Type type : 4;
};

class QIconLoaderEngineEntry
{
public:
    virtual ~QIconLoaderEngineEntry() {}
    QString filename;
    QIconDirInfo dir;
    static int count;
};

struct ScalableEntry : public QIconLoaderEngineEntry
{
    QIcon svgIcon;
};

struct PixmapEntry : public QIconLoaderEngineEntry
{
    QPixmap basePixmap;
};

typedef QList<QIconLoaderEngineEntry*> QThemeIconEntries;

class QIconTheme
{
public:
    QIconTheme(const QString &name);
    QIconTheme() : m_valid(false) {};
    QStringList parents() { return m_parents; }
    QList <QIconDirInfo> keyList() { return m_keyList; }
    QString contentDir() { return m_contentDir; }
    bool isValid() { return m_valid; }

private:
    QString m_contentDir;
    QList <QIconDirInfo> m_keyList;
    QStringList m_parents;
    bool m_valid;
};


class QtIconLoaderImplementation
{
public:
    QtIconLoaderImplementation();
    QIcon loadIcon(const QString &name);

private:
    QString themeName() const { return m_userTheme.isEmpty() ? m_systemTheme : m_userTheme; }

    QThemeIconEntries findIconHelper(const QString &themeName,
                                     const QString &iconName,
                                     QStringList &visited) const;

    uint m_themeKey;
    bool m_supportsSvg;
    mutable QString m_userTheme;
    mutable QString m_systemTheme;
    mutable QStringList m_iconDirs;
    mutable QHash <QString, QIconTheme> themeList;
};

Q_GLOBAL_STATIC(QtIconLoaderImplementation, iconLoaderInstance)

/*!

    Returns the standard icon for the given icon /a name
    as specified in the freedesktop icon spec
    http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    /a fallback is an optional argument to specify the icon to be used if
    no icon is found on the platform. This is particularily useful for
    crossplatform code.

*/
QIcon QtIconLoaderImplementation::loadIcon(const QString &name)
{
    QIcon icon;
    QString theme = themeName();
    if (!theme.isEmpty()) {
        QStringList visited;
        QThemeIconEntries entries = findIconHelper(theme, name, visited);
        for (int i = 0; i < entries.size() ; ++i) {
            int size = entries.at(i)->dir.size;
            icon.addFile(entries.at(i)->filename,
                         QSize(size, size),
                         QIcon::Normal, QIcon::Off);
        }
    }
    return icon;
}

QtIconLoaderImplementation::QtIconLoaderImplementation()
{
    m_systemTheme = systemThemeName();

    /*    QFactoryLoader iconFactoryLoader(QIconEngineFactoryInterfaceV2_iid,
                                     QLatin1String("/iconengines"),
                                     Qt::CaseInsensitive);
    if (iconFactoryLoader.keys().contains(QLatin1String("svg")))
  */
    m_supportsSvg = false;
}

static QString fallbackTheme()
{
    QString defaultTheme = systemThemeName();
    if (defaultTheme.isEmpty())
        defaultTheme = QLatin1String("hicolor");
    return defaultTheme;
}

static QStringList themeSearchPaths()
{
    QStringList iconDirs;
    if (iconDirs.isEmpty()) {

#if defined(Q_WS_X11)

        QString xdgDirString = QFile::decodeName(getenv("XDG_DATA_DIRS"));
        if (xdgDirString.isEmpty())
            xdgDirString = QLatin1String("/usr/local/share/:/usr/share/");

        QStringList xdgDirs = xdgDirString.split(QLatin1Char(':'));

        for (int i = 0 ; i < xdgDirs.size() ; ++i) {
            QDir dir(xdgDirs[i]);
            if (dir.exists())
                iconDirs.append(dir.path() +
                                QLatin1String("/icons"));
        }

        if (kdeVersion() != 0) {

            iconDirs << QLatin1Char(':') +
                    kdeHome() + QLatin1String("/share/icons");
            QStringList kdeDirs =
                    QFile::decodeName(getenv("KDEDIRS")).split(QLatin1Char(':'));

            for (int i = 0 ; i< kdeDirs.count() ; ++i) {
                QDir dir(QLatin1Char(':') + kdeDirs.at(i) +
                         QLatin1String("/share/icons"));
                if (dir.exists())
                    iconDirs.append(dir.path());
            }
        }

        // Add home directory first in search path
        QDir homeDir(QDir::homePath() + QLatin1String("/.icons"));
        if (homeDir.exists())
            iconDirs.prepend(homeDir.path());

#elif defined(Q_WS_WIN)
        m_iconDirs.append(qApp->applicationDirPath() +
                          QLatin1String("/icons"));
#elif defined(Q_WS_MAC)
        m_iconDirs.append(qApp->applicationDirPath() +
                          QLatin1String("/../Resources/icons"));
#endif
    }
    return iconDirs;
}


QIconTheme::QIconTheme(const QString &themeName)
        : m_valid(false)
{
    QFile themeIndex;

    QList <QIconDirInfo> keyList;
    QStringList iconDirs = themeSearchPaths();
    for ( int i = 0 ; i < iconDirs.size() ; ++i) {
        QDir iconDir(iconDirs[i]);
        QString themeDir = iconDir.path() + QLatin1Char('/') + themeName;
        themeIndex.setFileName(themeDir + QLatin1String("/index.theme"));
        if (themeIndex.exists()) {
            m_contentDir = themeDir;
            m_valid = true;
            break;
        }
    }

    if (themeIndex.exists()) {
        const QSettings indexReader(themeIndex.fileName(), QSettings::IniFormat);
        QStringListIterator keyIterator(indexReader.allKeys());
        while (keyIterator.hasNext()) {

            const QString key = keyIterator.next();
            if (key.endsWith(QLatin1String("/Size"))) {
                // Note the QSettings ini-format does not accept
                // slashes in key names, hence we have to cheat
                if (int size = indexReader.value(key).toInt()) {
                    QString directoryKey = key.left(key.size() - 5);
                    QIconDirInfo dirInfo(directoryKey);
                    dirInfo.size = size;
                    QString type = indexReader.value(directoryKey +
                                                     QLatin1String("/Type")
                                                     ).toString();

                    if (type == QLatin1String("Fixed"))
                        dirInfo.type = QIconDirInfo::Fixed;
                    else if (type == QLatin1String("Scalable"))
                        dirInfo.type = QIconDirInfo::Scalable;
                    else
                        dirInfo.type = QIconDirInfo::Threshold;

                    dirInfo.threshold = indexReader.value(directoryKey +
                                                          QLatin1String("/Threshold"),
                                                          2).toInt();

                    dirInfo.minSize = indexReader.value(directoryKey +
                                                        QLatin1String("/MinSize"),
                                                        size).toInt();

                    dirInfo.maxSize = indexReader.value(directoryKey +
                                                        QLatin1String("/MaxSize"),
                                                        size).toInt();
                    m_keyList.append(dirInfo);
                }
            }
        }

        // Parent themes provide fallbacks for missing icons
        m_parents = indexReader.value(
                QLatin1String("Icon Theme/Inherits")).toStringList();

        // Ensure a default platform fallback for all themes
        if (m_parents.isEmpty())
            m_parents.append(fallbackTheme());

        // Ensure that all themes fall back to hicolor
        if (!m_parents.isEmpty())
            m_parents.append(QLatin1String("hicolor"));
    }
}


QThemeIconEntries QtIconLoaderImplementation::findIconHelper(const QString &themeName,
                                                             const QString &iconName,
                                                             QStringList &visited) const
{
    QThemeIconEntries entries;
    Q_ASSERT(!themeName.isEmpty());

    QPixmap pixmap;

    // Used to protect against potential recursions
    visited << themeName;

    QIconTheme theme = themeList.value(themeName);
    if (!theme.isValid()) {
        theme = QIconTheme(themeName);
        if (!theme.isValid())
            theme = fallbackTheme();

        themeList.insert(themeName, theme);
    }

    QString contentDir = theme.contentDir() + QLatin1Char('/');
    QList<QIconDirInfo> subDirs = theme.keyList();

    const QString svgext(QLatin1String(".svg"));
    const QString pngext(QLatin1String(".png"));

    // Add all relevant files
    for (int i = 0; i < subDirs.size() ; ++i) {
        const QIconDirInfo &dirInfo = subDirs.at(i);
        QString subdir = dirInfo.path;
        QDir currentDir(contentDir + subdir);

        if (dirInfo.type == QIconDirInfo::Scalable && m_supportsSvg &&
            currentDir.exists(iconName + svgext)) {
            ScalableEntry *iconEntry = new ScalableEntry;
            iconEntry->dir = dirInfo;
            iconEntry->filename = currentDir.filePath(iconName + svgext);
            entries.append(iconEntry);

        } else if (currentDir.exists(iconName + pngext)) {
            PixmapEntry *iconEntry = new PixmapEntry;
            iconEntry->dir = dirInfo;
            iconEntry->filename = currentDir.filePath(iconName + pngext);
            // Notice we ensure that pixmap entries allways come before
            // scalable to preserve search order afterwards
            entries.prepend(iconEntry);
        }
    }

    if (entries.isEmpty()) {
        const QStringList parents = theme.parents();
        // Search recursively through inherited themes
        for (int i = 0 ; i < parents.size() ; ++i) {

            const QString parentTheme = parents.at(i).trimmed();

            if (!visited.contains(parentTheme)) // guard against recursion
                entries = findIconHelper(parentTheme, iconName, visited);

            if (!entries.isEmpty()) // success
                break;
        }
    }
    return entries;
}

#endif //Q_WS_X11

QIcon QtIconLoader::icon(const QString &name, const QIcon &fallback)
{
    QIcon icon;
#ifdef Q_WS_X11
    icon = iconLoaderInstance()->loadIcon(name);
#endif //Q_WS_X11
    if (icon.isNull())
        icon = fallback;
    Q_UNUSED(name);
    return icon;
}
