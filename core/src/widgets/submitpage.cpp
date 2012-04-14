/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "submitpage.h"
#include <qutim/libqutim_version.h>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QTextBrowser>
#include <qutim/systeminfo.h>
#include <QNetworkRequest>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <qutim/debug.h>
#include "profilecreationwizard.h"

namespace Core {

using namespace qutim_sdk_0_3;

SubmitPage::SubmitPage(qutim_sdk_0_3::StatisticsHelper *helper, QWizard *parent)
    : QWizardPage(parent)
{
	m_helper = helper;
	if (qobject_cast<ProfileCreationWizard*>(parent)) {
		setTitle(tr("Congratulations"));
		setSubTitle(tr("You've just created a profile for qutIM. Click Finish to proceed to adding user accounts"));
	} else {
		setTitle(tr("Statistics gatherer"));
		setSubTitle(tr("Your system settings differ from the ones sent last time"));
	}
	if (helper->action() == StatisticsHelper::NeedToAskInit
			|| helper->action() == StatisticsHelper::NeedToAskUpdate) {
		QVBoxLayout *scrollArealayout = new QVBoxLayout(this);
		scrollArealayout->setMargin(0);
		QScrollArea *scrollArea = new QScrollArea(this);
		scrollArealayout->addWidget(scrollArea);
		scrollArea->setFrameShape(QFrame::NoFrame);
		QWidget *scrollWidget = new QWidget(this);
		scrollArea->setWidget(scrollWidget);
		scrollArea->setWidgetResizable(true);
		QVBoxLayout *l = new QVBoxLayout(scrollWidget);
		m_submitBox = new QCheckBox(tr("Would you like to send details about your current setup?"), this);
		m_submitBox->setChecked(true);
		m_dontAskLater = new QCheckBox(tr("Dont's ask me later"), this);
		m_dontAskLater->setChecked(false);
		QLabel *label = new QLabel(tr("Information to be transferred to the qutIM's authors:"), this);
		m_information = new QTextBrowser(this);
		m_information->setHtml(m_helper->infoHtml());
		m_information->setMinimumHeight(100);

#ifdef Q_WS_MAEMO_5
		m_submitBox->setMaximumHeight(40);
		m_dontAskLater->setMaximumHeight(40);
#endif

		l->addWidget(m_submitBox);
		l->addWidget(label);
		l->addWidget(m_information);
		l->addWidget(m_dontAskLater);
	} else {
		m_submitBox = 0;
		m_dontAskLater = 0;
		m_information = 0;
	}
}

bool SubmitPage::validatePage()
{
	if (m_submitBox && m_dontAskLater) {
		m_helper->setDecisition(!m_submitBox->isChecked(), m_dontAskLater->isChecked());
	}
	return QWizardPage::validatePage();
}

} // namespace Core

