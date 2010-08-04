/****************************************************************************
 *  searchdialoglayer.h
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

#ifndef SEARCHDIALOGLAYER_H
#define SEARCHDIALOGLAYER_H

#include "abstractsearchform.h"

namespace Core
{

	class SearchLayer : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "SearchLayer")
		Q_CLASSINFO("Uses", "IconLoader")
		Q_CLASSINFO("Uses", "ContactList")
	public:
		SearchLayer();
		~SearchLayer();
		bool event(QEvent *ev);
	public slots:
		void showContactSearch(QObject*);
		QWidget *createSearchDialog(const QList<AbstractSearchFactory*> &factories,
									const QString &title = QString(),
									const QIcon &icon = QIcon(),
									QWidget *parent = 0);
	private:
		QPointer<AbstractSearchForm> m_contactSearchDialog;
		QList<AbstractSearchFactory*> m_contactSearchFactories;
	};

}

#endif // SEARCHDIALOGLAYER_H
