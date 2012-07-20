/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2007 Trolltech ASA <info@trolltech.com>
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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include "simplecontactlist_global.h"

class QToolButton;

class SIMPLECONTACTLIST_EXPORT LineEdit : public QLineEdit
{
	Q_OBJECT
public:
	LineEdit(QWidget *parent = 0);
protected:
	void resizeEvent(QResizeEvent *);
private slots:
	void updateCloseButton(const QString &text);
private:
	QToolButton *clearButton;
};

#endif // LIENEDIT_H

