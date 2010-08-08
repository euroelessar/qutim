/****************************************************************************
 *  accountcreator.cpp
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

#include "accountcreator.h"
#include "icqaccountmainsettings.h"
#include <QVBoxLayout>
#include "icqprotocol.h"

namespace qutim_sdk_0_3 {

namespace oscar {

IcqAccWizardPage::IcqAccWizardPage(QWidget *parent) :
	QWizardPage(parent),
	m_settingsWidget(new IcqAccountMainSettings(0, this))
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_settingsWidget);
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
