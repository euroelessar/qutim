/****************************************************************************
 *  jdataform.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                     by Denis Daschenko <daschenko@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef JDATAFORM_H
#define JDATAFORM_H

#include <QDialog>
#include <QGridLayout>
#include <QVariant>
#include <jreen/dataform.h>
#include <qutim/dataforms.h>

namespace Jabber
{

class JDataFormPrivate;

class JDataForm : public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JDataForm)
public:
	JDataForm(const jreen::DataForm::Ptr &form,
			  qutim_sdk_0_3::AbstractDataForm::StandardButtons buttons = qutim_sdk_0_3::AbstractDataForm::NoButton,
			  QWidget *parent = 0);
	~JDataForm();
	qutim_sdk_0_3::AbstractDataForm *widget();
	jreen::DataForm::Ptr getDataForm();
protected slots:
	void onItemChanged(const QString &name, const QVariant &data);
private:
	QScopedPointer<JDataFormPrivate> d_ptr;
};
}

#endif // JDATAFORM_H
