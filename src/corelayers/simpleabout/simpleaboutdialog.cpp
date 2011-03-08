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
#include <qutim/servicemanager.h>

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
	html = tr("qutIM %1<br>Uses Qt %2<p>Developers:<p>").arg(QLatin1String(qutimVersionStr()),
	                                        QLatin1String(qVersion()));
	for (int i = 0; i < persons.size(); i++) {
		html += persons.at(i).first.name();
		html += QLatin1String("<br>");
		html += persons.at(i).first.task();
		html += QLatin1String("<br><a href=\"mailto:\"");
		html += persons.at(i).first.email();
		html += QLatin1String("\">");
		html += persons.at(i).first.email();
		html += QLatin1String("</a><p>");
	}
	ui->label->setTextFormat(Qt::RichText);
	ui->label->setText(html);

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, ui->scrollArea->viewport()));
}

SimpleAboutDialog::~SimpleAboutDialog()
{
    delete ui;
}
}
