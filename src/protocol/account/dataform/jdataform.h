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
#include <gloox/jid.h>
#include <gloox/dataform.h>
#include <gloox/dataformfield.h>
#include <gloox/dataformfieldcontainer.h>

namespace Jabber
{
	using namespace gloox;

	struct JDataFormPrivate;

	class JDataForm : public QWidget
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JDataForm)
		public:
			JDataForm(const DataForm *form, bool twocolumn = false, QWidget *parent = 0);
			~JDataForm();
			DataForm *getDataForm();
		private:
			QScopedPointer<JDataFormPrivate> d_ptr;
	};
}

#endif // JDATAFORM_H
