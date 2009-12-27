/****************************************************************************
 *  accountcreator.h
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include "qutim/protocol.h"

namespace Icq {

using namespace qutim_sdk_0_3;

struct IcqAccWizardPrivate;

class IcqAccountCreationWizard : public AccountCreationWizard
{
	Q_OBJECT
public:
	IcqAccountCreationWizard();
	~IcqAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
	void finished();
private:
	QScopedPointer<IcqAccWizardPrivate> p;
};

} // namespace Icq

#endif // ACCOUNTCREATOR_H
