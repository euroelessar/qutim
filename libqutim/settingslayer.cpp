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
#include "objectgenerator.h"
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
		// TODO: If icon is null choose it by type
		return m_icon;
	}

	LocalizedString SettingsItem::text() const
	{
		return m_text;
	}

	QWidget *SettingsItem::widget() const
	{
		if(!m_gen)
			m_gen = generator();
		if(m_gen && m_widget.isNull())
			m_widget = m_gen->generate<QWidget>();
		return m_widget;
	}

	void SettingsItem::clearWidget()
	{
		if(!m_widget.isNull())
			delete m_widget.data();
	}

	SettingsLayer::SettingsLayer()
	{
	}

	SettingsLayer::~SettingsLayer()
	{
	}

	namespace Settings
	{
		struct SettingsPrivate
		{
			SettingsItemList items;
			QPointer<SettingsLayer> widget;
			const ObjectGenerator *gen;
			bool inited;
		};

		static SettingsPrivate *p = 0;

		void ensure_settings_private_helper()
		{
			p = new SettingsPrivate;
			p->gen = 0;
			p->inited = false;
		}

		inline void ensure_settings_private()
		{ if(!p) ensure_settings_private_helper();}

		void registerItem(SettingsItem *item)
		{
			ensure_settings_private();
			p->items.append(item);
		}

		void showWidget()
		{
			ensure_settings_private();
			if(isCoreInited() && p->widget.isNull())
			{
				if(!p->inited)
				{
					GeneratorList layers = moduleGenerators<SettingsLayer>();
					p->gen = layers.size() ? layers.first() : 0;
					p->inited = true;
				}
				if(p->gen)
					p->widget = p->gen->generate<SettingsLayer>();
			}
			if(!p->widget.isNull())
				p->widget->show(p->items);
			else
				qWarning("There is no '%s' module", SettingsLayer::staticMetaObject.className());
		}

		void closeWidget()
		{
			ensure_settings_private();
			if(!p->widget.isNull())
				p->widget->close();
		}
	}

}
