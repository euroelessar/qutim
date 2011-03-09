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
typedef QPair<PersonInfo, int> PersonIntPair;
typedef QHash<QString, PersonIntPair> StringPersonHash;

bool personLessThen(const PersonIntPair &a, const PersonIntPair &b)
{
	return a.second > b.second || (a.second == b.second && a.first.name() < b.first.name());
}

SimpleAboutDialog::SimpleAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimpleAboutDialog)
{
    ui->setupUi(this);
	StringPersonHash authors;
	StringPersonHash::iterator it;
	foreach (Plugin *plugin, pluginsList()) {
		foreach (const PersonInfo &person, plugin->info().authors()) {
			it = authors.find(person.name());
			if (it == authors.end())
				it = authors.insert(person.name(), qMakePair(person, 0));
			it.value().second += plugin->avaiableExtensions().size();
		}
	}
	QVector<PersonIntPair> persons;
	persons.reserve(authors.size());
	it = authors.begin();
	for (; it != authors.end(); it++)
		persons.append(it.value());
	qSort(persons.begin(), persons.end(), personLessThen);
	QString html;
	for (int i = 0; i < persons.size(); i++) {
		html += QLatin1String("<p><div><b>");
		html += persons.at(i).first.name();
		html += QLatin1String("</b></div><div>");
		html += persons.at(i).first.task();
		html += QLatin1String("</div><div><a href=\"mailto:\"");
		html += persons.at(i).first.email();
		html += QLatin1String("\">");
		html += persons.at(i).first.email();
		html += QLatin1String("</a></div></p>");
	}
	ui->textedit_content->setHtml(html);
	ui->label_version->setText(qutimVersionStr());
	ui->label_qtuses ->setText(tr("Uses Qt %1.").arg(QLatin1String(qVersion())));
	QFile licenseFile(":/GPL.txt");
	QString license = tr("<div><b>qutIM</b> %1 is licensed under GNU General Public License, version 2.</div>"
								"<div>qutIM resources such as themes, icons, sounds may come along with a "
								"different license.</div><br><hr><br>").arg(qutimVersionStr());
	if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
		license += licenseFile.readAll().replace("<", "&lt;").replace(">", "&gt;");  // 'cause of: <signature of Ty Coon>, 1 April 1989
	else
		license += "<a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">GPLv2</a>";
	license.replace("\\n\\n", "<br><br>");
	ui->textedit_license->setHtml(license);
}

SimpleAboutDialog::~SimpleAboutDialog()
{
    delete ui;
}
}
