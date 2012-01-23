/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include "qutim/protocol.h"
#include <QWizardPage>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccountMainSettings;
class IcqAccountCreationWizard;

class IcqAccWizardPage : public QWizardPage
{
	Q_SIGNAL
public:
	IcqAccWizardPage(QWidget *parent = 0);
	bool validatePage();
	bool isComplete() const;
private:
	IcqAccountMainSettings *m_settingsWidget;
};

class IcqAccountCreationWizard: public AccountCreationWizard
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	IcqAccountCreationWizard();
	~IcqAccountCreationWizard();
	QList<QWizardPage *> createPages(QWidget *parent);
private:
	IcqAccWizardPage *m_page;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ACCOUNTCREATOR_H

