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

#ifndef SETTINGSLAYER_H
#define SETTINGSLAYER_H

#include "localizedstring.h"
#include "objectgenerator.h"
#include <QWeakPointer>
#include <QWidget>
#include <QIcon>
#include <QComboBox>

namespace qutim_sdk_0_3
{
class SettingsWidget;
class SettingsItem;
class SettingsItemPrivate;
class MenuController;
class DataItem;

typedef QList<SettingsItem *> SettingsItemList;

namespace Settings
{
	enum Type
	{
		Invalid = 0,
		General,
		Protocol,
		Appearance,
		Plugin,
		Special
	};

//		struct TypeEntry
//		{
//			TypeEntry(const QIcon &icon,const LocalizedString &title,const LocalizedString &description = LocalizedString());
//			TypeEntry() {}
//			LocalizedString text;
//			QIcon icon;
//		};

//		typedef QMap<Settings::Type,TypeEntry> TypeEntryMap;

	// Exmample of usage:
	// Settings::registerItem(new SettingsWidget<MyCoolPopupSettings>(
	//                        Settings::Appearance,
	//                        QIcon(":/icons/mycoolicon"),
	//                        QT_TRANSLATE_NOOP_UTF8("Settings", "Popup theme")));

	LIBQUTIM_EXPORT void registerItem(SettingsItem *item);
	LIBQUTIM_EXPORT void removeItem(SettingsItem *item);
	LIBQUTIM_EXPORT void showWidget();
	LIBQUTIM_EXPORT void closeWidget();
	LIBQUTIM_EXPORT LocalizedString getTypeTitle(Type type);
	LIBQUTIM_EXPORT QIcon getTypeIcon(Type type);
	/*!
	  Add settings \a item to every object with QMetaObject \a meta.
	*/
	LIBQUTIM_EXPORT void registerItem(SettingsItem *item, const QMetaObject *meta);
	/*!
	  Add settings \a item to every object of type \a T.
	*/
	template <typename T>
	Q_INLINE_TEMPLATE void registerItem(SettingsItem *item) { registerItem(item,&T::staticMetaObject); }
	
	/*!
	  Returns list of every SettingsItem with QMetaObject \a meta, \warning each generated widget settings, should have a menucontroller as parent.
	*/
	SettingsItemList items(const QMetaObject *meta);
	template <typename T>
	Q_INLINE_TEMPLATE SettingsItemList items() { return items(&T::staticMetaObject); }
	//TODO,
//		LIBQUTIM_EXPORT TypeEntryMap *entries();
//		LIBQUTIM_EXPORT quint16 registerType(const char *id,
//													const QIcon &icon,
//													const LocalizedString &text,
//													const LocalizedString &description = LocalizedString());
}

class LIBQUTIM_EXPORT SettingsItem
{
	Q_DISABLE_COPY(SettingsItem)
	Q_DECLARE_PRIVATE(SettingsItem)
public:
	SettingsItem(SettingsItemPrivate &d);
	SettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text);
	SettingsItem(Settings::Type type, const LocalizedString &text);
	virtual ~SettingsItem();
	Settings::Type type() const;
	QIcon icon() const;
	LocalizedString text() const;
	SettingsWidget *widget() const;
	void clearWidget();
	void connect(const char *signal, QObject *receiver, const char *member);
	int priority() const;
	void setPriority(int priority);
protected:
	virtual const ObjectGenerator *generator() const = 0;
	QScopedPointer<SettingsItemPrivate> d_ptr;
};

template<typename T>
class GeneralSettingsItem : public SettingsItem
{
public:
	GeneralSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text)
			: SettingsItem(type, icon, text) {}
	GeneralSettingsItem(Settings::Type type, const LocalizedString &text)
			: SettingsItem(type, text) {}
	virtual ~GeneralSettingsItem() {}
protected:
	virtual const ObjectGenerator *generator() const
	{
		// T must be based on SettingsWidget
		register SettingsWidget *widget = reinterpret_cast<T *>(0);
		Q_UNUSED(widget);
		return new GeneralGenerator<T>();
	}
};

class AutoSettingsItemPrivate;
class LIBQUTIM_EXPORT AutoSettingsItem : public SettingsItem
{
	Q_DECLARE_PRIVATE(AutoSettingsItem)
public:
	class EntryPrivate;
	class LIBQUTIM_EXPORT Entry
	{
	public:
		Entry(const LocalizedString &text, const ObjectGenerator *gen);
		virtual ~Entry();
		Entry *setProperty(const char *name, QVariant value);
		Entry *setName(const QString &name);
		const LocalizedString &text() const;
		const ObjectGenerator *generator() const;
		QWidget *widget(QWidget *parent = 0) const;
		const QString &name() const;
	private:
		QScopedPointer<EntryPrivate> p;
	};
	AutoSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text);
	AutoSettingsItem(Settings::Type type, const LocalizedString &text);
	virtual ~AutoSettingsItem();
	void setConfig(const QString &config, const QString &group);
	Entry *addEntry(const LocalizedString &text, const ObjectGenerator *gen);
	template <typename T>
	Entry *addEntry(const LocalizedString &text)
	{
		register QWidget *widget = reinterpret_cast<T *>(0);
		Q_UNUSED(widget);
		return addEntry(text, new GeneralGenerator<T>());
	}
private:
	virtual const ObjectGenerator *generator() const;
};

class LIBQUTIM_EXPORT AutoSettingsComboBox : public QComboBox
{
	Q_PROPERTY(QStringList items READ items WRITE setItems)
	Q_OBJECT
public:
	inline AutoSettingsComboBox() {}
	QStringList items() const;
	void setItems(const QStringList &ls);
};

class AutoSettingsFileChooserPrivate;
class LIBQUTIM_EXPORT AutoSettingsFileChooser : public QWidget
{
	Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged USER true)
	Q_OBJECT
	Q_DECLARE_PRIVATE(AutoSettingsFileChooser)
public:
	AutoSettingsFileChooser(QWidget *parent = 0);
	~AutoSettingsFileChooser();
	QString path() const;
	void setPath(const QString &p);
signals:
	void pathChanged(const QString &path);
private:
	QScopedPointer<AutoSettingsFileChooserPrivate> d_ptr;
};

class DataSettingsItemPrivate;
class LIBQUTIM_EXPORT DataSettingsItem : public SettingsItem
{
	Q_DECLARE_PRIVATE(DataSettingsItem)
public:
	DataSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text);
	DataSettingsItem(Settings::Type type, const LocalizedString &text);
	virtual ~DataSettingsItem();
	void setConfig(const QString &config, const QString &group);
	void setDataItem(const DataItem &item);
private:
	virtual const ObjectGenerator *generator() const;
};

class LIBQUTIM_EXPORT SettingsLayer : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SettingsLayer")
public:
	virtual void show(const SettingsItemList &settings, QObject *controller = 0) = 0;
	virtual void close(QObject* controller = 0) = 0;
	virtual void update(const SettingsItemList &settings, QObject *controller = 0) = 0;
	void show (MenuController *controller);
protected:
	SettingsLayer();
	virtual ~SettingsLayer();
	virtual void virtual_hook(int id, void *data);
};
}

#endif // SETTINGSLAYER_H

