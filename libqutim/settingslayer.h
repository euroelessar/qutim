/****************************************************************************
 *  settingslayer.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SETTINGSLAYER_H
#define SETTINGSLAYER_H

#include "localizedstring.h"
#include "objectgenerator.h"
#include <QPointer>
#include <QWidget>
#include <QIcon>
#include <QComboBox>

namespace qutim_sdk_0_3
{
	class SettingsWidget;
	class SettingsItem;
	class SettingsItemPrivate;
	class MenuController;

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
		//TODO,
//		LIBQUTIM_EXPORT TypeEntryMap *entries();
//		LIBQUTIM_EXPORT quint16 registerType(const char *id,
//													const QIcon &icon,
//													const LocalizedString &text,
//													const LocalizedString &description = LocalizedString());
	}
	
	class LIBQUTIM_EXPORT SettingsItem
	{
		Q_DISABLE_COPY(SettingsItem);
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
		QScopedPointer<SettingsItemPrivate> p;
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

	struct AutoSettingsItemPrivate;

	class LIBQUTIM_EXPORT AutoSettingsItem : public SettingsItem
	{
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

	typedef QList<SettingsItem *> SettingsItemList;

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
