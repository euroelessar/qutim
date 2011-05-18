/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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
