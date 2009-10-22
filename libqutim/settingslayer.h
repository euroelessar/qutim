/****************************************************************************
 *  settingslayer.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace qutim_sdk_0_3
{
	class SettingsWidget;
	class SettingsItem;
	namespace Settings
	{
		enum Type
		{
			Invalid,
			General,
			Appearance,
			Plugin,
			Protocol
		};

		// Exmample of usage:
		// Settings::registerItem(new SettingsWidget<MyCoolPopupSettings>(
		//                        Settings::Appearance,
		//                        QIcon(":/icons/mycoolicon"),
		//                        QT_TRANSLATE_NOOP_UTF8("Settings", "Popup theme")));

		LIBQUTIM_EXPORT void registerItem(SettingsItem *item);
		LIBQUTIM_EXPORT void showWidget();
		LIBQUTIM_EXPORT void closeWidget();
	}
	
	class LIBQUTIM_EXPORT SettingsItem
	{
		Q_DISABLE_COPY(SettingsItem);
	public:
		SettingsItem()
				: m_gen(0), m_type(Settings::Invalid), m_text("Settings", 0) {}
		SettingsItem(Settings::Type type, const QIcon &icon, const char *text)
				: m_gen(0), m_type(type), m_icon(icon), m_text("Settings", text) {}
		SettingsItem(Settings::Type type, const char *text)
				: m_gen(0), m_type(type), m_text("Settings", text) {}
		virtual ~SettingsItem();
		Settings::Type type() const;
		QIcon icon() const;
		LocalizedString text() const;
		SettingsWidget *widget() const;
		void clearWidget();
	protected:
		virtual const ObjectGenerator *generator() const = 0;
		mutable const ObjectGenerator *m_gen;
		Settings::Type m_type;
		QIcon m_icon;
		LocalizedString m_text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
		mutable QPointer<SettingsWidget> m_widget;
	};

	template<typename T>
	class LIBQUTIM_EXPORT GeneralSettingsItem : public SettingsItem
	{
	public:
		GeneralSettingsItem(Settings::Type type, const QIcon &icon, const char *text)
				: SettingsItem(type,icon,text) {}
		GeneralSettingsItem(Settings::Type type, const char *text)
				: SettingsItem(type,text) {}
	protected:
		virtual const ObjectGenerator *generator() const
		{
			// T must be based on QWidget
			register QWidget *widget = reinterpret_cast<T *>(0);
			return new GeneralGenerator<T>();
		}
	};

	typedef QList<SettingsItem *> SettingsItemList;

	class LIBQUTIM_EXPORT SettingsLayer : public QObject
	{
		Q_OBJECT
	public:
		virtual void show(const SettingsItemList &settings) = 0;
		virtual void close() = 0;
	protected:
		SettingsLayer();
		virtual ~SettingsLayer();
	};
}

#endif // SETTINGSLAYER_H
