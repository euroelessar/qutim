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

#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include "resultmodel.h"
#include <QListView>

class QComboBox;
class QPushButton;

namespace Core {

class AbstractSearchForm : public QWidget
{
	Q_OBJECT
public:
	explicit AbstractSearchForm(const QList<AbstractSearchFactory*> &factories,
								const QString &title = QString(),
								const QIcon &icon = QIcon(),
								QWidget *parent = 0);
	~AbstractSearchForm();
protected:
	void setTitle(const QString &title, const QIcon &icon = QIcon());
	bool startSearch();
	bool cancelSearch();
	void setCurrentRequest(RequestPtr request);
	void setService(const QString &service);
	void updateServiceBox(QComboBox *serviceBox, QPushButton *updateServiceButton);
	void updateSearchFieldsWidget();
	QAction *actionAt(int index);
	RequestPtr currentRequest() { return m_currentRequest; }
	RequestsListModel *requestsModel() { return m_requestsModel; }
	ResultModel *resultModel() { return m_resultModel; }
	QWidget *searchFieldsWidget() { return m_searchFieldsWidget.data(); }
	void clearActionButtons();
	void actionButtonClicked(QAction *button, const QList<QModelIndex> &selected);
private slots:
	void done(bool ok);
protected:
	bool event(QEvent *e);
private:
	RequestPtr m_currentRequest;
	QWeakPointer<AbstractDataForm> m_searchFieldsWidget;
	ResultModel *m_resultModel;
	RequestsListModel *m_requestsModel;
	bool m_done;
	QList<QAction*> m_actions;
};

class AbstractSearchFormFactory : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SearchForm")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "SearchLayer")
public:
	static AbstractSearchFormFactory *instance();
	virtual AbstractSearchForm *createForm(const QList<AbstractSearchFactory*> &factories,
										   const QString &title = QString(),
										   const QIcon &icon = QIcon(),
										   QWidget *parent = 0) = 0;
};

} // namespace Core

#endif // EARCHFORM_H

