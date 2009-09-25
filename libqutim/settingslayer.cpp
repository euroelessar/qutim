/****************************************************************************
 *  settingslayer.cpp
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

#include "settingslayer.h"
#include <QtGui/QApplication>

namespace qutim_sdk_0_3
{
	SettingsItem::SettingsItem() : m_type(SettingsInvalid), m_text("Settings", 0)
	{
	}

	SettingsItem::~SettingsItem()
	{
		clearWidget();
	}

	SettingsType SettingsItem::type() const
	{
		if(m_type < SettingsGeneral || m_type > SettingsProtocol)
			return SettingsInvalid;
		return m_type;
	}

	QIcon SettingsItem::icon() const
	{
		// If icon is null choose it by type
		return m_icon;
	}

	LocalizedString SettingsItem::text() const
	{
		return m_text;
	}

	QWidget *SettingsItem::widget() const
	{
		if(!m_widget)
			m_widget = generateWidget();
		return m_widget;
	}

	void SettingsItem::clearWidget()
	{
		if(!m_widget.isNull())
			delete m_widget.data();
	}

	QPointer<SettingsLayer> SettingsLayer::instance = 0;

	SettingsLayer::SettingsLayer()
	{
	}

	SettingsLayer::~SettingsLayer()
	{
	}

}
