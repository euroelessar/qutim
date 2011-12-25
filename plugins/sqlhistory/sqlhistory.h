/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#ifndef SQLHISTORY_H
#define SQLHISTORY_H

#include "sqlengine.h"

using namespace qutim_sdk_0_2;

class SqlHistory : public QObject, public LayerPluginInterface {
	Q_OBJECT
	Q_DISABLE_COPY(SqlHistory)
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface qutim_sdk_0_2::LayerPluginInterface)
public:
	SqlHistory(QObject *parent = 0);
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release();
	virtual QString name();
	virtual QString description();
	virtual QIcon *icon();
	virtual void setProfileName(const QString &profile_name);
public slots:
	void itemAdded(const TreeModelItem &item, const QString &name);
private:
	SqlHistoryNamespace::SqlEngine *m_engine;
	QIcon *m_icon;
};

#endif // SQLHISTORY_H

