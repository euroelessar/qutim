/****************************************************************************
 *  mobilesearchform.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
