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

