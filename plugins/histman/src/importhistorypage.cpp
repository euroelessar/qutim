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

#include "importhistorypage.h"
#include "clientconfigpage.h"
#include "ui_importhistorypage.h"
#include <QTimer>
#include <QAbstractButton>

namespace HistoryManager {

ImportHistoryPageHepler::ImportHistoryPageHepler(ImportHistoryPage *parent) : QThread(parent)
{
	m_parent = parent;
	m_time = 0;
}

void ImportHistoryPageHepler::run()
{
	QTime t;
	t.start();
	m_parent->m_parent->getCurrentClient()->loadMessages(m_path);
	m_time = t.elapsed();
}

ImportHistoryPage::ImportHistoryPage(HistoryManagerWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ImportHistoryPage)
{
	m_parent = parent;
    m_ui->setupUi(this);
	setTitle(tr("Loading"));
	connect(m_parent, SIGNAL(maxValueChanged(int)), m_ui->progressBar, SLOT(setMaximum(int)));
	connect(m_parent, SIGNAL(valueChanged(int)), m_ui->progressBar, SLOT(setValue(int)));
	m_helper = new ImportHistoryPageHepler(this);
	connect(m_helper, SIGNAL(finished()), this, SLOT(completed()));
	setCommitPage(true);
	setButtonText(QWizard::CommitButton, m_parent->nextStr());
}

ImportHistoryPage::~ImportHistoryPage()
{
    delete m_ui;
}

void ImportHistoryPage::initializePage()
{
	m_completed = false;
	setSubTitle(tr("Manager loads all history to memory, it may take several minutes."));
	m_parent->getCurrentClient()->setCharset(m_parent->charset());
	m_helper->setPath(ClientConfigPage::getAppropriateFilePath(field("historypath").toString()));
	m_ui->progressBar->setValue(0);
	QTimer::singleShot(100, m_helper, SLOT(start()));
	m_parent->button(QWizard::BackButton)->setEnabled(false);
	m_parent->button(QWizard::CancelButton)->setEnabled(false);
}

void ImportHistoryPage::cleanupPage()
{
	m_completed = false;
}

bool ImportHistoryPage::isComplete() const
{
	return m_completed;
}

int ImportHistoryPage::nextId() const
{
	return HistoryManagerWindow::ChooseOrDump;
}

void ImportHistoryPage::completed()
{
	setSubTitle(tr("%n message(s) have been successfully loaded to memory.", 0, m_parent->getMessageNum())
				+ " " + tr("It has taken %n ms.", 0, m_helper->getTime()));
	m_completed = true;
	m_ui->progressBar->setValue(m_ui->progressBar->maximum());
	m_parent->button(QWizard::BackButton)->setEnabled(true);
	m_parent->button(QWizard::CancelButton)->setEnabled(true);
	emit completeChanged();
}

void ImportHistoryPage::changeEvent(QEvent *e)
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

