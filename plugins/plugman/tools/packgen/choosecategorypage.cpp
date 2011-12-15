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
#include "choosecategorypage.h"
#include "generatorwindow.h"
#include "ui_choosecategorypage.h"
#include <QFileDialog>
#include <QLibrary>

ChooseCategoryPage::ChooseCategoryPage(GeneratorWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ChooseCategoryPage)
{
	m_parent = parent;
    m_ui->setupUi(this);
	registerField("path", m_ui->filenameEdit);
}

ChooseCategoryPage::~ChooseCategoryPage()
{
    delete m_ui;
}

int ChooseCategoryPage::nextId () const
{
	return GeneratorWindow::Config;
}

bool ChooseCategoryPage::validatePage()
{
	qGen->setType(static_cast<GeneratorWindow::Type>(m_ui->comboBox->currentIndex()));
	return true;
}

void ChooseCategoryPage::on_comboBox_currentIndexChanged(int index)
{
	Q_UNUSED(index);
}

void ChooseCategoryPage::on_browseButton_clicked()
{
	switch(static_cast<GeneratorWindow::Type>(m_ui->comboBox->currentIndex()))
	{
	case GeneratorWindow::Art: {
		QString path = QFileDialog::getExistingDirectory(this, tr("Select art"), QDir::currentPath());
		m_ui->filenameEdit->setText(path);
		break; }
	case GeneratorWindow::Core: {
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
		QString filter = "*.exe";
#else
		QString filter;
#endif
		QString file = QFileDialog::getOpenFileName(this, tr("Select core"), QDir::currentPath(), filter);
		m_ui->filenameEdit->setText(file);
		break; }
	default: {
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
		QString filter = tr("Library (*.dll)");
#elif defined(Q_OS_DARWIN)
		QString filter = tr("Library (*.dylib *.bundle *.so)");
#elif defined(Q_OS_UNIX)
		QString filter = tr("Library (*.so)");
#endif
		QString file = QFileDialog::getOpenFileName(this, tr("Select library"), QDir::currentPath(), filter);
		m_ui->filenameEdit->setText(file);
		break; }
	}
}

void ChooseCategoryPage::changeEvent(QEvent *e)
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

