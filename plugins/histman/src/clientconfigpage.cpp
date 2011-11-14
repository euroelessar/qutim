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

#include "clientconfigpage.h"
#include "ui_clientconfigpage.h"
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <QFileDialog>
#include <QTextCodec>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

ClientConfigPage::ClientConfigPage(HistoryManagerWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ClientConfigPage)
{
    m_ui->setupUi(this);
	m_parent = parent;
	registerField("historypath", m_ui->filenameEdit);
	QList<int> mibs = QTextCodec::availableMibs();
	foreach (int mib, mibs) {
		QTextCodec *codec = QTextCodec::codecForMib(mib);
		QString name = codec->name();
		foreach (const QString &alias, codec->aliases())
			name += '/' + alias;
		m_ui->codepageBox->addItem(name, codec->name());
	}
	QTextCodec *locale_codec = QTextCodec::codecForLocale();
	m_ui->codepageBox->setCurrentIndex(m_ui->codepageBox->findData(locale_codec ? locale_codec->name() : "UTF-8"));
	connect(m_ui->filenameEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
	setTitle(tr("Configuration"));
	setCommitPage(true);
	setButtonText(QWizard::CommitButton, m_parent->nextStr());
}

ClientConfigPage::~ClientConfigPage()
{
    delete m_ui;
}

void ClientConfigPage::initializePage()
{
	m_valid = false;
	if(m_valid_pixmap.isNull())
	{
		m_valid_pixmap = Icon("dialog-ok-apply").pixmap(16);
		m_invalid_pixmap = Icon("dialog-cancel").pixmap(16);
	}
	m_ui->validIcon->setPixmap(m_invalid_pixmap);
	QString subtitle;
	if(m_parent->getCurrentClient()->chooseFile())
		subtitle = tr("Enter path of your %1 profile file.");
	else
		subtitle = tr("Enter path of your %1 profile dir.");
	subtitle.replace("%1", m_parent->getCurrentClient()->name());
	if(m_parent->getCurrentClient()->needCharset())
	{
		subtitle += " ";
		subtitle += tr("If your history encoding differs from the system one, choose the appropriate encoding for history.");
		m_ui->label_2->show();
		m_ui->codepageBox->show();
	}
	else
	{
		m_ui->label_2->hide();
		m_ui->codepageBox->hide();
	}
	QString client_specific = m_parent->getCurrentClient()->additionalInfo();
	if(!client_specific.isEmpty())
	{
		subtitle += " ";
		subtitle += client_specific;
	}
	setSubTitle(subtitle);
	ConfigGroup config = Config().group("histman").group(m_parent->getCurrentClient()->name());
	m_ui->filenameEdit->setText(config.value("importpath", QString()));
	m_ui->codepageBox->setCurrentIndex(m_ui->codepageBox->findData(config.value("codepage", QVariant("System"))));
	onTextChanged(m_ui->filenameEdit->text());
	m_config_list = m_parent->getCurrentClient()->config();
	for(int i = 0; i < m_config_list.size(); i++)
	{
		m_ui->formLayout->setWidget(i + 2, QFormLayout::LabelRole, m_config_list[i].first);
		m_ui->formLayout->setWidget(i + 2, QFormLayout::FieldRole, m_config_list[i].second);
	}
}

void ClientConfigPage::cleanupPage()
{
	m_valid = false;
	foreach(ConfigWidget config, m_config_list)
	{
		delete config.first;
		delete config.second;
	}
	m_config_list.clear();
}

bool ClientConfigPage::validatePage()
{
	ConfigGroup config = Config().group("histman").group(m_parent->getCurrentClient()->name());
	config.setValue("importpath", m_ui->filenameEdit->text());
	QByteArray charset = m_ui->codepageBox->itemData(m_ui->codepageBox->currentIndex()).toByteArray();
	config.setValue("codepage", QString::fromLatin1(charset));
	m_parent->setCharset(charset);
	m_parent->getCurrentClient()->useConfig();
	return true;
}

bool ClientConfigPage::isComplete() const
{
	return m_valid;
}

int ClientConfigPage::nextId() const
{
	return HistoryManagerWindow::ImportHistory;
}

QString ClientConfigPage::getAppropriateFilePath(const QString &filename_)
{
	if(filename_.startsWith("~/"))
	{
		QString filename =  QDir::homePath();
		filename += QDir::separator();
		filename += filename_.mid(2);
		return filename;
	}
	else
		return filename_;
}

QString ClientConfigPage::getAppropriatePath(const QString &path_)
{
	QString path = getAppropriateFilePath(path_);
	path.replace("\\", "/");
	while(!path.isEmpty() && !QFileInfo(path).isDir())
	{
		int size = path.lastIndexOf("/");
		path.truncate(size);
	}
	return path.isEmpty() ? QDir::homePath() : path;
}

void ClientConfigPage::on_browseButton_clicked()
{
	QString path;
	if(m_parent->getCurrentClient()->chooseFile())
		path = QFileDialog::getOpenFileName(this, tr("Select path"), getAppropriatePath(m_ui->filenameEdit->text()));
	else
		path = QFileDialog::getExistingDirectory(this, tr("Select path"), getAppropriatePath(m_ui->filenameEdit->text()));
	if (!path.isEmpty())
		m_ui->filenameEdit->setText(path);
}

void ClientConfigPage::onTextChanged(const QString &filename)
{
	m_valid = m_parent->getCurrentClient()->validate(getAppropriateFilePath(filename));
	m_ui->validIcon->setPixmap(m_valid ? m_valid_pixmap : m_invalid_pixmap);
	emit completeChanged();
}

void ClientConfigPage::changeEvent(QEvent *e)
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

