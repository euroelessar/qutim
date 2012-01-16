/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "webkitmessageviewstyle.h"
#include "webkitpreviewloader.h"

namespace Ui {
    class Widget;
}

class QWebPage;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
	
public slots:
	void onLoad();
	void onTimer();

private slots:
	void on_comboBox_currentIndexChanged(const QString &arg1);
	
private:
	void fillPage(QWebPage *page);
	QStringList contentScripts();
	
private:
    Ui::Widget *ui;
	WebKitMessageViewStyle m_style;
	IPreview::Ptr m_preview;
};

#endif // WIDGET_H
