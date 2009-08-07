#ifndef PLUGIN_H
#define PLUGIN_H

#include "libqutim_global.h"
#include <QObject>
#include <QIcon>
#include <QSharedDataPointer>

#define PLUGIN_VERSION QUTIM_VERSION_CHECK

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT PersonInfoData : public QSharedData
	{
	public:
		PersonInfoData();
		PersonInfoData(const PersonInfoData &other);
		const char *name;
		const char *task;
		QString email;
		QString web;
	};

	class LIBQUTIM_EXPORT PersonInfo
	{
	public:
		PersonInfo(const char *name = 0, const char *task = 0,
				   const QString &email = QString(), const QString &web = QString());
		inline PersonInfo(const PersonInfo &other) : d(other.d) {}
		inline PersonInfo &setName(const char *name) { d->name = name; return *this; }
		inline PersonInfo &setTask(const char *task) { d->task = task; return *this; }
		inline PersonInfo &setEmail(const QString &email) { d->email = email; return *this; }
		inline PersonInfo &setWeb(const QString &web) { d->web = web; return *this; }
		QString name() const;
		QString task() const;
		inline const QString &email() const { return d->email; }
		inline const QString &web() const { return d->web; }
	private:
		QSharedDataPointer<PersonInfoData> d;
	};
	/*
	 * Encoding is utf-8, language russian
	 * PersonInfo author = PersonInfo(QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff"),
	 *								  QT_TRANSLATE_NOOP("Task", "Plugin author"),
	 *								  "v.sidorov@proger.ru",
	 *								  "http://proger.ru/v.sidorov");
	 * author.name(); // "Иван Сидоров"
	 * author.task(); // "Автор плагина"
	 */

//	struct PersonInfo
//	{
//		// Full name, insert by QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff")
//		const char *name;
//		// Devel task, insert by QT_TRANSLATE_NOOP("Task", "Plugin author")
//		const char *task;
//		// E-mail address
//		QString email_address;
//		// Web address, if exists
//		QString web_address;
//	};

	class LIBQUTIM_EXPORT PluginInfoData : public QSharedData
	{
	public:
		PluginInfoData();
		PluginInfoData(const PluginInfoData &other);
		QList<PersonInfo> authors;
		const char *name;
		const char *description;
		quint32 version;
		QIcon icon;
	};

	class LIBQUTIM_EXPORT PluginInfo
	{
	public:
		PluginInfo(const char *name = 0, const char *description = 0,
				   quint32 version = 0, QIcon icon = QIcon());
		inline PluginInfo(const PluginInfo &other) : d(other.d) {}
		PluginInfo &addAuthor(const PersonInfo &author);
		PluginInfo &addAuthor(const char *name, const char *task,
					   const QString &email = QString(), const QString &web = QString());
		inline PluginInfo &setName(const char *name) { d->name = name; return *this; }
		inline PluginInfo &setDescription(const char *description) { d->description = description; return *this; }
		inline PluginInfo &setIcon(const QIcon &icon) { d->icon = icon; return *this; }
		inline PluginInfo &setVersion(quint32 version) { d->version = version; return *this; }
		inline QList<PersonInfo> authors() const { return d->authors; }
		QString name() const;
		QString description() const;
		inline quint32 version() const { return d->version; }
		inline QIcon icon() const { return d->icon; }
	private:
		QSharedDataPointer<PluginInfoData> d;
	};

//	struct PluginInfo
//	{
//		// Authors
//		QList<PersonInfo> authors;
//		// Name, insert by QT_TRANSLATE_NOOP("Plugin", "My cool plugin")
//		const char *name;
//		// Html description, insert by QT_TRANSLATE_NOOP("Plugin", "This is my first plugin")
//		const char *description;
//		// Can be accessed by Icon( name, Icon::Plugin );
//		QIcon icon;
//		// Format is like QT_VERSION, but contains 4 numbers, i.e. 0x00010405 for version 0.1.4.5
//		quint32 version;
//	};

	class ExtensionInfoData : public QSharedData
	{
	public:
		ExtensionInfoData();
		ExtensionInfoData(const ExtensionInfoData &other);
		const char *name;
		const char *description;
		const QMetaObject *meta;
		QIcon icon;
	};

	class ExtensionInfo
	{
	public:
		ExtensionInfo(const char *name = 0, const char *description = 0,
					  const QMetaObject *meta = 0, QIcon icon = QIcon());
		inline ExtensionInfo(const ExtensionInfo &other) : d(other.d) {}
		inline ExtensionInfo &setName(const char *name) { d->name = name; return *this; }
		inline ExtensionInfo &setDescription(const char *description) { d->description = description; return *this; }
		inline ExtensionInfo &setIcon(const QIcon &icon) { d->icon = icon; return *this; }
		inline ExtensionInfo &setMeta(const QMetaObject *meta) { d->meta = meta; return *this; }
		QString name() const;
		QString description() const;
		inline const QMetaObject *meta() const { return d->meta; }
		inline QIcon icon() const { return d->icon; }
	private:
		QSharedDataPointer<ExtensionInfoData> d;
	};

//	struct ExtensionInfo
//	{
//		// Extension icon, if null core would try to detect it by extension type
//		QIcon icon;
//		// Name, insert by QT_TRANSLATE_NOOP("Plugin", "Kickoff settings layer")
//		const char *name;
//		// Html description, insert by QT_TRANSLATE_NOOP("Plugin", "Make kickoff-like settings window")
//		const char *description;
//		// Meta object of Extension, i.e. &KickoffSettingsLayer::staticMetaObject();
//		const QMetaObject *meta;
//	};

	class LIBQUTIM_EXPORT Plugin : public QObject
	{
		Q_OBJECT
	public:
		Plugin();
		inline const PluginInfo &info() const { return m_info; }
		const QList<ExtensionInfo> &avaiableExtensions() const { return m_extensions; }
		virtual void init() = 0;
		virtual bool load() = 0;
		virtual bool unload() = 0;
	protected:
		// Should be filled at init
		PluginInfo m_info;
		QList<ExtensionInfo> m_extensions;
	};
}

#endif // PLUGIN_H
