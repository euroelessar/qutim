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
#include "configpackagepage.h"
#include "generatorwindow.h"
#include "pluglibinfo.h"
#include "ui_configpackagepage.h"
#include <QDebug>
#include <QZipReader/qzipwriter.h>
#include <QDir>
#include <QTimer>

ConfigPackagePage::ConfigPackagePage(GeneratorWindow *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ConfigPackagePage)
{
	m_ui->setupUi(this);
	registerField("name*", m_ui->nameLineEdit);
	registerField("type*", m_ui->typeLineEdit);
	registerField("version*", m_ui->versionLineEdit);
	registerField("category", m_ui->categoryComboBox);
	registerField("shortdesc", m_ui->shortDescLineEdit);
	registerField("url*", m_ui->urlLineEdit);
	registerField("author", m_ui->authorLineEdit);
	registerField("license", m_ui->licenseLineEdit);
	registerField("platform", m_ui->platformLineEdit);
	registerField("package", m_ui->packageNameLineEdit);
	setCommitPage(true);
	m_parent = parent;
}

ConfigPackagePage::~ConfigPackagePage()
{
    delete m_ui;
}

int ConfigPackagePage::nextId() const
{
	return GeneratorWindow::Save;
}

void ConfigPackagePage::initializePage()
{
	packageInfo info;
	if(qGen->type() == GeneratorWindow::Plugin)
	{
		plugLibInfo libInfo(field("path").toString());
		if(libInfo.isValid())
		{
			info = libInfo.toInfo();
		}
	}
	else if(qGen->type() == GeneratorWindow::Art)
	{
		QFileInfo file(field("path").toString());
		info.properties["name"] = file.fileName();
		QDir dir = file.dir();
		info.properties["type"] = dir.dirName();
		info.properties["platform"] = "all";
	}
	setField("name", info.properties["name"]);
	setField("type", info.properties["type"]);
	setField("version", info.properties["version"]);
	setField("category", qGen->type());
	setField("shortdesc", info.properties["description"]);
	setField("url", info.properties["url"]);
	setField("author", info.properties["author"]);
	setField("platform", info.properties["platform"]);
	setField("license", info.properties["license"]);
}

void addFilesToArchive(const QDir &dir, const QFileInfo &info, QStringList &files)
{
	qDebug() << info.absoluteFilePath() << info.isDir() << info.isFile();
	if(info.isFile())
		files << dir.relativeFilePath(info.absoluteFilePath());
	else if(info.isDir())
	{
		qDebug() << info.absoluteFilePath() << QDir(info.absoluteFilePath()).entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
		QFileInfoList file_infos = QDir(info.absoluteFilePath()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		foreach(const QFileInfo &file_info, file_infos)
		{
			addFilesToArchive(dir, file_info, files);
		}
	}
}

bool ConfigPackagePage::validatePage()
{
	{
		packageInfo info;
		info.properties["name"] = field("name").toString();
		info.properties["type"] = field("type").toString();
		info.properties["version"] = field("version").toString();
		if(qGen->type() == GeneratorWindow::Art)
			info.properties["category"] = "art";
		else if(qGen->type() == GeneratorWindow::Core)
			info.properties["category"] = "core";
		else if(qGen->type() == GeneratorWindow::Lib)
			info.properties["category"] = "lib";
		else if(qGen->type() == GeneratorWindow::Plugin)
			info.properties["category"] = "plugin";
		info.properties["description"] = field("shortdesc").toString();
		info.properties["url"] = field("url").toString();
		info.properties["author"] = field("author").toString();
		info.properties["platform"] = field("platform").toString();
		info.properties["license"] = field("license").toString();
		QDir dir = QFileInfo(field("path").toString()).dir();
		dir.cdUp();
		qDebug() << dir.absolutePath();
		addFilesToArchive(dir, QFileInfo(field("path").toString()), info.files);
		qDebug() << info.files;
		QString file_name = field("package").toString().toLower();
		file_name += "-";
		file_name += field("version").toString();
		file_name += ".zip";
		QZipWriter writer(file_name);
		{
			QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
			data.append(info.toString().toUtf8());
			writer.addFile("Pinfo.xml", data);
		}
		foreach(const QString filepath, info.files)
		{
			QFile file(dir.filePath(filepath));
			if(file.open(QIODevice::ReadOnly))
				writer.addFile(filepath, &file);
		}
		writer.close();
	}
	QTimer::singleShot(0, qGen, SLOT(restart()));
	return false;
}

void ConfigPackagePage::on_nameLineEdit_textChanged(const QString &text)
{
}

void ConfigPackagePage::changeEvent(QEvent *e)
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

