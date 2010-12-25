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

#include <QWidget>
#include <QGridLayout>

namespace jreen
{
class DataForm;
}

namespace Jabber
{

class JDataFormPrivate;

class JDataForm : public QWidget
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JDataForm)
public:
	JDataForm(const QSharedPointer<jreen::DataForm> &form, QWidget *parent = 0);
	~JDataForm();
	jreen::DataForm *getDataForm();
private:
	QScopedPointer<JDataFormPrivate> d_ptr;
};
}

#endif // JDATAFORM_H
