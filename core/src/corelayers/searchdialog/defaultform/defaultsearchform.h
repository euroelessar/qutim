/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef DEFAULTSEARCHFORM_H
#define DEFAULTSEARCHFORM_H

#include "../abstractsearchform.h"
#include "ui_defaultsearchform.h"

namespace Core {

	class DefaultSearchForm : public AbstractSearchForm
	{
		Q_OBJECT
	public:
		DefaultSearchForm(const QList<AbstractSearchFactory*> &factories,
						  const QString &title = QString(),
						  const QIcon &icon = QIcon(),
						  QWidget *parent = 0);

	private slots:
		void startSearch();
		void cancelSearch();
		void updateRequest(int row);
		void updateService();
		void done(bool ok);
		void updateFields();
		void updateServiceBox();
		void updateActionButtons();	
		void actionButtonClicked();
	private:
		void setState(bool search);
		Ui::DefaultSearchForm ui;
		QList<QPushButton*> m_actionButtons;
	};

	class DefaultSearchFormFactory : public AbstractSearchFormFactory
	{
		Q_OBJECT
	public:
		AbstractSearchForm *createForm(const QList<AbstractSearchFactory*> &factories,
									   const QString &title = QString(),
									   const QIcon &icon = QIcon(),
									   QWidget *parent = 0);

	};
}

#endif // DEFAULTSEARCHFORM_H

