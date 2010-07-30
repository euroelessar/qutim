/****************************************************************************
 *  contactsearchform.h
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

#ifndef CONTACTSEARCHFORM_H
#define CONTACTSEARCHFORM_H

#include "contactsmodel.h"
#include "ui_searchcontactform.h"

namespace Core {

class ContactSearchForm : public QWidget
{
	Q_OBJECT
public:
	ContactSearchForm(QWidget *parent = 0);
private slots:
	void updateRequestBox();
	void startSearch();
	void updateRequest(int index);
	void updateFields();
	void clearFields();
	void cancelSearch();
	void done(bool ok);
	void addContact();
	void updateService();
	void updateServiceBox();
protected:
	bool event(QEvent *e);
	void setState(bool search);
private:
	void addContact(int row);
	Ui::SearchContactForm ui;
	QList<FactoryPtr> m_factories;
	struct RequestBoxItem
	{
		RequestBoxItem() {}
		RequestBoxItem(const FactoryPtr &f, const QString &n) :
				factory(f), name(n)
		{}
		FactoryPtr factory;
		QString name;
	};
	QList<RequestBoxItem> m_requestItems;
	RequestPtr m_currentRequest;
	QPointer<QWidget> m_searchWidget;
	ContactsModel m_contactModel;
	bool requestListUpdating;
};

} // namespace Core

#endif // CONTACTSEARCHFORM_H
