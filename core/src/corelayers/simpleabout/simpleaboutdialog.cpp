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
	ui->labelVersion->setText(QLatin1String(versionString()));
	ui->labelQtVer ->setText(tr("Based on Qt %1 (%2 bit).")
	                         .arg(QLatin1String(qVersion()), QString::number(QSysInfo::WordSize)));
	QFile licenseFile(":/GPL");
	QString license = tr("<div><b>qutIM</b> %1 is licensed under GNU General Public License, version 3"
								" or (at your option) any later version.</div>"
								"<div>qutIM resources such as themes, icons, sounds may come along with a "
								"different license.</div><br><hr><br>");
	if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		license += QString::fromUtf8(licenseFile.readAll()).toHtmlEscaped();
	} else {
		license += QLatin1String("<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GPLv3</a>");
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
		html += info.name().toString().toHtmlEscaped();
		html += QLatin1String("</b>");
		if (useTask) {
			html += QLatin1String("</div><div>");
			html += info.task().toString().toHtmlEscaped();
		}
		html += QLatin1String("</div>");
		if (!info.email().isEmpty()) {
			html += QLatin1String("<div><a href=\"mailto:\"");
			html += info.email().toString().toHtmlEscaped();
			html += QLatin1String("\">");
			html += info.email().toString().toHtmlEscaped();
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

