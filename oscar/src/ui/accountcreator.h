/****************************************************************************
 *  accountcreator.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

namespace qutim_sdk_0_3 {

namespace oscar {

struct IcqAccWizardPrivate;

class IcqAccountCreationWizard: public AccountCreationWizard
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	IcqAccountCreationWizard();
	~IcqAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
	void finished();
private:
	QScopedPointer<IcqAccWizardPrivate> p;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ACCOUNTCREATOR_H
