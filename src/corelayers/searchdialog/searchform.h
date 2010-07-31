/****************************************************************************
 *  searchform.h
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
#include "ui_searchform.h"
#include <QListView>

namespace Core {

class SearchForm : public QWidget
{
	Q_OBJECT
public:
	explicit SearchForm(QMetaObject *factory, const QString &title = QString(),
						const QIcon &icon = QIcon(), QWidget *parent = 0);
	~SearchForm();
private slots:
	void startSearch();
	void updateRequest(int row);
	void updateFields();
	void clearFields();
	void cancelSearch();
	void done(bool ok);
	void actionButtonClicked();
	void updateService();
	void updateServiceBox();
	void updateActionButtons();
protected:
	bool event(QEvent *e);
	void setState(bool search);
private:
	void actionButtonClicked(int actionIndex, int row);
	Ui::SearchForm ui;
	RequestPtr m_currentRequest;
	QPointer<QWidget> m_searchWidget;
	ResultModel m_resultModel;
	RequestsListModel *m_requestsModel;
	bool m_done;
	QList<QPushButton*> m_actionButtons;
};

} // namespace Core

#endif // EARCHFORM_H
