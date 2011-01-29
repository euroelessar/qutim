/****************************************************************************
 *  mergedialog.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MERGEDIALOG_H
#define MERGEDIALOG_H

#include <QWidget>

namespace qutim_sdk_0_3
{
class MetaContact;
class Contact;
}

namespace Core {
namespace MetaContacts {

class Model;


namespace Ui {
    class MergeDialog;
}
class MetaContactImpl;
class MergeDialog : public QWidget
{
    Q_OBJECT

public:
    explicit MergeDialog(QWidget *parent = 0);
    ~MergeDialog();
	void setMetaContact(MetaContactImpl *contact);
protected:
    virtual void closeEvent(QCloseEvent* );	
private slots:
	void addContact(qutim_sdk_0_3::Contact *);
	void removeContact(qutim_sdk_0_3::Contact *);
	void setName(const QString &name);
	void nameChanged(const QString &name);
private:
    Ui::MergeDialog *ui;
	Model *m_model;
};


} // namespace MetaContacts
} // namespace Core
#endif // MERGEDIALOG_H
