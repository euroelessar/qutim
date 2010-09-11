//   Modern notifications, which use new Qt Statemachine and Animation framework
//   (c) by Sidorov "Sauron" Aleksey, sauron@citadelspb.com, 2010
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Library General Public
//   License version 2 or later as published by the Free Software Foundation.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public License
//   along with this library; see the file COPYING.LIB.  If not, write to
//   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
//   Boston, MA 02110-1301, USA.

#ifndef POPUPWIDGET_H
#define POPUPWIDGET_H

#include <QTimer>
#include "themehelper.h"
#include <backend.h>

class QTextBrowser;
namespace Core
{
namespace KineticPopups
{
class TextPopupWidget : public AbstractPopupWidget
{
	Q_OBJECT
public:
	TextPopupWidget();
	virtual void setData(const QString& title,
				const QString& body,
				QObject *sender,
				const QVariant &data); //size of textbrowser

	virtual void setTheme(const QString &theme);
	virtual ~TextPopupWidget();
	virtual QSize sizeHint() const;
	virtual void timerEvent(QTimerEvent *);
protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);
signals:
	void activated();
	void sizeChanged(const QSize &size);
private slots:
	//TODO need refactoring in future
	void onAction1Triggered();
	void onAction2Triggered();
private:
	void setTheme(const ThemeHelper::PopupSettings &popupSettings);
	ThemeHelper::PopupSettings popup_settings;
	void init(const ThemeHelper::PopupSettings &popupSettings);
	QObject *m_sender;
	QVariant m_data;
	QTimer m_timer;
	QTextBrowser *m_browser;
};
}
}
#endif // POPUPWIDGET_H
