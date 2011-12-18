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

#include "mobileaboutdialog.h"
#include "ui_mobileaboutdialog.h"
#include <qutim/plugin.h>
#include <qutim/debug.h>
#include <QHash>
#include <qutim/servicemanager.h>

using namespace qutim_sdk_0_3;

namespace Core
{
MobileAboutDialog::MobileAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MobileAboutDialog)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	QList<PersonInfo> persons = PersonInfo::authors();
	QString html;
	html = tr("qutIM %1<br>Uses Qt %2<p>Developers:<p>").arg(QLatin1String(versionString()),
	                                        QLatin1String(qVersion()));
	for (int i = 0; i < persons.size(); i++) {
		html += persons.at(i).name();
		html += QLatin1String("<br>");
		html += persons.at(i).task();
		html += QLatin1String("<br><a href=\"mailto:\"");
		html += persons.at(i).email();
		html += QLatin1String("\">");
		html += persons.at(i).email();
		html += QLatin1String("</a><p>");
	}
	ui->label->setTextFormat(Qt::RichText);
	ui->label->setText(html);

	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller, "enableScrolling", Q_ARG(QObject*, ui->scrollArea->viewport()));
}

MobileAboutDialog::~MobileAboutDialog()
{
    delete ui;
}
}

