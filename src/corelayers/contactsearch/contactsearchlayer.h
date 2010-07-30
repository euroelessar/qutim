/****************************************************************************
 *  contactsearchlayer.h
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

#ifndef CONTACTSEARCHLAYER_H
#define CONTACTSEARCHLAYER_H

#include "contactsearchform.h"

namespace Core
{

class ContactSearch : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactSearch")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactList")
public:
    ContactSearch();
public slots:
	void show(QObject*);
private:
	QPointer<ContactSearchForm> searchContactForm;
};

}

#endif // CONTACTSEARCHLAYER_H
