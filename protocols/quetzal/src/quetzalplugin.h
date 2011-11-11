/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PURPLEPROTOCOLPLUGIN_H
#define PURPLEPROTOCOLPLUGIN_H

#include <qutim/protocol.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class QuetzalPlugin : public Plugin
{
    Q_OBJECT
	Q_CLASSINFO("DebugName", "quetzal")
public:
	virtual void init();
	virtual bool load() { return true; }
	virtual bool unload() { return false; }
	void clearTemporaryDir();
private slots:
	void onFinished(void *data);
private:
	void initLibPurple();
//	QString m_tmpDir;
};

#endif // PURPLEPROTOCOLPLUGIN_H

