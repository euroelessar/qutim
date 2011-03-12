/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "simpleaboutdialog.h"
#include "ui_simpleaboutdialog.h"
#include <qutim/plugin.h>
#include <qutim/debug.h>
#include <QHash>
#include <QFile>
#include <QRegExp>

using namespace qutim_sdk_0_3;

namespace Core
{
SimpleAboutDialog::SimpleAboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SimpleAboutDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	ui->texteditDevelopers->setHtml(toHtml(PersonInfo::authors(), true));
	QList<PersonInfo> translators = PersonInfo::translators();
	if (translators.isEmpty())
		ui->tabWidget->removeTab(1);
	else
		ui->texteditTranslators->setHtml(toHtml(translators, false));
	ui->labelVersion->setText(QLatin1String(qutimVersionStr()));
	ui->labelQtVer ->setText(tr("Based on Qt %1 (%2 bit).")
	                         .arg(QLatin1String(QT_VERSION_STR), QString::number(QSysInfo::WordSize)));
	QFile licenseFile(":/GPL");
	QString license = tr("<div><b>qutIM</b> %1 is licensed under GNU General Public License, version 2"
								" or (at your option) any later version.</div>"
								"<div>qutIM resources such as themes, icons, sounds may come along with a "
								"different license.</div><br><hr><br>").arg(qutimVersionStr());
	if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		license += Qt::escape(QLatin1String(licenseFile.readAll()));
	} else {
		license += "<a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GPLv2</a>";
	}
	license.replace(QLatin1String("\n\n"), "<br><br>");
	ui->texteditLicense->setHtml(license);
}

QString SimpleAboutDialog::toHtml(const QList<PersonInfo> &persons, bool useTask)
{
	QString html;
	for (int i = 0; i < persons.size(); i++) {
		const PersonInfo &info = persons.at(i);
		html += QLatin1String("<p><div><b>");
		html += Qt::escape(info.name());
		html += QLatin1String("</b>");
		if (useTask) {
			html += QLatin1String("</div><div>");
			html += Qt::escape(info.task());
		}
		html += QLatin1String("</div>");
		if (!info.email().isEmpty()) {
			html += QLatin1String("<div><a href=\"mailto:\"");
			html += Qt::escape(info.email());
			html += QLatin1String("\">");
			html += Qt::escape(info.email());
			html += QLatin1String("</a></div>");
		}
		html += QLatin1String("</p>");
	}
	return html;
}

SimpleAboutDialog::~SimpleAboutDialog()
{
	delete ui;
}
}
