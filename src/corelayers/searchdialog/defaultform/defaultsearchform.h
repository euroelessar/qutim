/****************************************************************************
 *  defaultsearchform.cpp
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
