/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef PLUGIN_H
#define PLUGIN_H

#include "libqutim_version.h"
#include "extensioninfo.h"
#include <QtCore/QtPlugin>
#include <QtCore/QObject>
#include <QtCore/QSharedDataPointer>

#define PLUGIN_VERSION(major, minor, secminor, patch) QUTIM_MAKE_VERSION(major, minor, secminor, patch)

namespace qutim_sdk_0_3
{
class ModuleManager;
class PluginInfoData;
class PluginPrivate;

LIBQUTIM_EXPORT void debugAddPluginId(quint64, const QMetaObject *meta);

class LIBQUTIM_EXPORT PluginInfo
{
public:
    // Keep in sync with Plugin::Capability
    enum Capability
    {
        Loadable = 0x01
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)

    PluginInfo(const LocalizedString &name = LocalizedString(),
               const LocalizedString &description = LocalizedString(),
               quint32 version = 0, ExtensionIcon icon = ExtensionIcon());
    PluginInfo(const PluginInfo &other);
    ~PluginInfo();
    PluginInfo &operator =(const PluginInfo &other);
    PluginInfo &addAuthor(const PersonInfo &author);
    PluginInfo &addAuthor(const QString &ocsUsername);
    PluginInfo &addAuthor(const LocalizedString &name, const LocalizedString &task,
                          const QString &email = QString(), const QString &web = QString());
    PluginInfo &setName(const LocalizedString &name);
    PluginInfo &setDescription(const LocalizedString &description);
    PluginInfo &setIcon(const ExtensionIcon &icon);
    PluginInfo &setVersion(quint32 version);
    Capabilities capabilities() const;
    PluginInfo &setCapabilities(Capabilities capabilities);
    QList<PersonInfo> authors() const;
    LocalizedString name() const;
    LocalizedString description() const;
    quint32 version() const;
    ExtensionIcon icon() const;
#ifndef Q_QDOC
    QString fileName() const;
private:
    QSharedDataPointer<PluginInfoData> d;
public:
    typedef PluginInfoData Data;
    Data *data();
#endif
};

class CommandArgumentPrivate;

class LIBQUTIM_EXPORT CommandArgument
{
public:
	enum Type {
		NoValue = 0,                /*!< The option does not accept a value. */
		ValueOptional = 1,          /*!< The option may accept a value. */
		ValueRequired = 2,          /*!< The option requires a value. */
		AllowMultiple = 4,          /*!< The option may be passed multiple times. */
		Undocumented = 8            /*!< The option is not output in the help text. */
	};
	Q_DECLARE_FLAGS(Types, Type)

	CommandArgument(const QString &name, const QString &desc = QString(), Types types = NoValue, int group = -1);
	CommandArgument(const CommandArgument &o);
	CommandArgument &operator =(const CommandArgument &o);
	~CommandArgument();

	QString name() const;
	QString description() const;
	Types types() const;
	int group() const;
	void setAliases(const QStringList &aliases);
	void addAlias(const QStringList &alias);
	QStringList aliases() const;

private:
	QSharedPointer<CommandArgumentPrivate> d;
};

class LIBQUTIM_EXPORT CommandArgumentsHandler
{
public:
	virtual ~CommandArgumentsHandler();

	virtual QStringList commands() = 0;
	virtual QList<CommandArgument> commandOptions(const QString &name) = 0;
};

#ifndef Q_QDOC
typedef void ( /*QObject::*/ *ModuleInit)();
#endif

class LIBQUTIM_EXPORT Plugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Plugin)
    Q_DECLARE_PRIVATE(Plugin)
public:
    // Keep in sync with PluginInfo::Capability
    enum Capability
    {
        Loadable = 0x01
    };
    Q_DECLARE_FLAGS(Capabilities, Capability)
    Plugin();
    virtual ~Plugin();
    PluginInfo info() const;
    ExtensionInfoList avaiableExtensions() const;
    virtual void init() = 0;
    virtual bool load() = 0;
    virtual bool unload() = 0;
protected:
    // Should be called at init
    void addAuthor(const LocalizedString &name, const LocalizedString &task,
                   const QString &email = QString(), const QString &web = QString());
    void addAuthor(const QString &ocsUsername);
    void setInfo(const LocalizedString &name, const LocalizedString &description,
                 quint32 version = 0, ExtensionIcon icon = ExtensionIcon());
    void setCapabilities(Capabilities capabilities);
    void addExtension(const LocalizedString &name, const LocalizedString &description,
                      const ObjectGenerator *generator, ExtensionIcon icon = ExtensionIcon());
    template<typename T>
    void addExtension(const LocalizedString &name, const LocalizedString &description,
                      ExtensionIcon icon = ExtensionIcon())
    { addExtension(name, description, new GeneralGenerator<T>(), icon); }
    template<typename T, typename I0>
    void addExtension(const LocalizedString &name, const LocalizedString &description,
                      ExtensionIcon icon = ExtensionIcon())
    { addExtension(name, description, new GeneralGenerator<T, I0>(), icon); }
    template<typename T, typename I0, typename I1>
    void addExtension(const LocalizedString &name, const LocalizedString &description,
                      ExtensionIcon icon = ExtensionIcon())
    { addExtension(name, description, new GeneralGenerator<T, I0, I1>(), icon); }
#ifndef Q_QDOC
private:
    QScopedPointer<PluginPrivate> p;
    friend class ModuleManager;
#endif
};

class LIBQUTIM_EXPORT PluginFactory
{
public:
    virtual ~PluginFactory() {}
    // Will be called after ::load() method
    virtual QList<Plugin*> loadPlugins() = 0;
};

LIBQUTIM_EXPORT QList<QPointer<Plugin> > pluginsList();
}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::PluginInfo::Capabilities)
Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::Plugin::Capabilities)

#ifdef QUTIM_STATIC_PLUGIN

# define QUTIM_DEBUG_ID_CONVERT_HELPER(A) 0x ## A ## ULL
# define QUTIM_DEBUG_ID_CONVERT(A) QUTIM_DEBUG_ID_CONVERT_HELPER(A)
# define QUTIM_PLUGIN_INSTANCE_BODY(IMPLEMENTATION) \
		{ \
			static QT_PREPEND_NAMESPACE(QPointer)<QT_PREPEND_NAMESPACE(QObject)> _instance; \
			if (!_instance) {      \
				_instance = new IMPLEMENTATION; \
				debugAddPluginId(QUTIM_DEBUG_ID_CONVERT(QUTIM_PLUGIN_ID), &IMPLEMENTATION::staticMetaObject);\
			} \
			return _instance; \
		}
# define QUTIM_EXPORT_PLUGIN_HELPER(PluginInstance, Class) \
    QT_PREPEND_NAMESPACE(QObject) \
    *PluginInstance() \
	QUTIM_PLUGIN_INSTANCE_BODY(Class)
# undef QUTIM_DEBUG_ID_CONVERT_HELPER
# undef QUTIM_DEBUG_ID_CONVERT

# define QUTIM_EXPORT_PLUGIN(Class) \
	QUTIM_EXPORT_PLUGIN_HELPER(QUTIM_PLUGIN_INSTANCE_BODY, Class)
# define QUTIM_EXPORT_PLUGIN2(Plugin,Class) \
    QUTIM_EXPORT_PLUGIN(Class)
#else
# define QUTIM_EXPORT_STRING_HELPER(ID) #ID
# define QUTIM_EXPORT_STRING(ID) QUTIM_EXPORT_STRING_HELPER(ID)
# define QUTIM_EXPORT_PLUGIN2(Plugin,Class) \
    Q_EXPORT_PLUGIN2(Plugin,Class) \
    static const char *qutim_plugin_verification_data = \
    "pattern=""QUTIM_PLUGIN_VERIFICATION_DATA""\n" \
	"debugid=" QUTIM_EXPORT_STRING(QUTIM_PLUGIN_ID)"\n" \
	"libqutim=" QUTIM_VERSION_STRING"\0"; \
    Q_EXTERN_C Q_DECL_EXPORT \
    const char * Q_STANDARD_CALL qutim_plugin_query_verification_data() \
{ return qutim_plugin_verification_data; }

#define QUTIM_EXPORT_PLUGIN(Plugin) \
    QUTIM_EXPORT_PLUGIN2(Plugin, Plugin)
#endif		
Q_DECLARE_INTERFACE(qutim_sdk_0_3::PluginFactory, "org.qutim.PluginFactory")
Q_DECLARE_INTERFACE(qutim_sdk_0_3::CommandArgumentsHandler, "org.qutim.CommandArgumentsHandler")

#endif // PLUGIN_H

