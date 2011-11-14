/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef SIMPLETAGSEDITOR_H
#define SIMPLETAGSEDITOR_H

#include <QDialog>
#include <qutim/libqutim_global.h>
#include <QSet>

namespace Ui {
class SimpleTagsEditor;
}

namespace qutim_sdk_0_3
{
class Contact;
}

using namespace qutim_sdk_0_3;

namespace Core
{
class SimpleTagsEditor : public QDialog {
	Q_OBJECT
public:
	SimpleTagsEditor(Contact *contact);
	~SimpleTagsEditor();
	void accept();
public slots:
	void load();
private slots:
	void on_addButton_clicked();
	void save();
protected:
	void changeEvent(QEvent *e);
private:
	Ui::SimpleTagsEditor *ui;
	Contact *m_contact;
};

}
#endif // SIMPLETAGSEDITOR_H

