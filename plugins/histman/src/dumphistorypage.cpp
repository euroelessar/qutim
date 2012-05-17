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

#include "dumphistorypage.h"
#include "ui_dumphistorypage.h"
#include <qutim/systeminfo.h>

#include <QTimer>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

DumpHistoryPageHelper::DumpHistoryPageHelper(DumpHistoryPage *parent) : QThread(parent)
{
	m_parent = parent;
}

void DumpHistoryPageHelper::run()
{
	if(m_parent->m_state == DumpHistoryPage::LoadingHistory)
	{
		m_parent->m_parent->getQutIM()->loadMessages(SystemInfo::getPath(SystemInfo::HistoryDir));
	}
	else if(m_parent->m_state == DumpHistoryPage::SavingHistory)
	{
		m_parent->m_parent->saveMessages(m_parent->m_format);
	}
}

DumpHistoryPage::DumpHistoryPage(HistoryManagerWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::DumpHistoryPage)
{
    m_ui->setupUi(this);
	m_parent = parent;
	setFinalPage(true);
	m_state = PreInit;
	connect(m_parent, SIGNAL(maxValueChanged(int)), m_ui->mergeProgressBar, SLOT(setMaximum(int)));
	connect(m_parent, SIGNAL(valueChanged(int)), m_ui->mergeProgressBar, SLOT(setValue(int)));
	connect(m_parent, SIGNAL(saveMaxValueChanged(int)), m_ui->dumpProgressBar, SLOT(setMaximum(int)));
	connect(m_parent, SIGNAL(saveValueChanged(int)), m_ui->dumpProgressBar, SLOT(setValue(int)));
	m_format = 0;
	m_helper = new DumpHistoryPageHelper(this);
	connect(m_helper, SIGNAL(finished()), this, SLOT(completed()));
	setTitle(tr("Dumping"));
//	GeneratorList gens = moduleGenerators<HistoryExporter>();
//	Q_REGISTER_EVENT(event_exports, EventExporters);
//	qutim_sdk_0_2::Event(event_exports, 1, &m_clients_list).send();
	m_ui->label_3->hide();
	m_ui->binaryRadioButton->hide();
	m_ui->jsonRadioButton->hide();
}

DumpHistoryPage::~DumpHistoryPage()
{
    delete m_ui;
}

void DumpHistoryPage::initializePage()
{
	m_state = PreInit;
//	QFileInfoList files;
//	int num = 0;
	m_ui->mergeProgressBar->setValue(0);
	m_ui->dumpProgressBar->setValue(0);
	m_ui->binaryRadioButton->setEnabled(true);
	m_ui->jsonRadioButton->setEnabled(true);
//	if(m_parent->getQutIM()->guessJson(profile_path, files, num))
//	{
		m_ui->jsonRadioButton->setChecked(true);
		m_ui->binaryRadioButton->setChecked(false);
//	}
//	else
//	{
//		m_ui->binaryRadioButton->setChecked(true);
//		m_ui->jsonRadioButton->setChecked(false);
//	}
	setButtonText(QWizard::FinishButton, m_parent->dumpStr());
	setSubTitle(tr("Last step. Click 'Dump' to start dumping process."));//tr("Choose appropriate format of history, binary is default qutIM format nowadays."));
}

void DumpHistoryPage::cleanupPage()
{
}

bool DumpHistoryPage::isComplete() const
{
	return m_state == Finished || m_state == PreInit;
}

bool DumpHistoryPage::validatePage()
{
	if(m_state == Finished)
		return true;
	setSubTitle(tr("Manager merges history, it make take several minutes."));
	setButtonText(QWizard::FinishButton, m_parent->finishStr());
	m_ui->binaryRadioButton->setEnabled(false);
	m_ui->jsonRadioButton->setEnabled(false);
	m_state = LoadingHistory;
	m_format = m_ui->jsonRadioButton->isChecked() ? 'j' : 'b';
	emit completeChanged();
	m_parent->button(QWizard::BackButton)->setEnabled(false);
	m_parent->button(QWizard::CancelButton)->setEnabled(false);
	QTimer::singleShot(100, m_helper, SLOT(start()));
	return false;
}

int DumpHistoryPage::nextId() const
{
	return -1;
}

void DumpHistoryPage::completed()
{
	if(m_state == LoadingHistory)
	{
		m_state = SavingHistory;
		QTimer::singleShot(100, m_helper, SLOT(start()));
	}
	else if(m_state == SavingHistory)
	{
		setSubTitle(tr("History has been successfully imported."));
		m_state = Finished;
		m_parent->button(QWizard::BackButton)->setEnabled(true);
		m_parent->button(QWizard::CancelButton)->setEnabled(true);
		emit completeChanged();
	}
}

void DumpHistoryPage::changeEvent(QEvent *e)
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

