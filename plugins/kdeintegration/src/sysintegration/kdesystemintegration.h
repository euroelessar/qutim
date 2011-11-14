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

#ifndef KDESYSTEMINTEGRATION_H
#define KDESYSTEMINTEGRATION_H

#include <qutim/systemintegration.h>

class KdeSystemIntegration : public qutim_sdk_0_3::SystemIntegration
{
    Q_OBJECT
public:
    KdeSystemIntegration();
	
	virtual void init();
	virtual bool isAvailable() const;
	virtual int priority();

protected:
	virtual QVariant doGetValue(Attribute attr, const QVariant &data) const;
	virtual QVariant doProcess(Operation act, const QVariant &data) const;
	virtual bool canHandle(Attribute attribute) const;
	virtual bool canHandle(Operation operation) const;
};

#endif // KDESYSTEMINTEGRATION_H

