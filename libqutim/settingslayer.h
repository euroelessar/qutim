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
	enum SettingsType
	{
		SettingsInvalid,
		SettingsGeneral,
		SettingsAppearance,
		SettingsPlugin,
		SettingsProtocol
	};

	// Exmample of usage:
	// Settings::registerItem(new SettingsWidget<MyCoolPopupSettings>(
	//                        SettingsAppearance,
	//                        QIcon(":/icons/mycoolicon"),
	//                        QT_TRANSLATE_NOOP_UTF8("Settings", "Popup theme")));

	class LIBQUTIM_EXPORT SettingsItem
	{
		Q_DISABLE_COPY(SettingsItem);
	public:
		SettingsItem()
				: m_gen(0), m_type(SettingsInvalid), m_text("Settings", 0) {}
		SettingsItem(SettingsType type, const QIcon &icon, const char *text)
				: m_gen(0), m_type(type), m_icon(icon), m_text("Settings", text) {}
		SettingsItem(SettingsType type, const char *text)
				: m_gen(0), m_type(type), m_text("Settings", text) {}
		virtual ~SettingsItem();
		SettingsType type() const;
		QIcon icon() const;
		LocalizedString text() const;
		QWidget *widget() const;
		void clearWidget();
	protected:
		virtual const ObjectGenerator *generator() const = 0;
		mutable const ObjectGenerator *m_gen;
		SettingsType m_type;
		QIcon m_icon;
		LocalizedString m_text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
		mutable QPointer<QWidget> m_widget;
	};

	template<typename T>
	class LIBQUTIM_EXPORT GeneralSettingsItem : public SettingsItem
	{
	protected:
		virtual const ObjectGenerator *generator() const
		{
			// T must be based on QWidget
			register QWidget *widget = reinterpret_cast<T *>(0);
			return new GeneralGenerator<T>();
		}
	};

	typedef QList<SettingsItem *> SettingsItemList;

	namespace Settings
	{
		LIBQUTIM_EXPORT void registerItem(SettingsItem *item);
		LIBQUTIM_EXPORT void showWidget();
		LIBQUTIM_EXPORT void closeWidget();
	}

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
