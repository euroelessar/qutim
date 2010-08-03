/****************************************************************************
 *  abstractsearchform.h
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
	QWidget *searchFieldsWidget() { return m_searchFieldsWidget; }
	void clearActionButtons();
	void actionButtonClicked(QAction *button, const QList<QModelIndex> &selected);
private slots:
	void done(bool ok);
protected:
	bool event(QEvent *e);
private:
	RequestPtr m_currentRequest;
	QPointer<QWidget> m_searchFieldsWidget;
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
