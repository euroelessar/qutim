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

	class LIBQUTIM_EXPORT SettingsItem
	{
		Q_DISABLE_COPY(SettingsItem);
	public:
		SettingsItem();
		SettingsItem(SettingsType type, const QIcon &icon, const char *text)
			: m_type(type), m_icon(icon), m_text("Settings", text) {}
		SettingsItem(SettingsType type, const char *text)
			: m_type(type), m_text("Settings", text) {}
		virtual ~SettingsItem();
		SettingsType type() const;
		QIcon icon() const;
		LocalizedString text() const;
		QWidget *widget() const;
		void clearWidget();
	protected:
		virtual QWidget *generateWidget() const = 0;
		SettingsType m_type;
		QIcon m_icon;
		LocalizedString m_text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
		mutable QPointer<QWidget> m_widget;
	};

	template<typename T>
	class LIBQUTIM_EXPORT SettingsWidget : public SettingsItem
	{
	protected:
		virtual QWidget *generateWidget() const
		{
			return new T();
		}
	};

	class LIBQUTIM_EXPORT SettingsLayer : public QObject
	{
		Q_OBJECT
	public:
	protected:
		SettingsLayer();
		virtual ~SettingsLayer();
	private:
		static QPointer<SettingsLayer> instance;
	};
}

#endif // SETTINGSLAYER_H
