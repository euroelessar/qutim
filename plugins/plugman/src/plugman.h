/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef PLUGMAN_H
#define PLUGMAN_H
#include <qutim/plugin.h>
#include <attica/provider.h>
#include <attica/providermanager.h>
#include <QNetworkAccessManager>
//#include "packageengine.h"

namespace qutim_sdk_0_3 {
class ActionGenerator;
}

namespace PackageManager
{
class PackageManagerPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();

public slots:
	void onManagerButtonClicked();

private:
	qutim_sdk_0_3::ActionGenerator *m_gen;
//	PackageEngine *m_emoticonsEngine;
//	PackageEngine *m_stylesEngine;
};
}

#endif

