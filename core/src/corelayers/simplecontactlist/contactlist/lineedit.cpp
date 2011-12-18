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

#include "lineedit.h"
#include <QToolButton>
#include <QStyle>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	clearButton = new QToolButton(this);
	clearButton->setIcon(Icon("edit-clear-locationbar-rtl"));
	clearButton->setCursor(Qt::ArrowCursor);
	clearButton->setStyleSheet("QToolButton { border: none; padding: 2px; }");
	clearButton->setToolTip(tr("Clear search field"));
	clearButton->hide();

	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 2));

	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
				   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));

	connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateCloseButton(QString)));
}

void LineEdit::resizeEvent(QResizeEvent *)
{
	QSize sz = clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	clearButton->move(rect().right() - frameWidth - sz.width(),
					  (rect().bottom() + 1 - sz.height())/2);
}

void LineEdit::updateCloseButton(const QString& text)
{
	clearButton->setVisible(!text.isEmpty());
}


