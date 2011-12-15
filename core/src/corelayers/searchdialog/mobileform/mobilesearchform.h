/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef MOBILESEARCHFORM_H
#define MOBILESEARCHFORM_H

#include "../abstractsearchform.h"
#include "ui_mobilesearchform.h"

namespace Core {

class MobileSearchForm : public AbstractSearchForm
{
	Q_OBJECT
public:
	enum SearchState
	{
		SearchingState,
		ReadyState,
		ErrorState,
		DoneState
	};
	MobileSearchForm(const QList<AbstractSearchFactory*> &factories,
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
	void onNegativeActionTriggered();
private:
	void setState(SearchState search);
	Ui::MobileSearchForm ui;
	QAction *m_action;
	QList<QAction *> m_actions;
	SearchState m_search_state;
};

class MobileSearchFormFactory : public AbstractSearchFormFactory
{
	Q_OBJECT
public:
	AbstractSearchForm *createForm(const QList<AbstractSearchFactory*> &factories,
								   const QString &title = QString(),
								   const QIcon &icon = QIcon(),
								   QWidget *parent = 0);

};
}

#endif // MOBILESEARCHFORM_H

