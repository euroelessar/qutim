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

#include "accountcreator.h"
#include "icqaccountmainsettings.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include "../icqprotocol.h"

namespace qutim_sdk_0_3 {

namespace oscar {

IcqAccWizardPage::IcqAccWizardPage(QWidget *parent) :
	QWizardPage(parent),
	m_settingsWidget(new IcqAccountMainSettings(0, this))
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QScrollArea *scrollArea = new QScrollArea(this);
	layout->addWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(m_settingsWidget);
	connect(m_settingsWidget, SIGNAL(completeChanged()),
			SIGNAL(completeChanged()));
}

bool IcqAccWizardPage::validatePage()
{
	if (!m_settingsWidget->isComplete())
		return false;
	m_settingsWidget->saveSettings();
	return true;
}

bool IcqAccWizardPage::isComplete() const
{
	return m_settingsWidget->isComplete();
}

IcqAccountCreationWizard::IcqAccountCreationWizard() :
	AccountCreationWizard(IcqProtocol::instance())
{
}

IcqAccountCreationWizard::~IcqAccountCreationWizard()
{
}

QList<QWizardPage *> IcqAccountCreationWizard::createPages(QWidget *parent)
{
	m_page = new IcqAccWizardPage(parent);
	QList<QWizardPage *> pages;
	pages << m_page;
	return pages;
}

} } // namespace qutim_sdk_0_3::oscar

