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

#ifndef JDATAFORM_H
#define JDATAFORM_H

#include <QDialog>
#include <QGridLayout>
#include <QVariant>
#include <jreen/dataform.h>
#include <jreen/bitsofbinary.h>
#include <qutim/dataforms.h>

namespace Jabber
{

class JDataFormPrivate;

class JDataForm : public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JDataForm)
public:
	JDataForm(const Jreen::DataForm::Ptr &form, QWidget *parent = 0);
	JDataForm(const Jreen::DataForm::Ptr &form, const QList<Jreen::BitsOfBinary::Ptr> &bobs, QWidget *parent = 0);
	~JDataForm();

	qutim_sdk_0_3::AbstractDataForm *widget();
	Jreen::DataForm::Ptr getDataForm();
	
	static qutim_sdk_0_3::DataItem convertToDataItem(const Jreen::DataForm::Ptr &form, const QList<Jreen::BitsOfBinary::Ptr> &bobs = QList<Jreen::BitsOfBinary::Ptr>());
	static void convertFromDataItem(const Jreen::DataForm::Ptr &form, const qutim_sdk_0_3::DataItem &item);
	
private:
	QScopedPointer<JDataFormPrivate> d_ptr;
};
}

#endif // JDATAFORM_H

