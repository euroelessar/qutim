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
#include "jcertinfo.h"

namespace Jabber
{

struct JCertInfoPrivate
{
	Ui::JCertInfo ui;
};

//JCertInfo::JCertInfo(const CertInfo &info, QWidget *parent) : QDialog(parent), p(new JCertInfoPrivate)
//{
//	p->ui.setupUi(this);

//	QDateTime from, to;
//	from.setTime_t(info.date_from);
//	to.setTime_t(info.date_to);

//	p->ui.cipherLabel->setText(QString::fromStdString(info.cipher));
//	p->ui.compressionLabel->setText(QString::fromStdString(info.compression));
//	p->ui.serverLabel->setText(QString::fromStdString(info.server));
//	p->ui.fromLabel->setText(from.toString("hh:mm:ss dd/MM/yyyy"));
//	p->ui.toLabel->setText(to.toString("hh:mm:ss dd/MM/yyyy"));
//}

JCertInfo::~JCertInfo()
{
}

bool JCertInfo::exec(bool &result)
{
	result = QDialog::exec();
	return p->ui.rememberBox->isChecked();
}
}

