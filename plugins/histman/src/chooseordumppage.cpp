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

#include "chooseordumppage.h"
#include "ui_chooseordumppage.h"

namespace HistoryManager {

ChooseOrDumpPage::ChooseOrDumpPage(HistoryManagerWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ChooseOrDumpPage)
{
    m_ui->setupUi(this);
	m_parent = parent;
	setTitle(tr("What to do next?", "Dump history or choose next client"));
	setSubTitle(tr("It is possible to choose another client for import history or dump history to the disk."));
}

ChooseOrDumpPage::~ChooseOrDumpPage()
{
    delete m_ui;
}

void ChooseOrDumpPage::initializePage()
{
	m_ui->dumpRadioButton->setChecked(true);
}

void ChooseOrDumpPage::cleanupPage()
{
	m_ui->dumpRadioButton->setChecked(true);
}

bool ChooseOrDumpPage::validatePage()
{
	if(nextId() == HistoryManagerWindow::ChooseClient)
	{
		m_parent->restart();
		return false;
	}
	return true;
}

int ChooseOrDumpPage::nextId() const
{
	return m_ui->dumpRadioButton->isChecked() ? HistoryManagerWindow::ExportHistory : HistoryManagerWindow::ChooseClient;
}

void ChooseOrDumpPage::changeEvent(QEvent *e)
{
    QWizardPage::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

}

