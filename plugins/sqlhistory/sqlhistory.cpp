/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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

#include "sqlhistory.h"
#include <qutim/iconmanagerinterface.h>

SqlHistory::SqlHistory(QObject *parent) :
	QObject(parent)
{
	m_engine = 0;
	m_icon = 0;
}

bool SqlHistory::init(PluginSystemInterface *plugin_system)
{
	PluginInterface::init(plugin_system);
	m_engine = new SqlHistoryNamespace::SqlEngine;
	if(!plugin_system->setLayerInterface(HistoryLayer, m_engine))
	{
		delete m_engine;
		m_engine = 0;
		return false;
	}
	plugin_system->registerEventHandler("Core/ContactList/ItemAdded", this, SLOT(itemAdded(TreeModelItem,QString)));
	return true;
}

void SqlHistory::release()
{
	delete m_engine;
	m_engine = 0;
}

QString SqlHistory::name()
{
	return "SqlHistory";
}

QString SqlHistory::description()
{
	return "SQL History engine";
}

QIcon *SqlHistory::icon()
{
	return m_icon;
}

void SqlHistory::setProfileName(const QString &profile_name)
{
	SystemsCity::instance().setProfileName(profile_name);
	m_icon = new Icon("history");
}

void SqlHistory::itemAdded(const TreeModelItem &item, const QString &)
{
	if(m_engine)
		m_engine->setHistory(item);
}

Q_EXPORT_PLUGIN2(sqlhistory, SqlHistory)

