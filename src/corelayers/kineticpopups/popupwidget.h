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

#include <QTextBrowser>
#include "themehelper.h"

class QTextBrowser;
namespace KineticPopups
{
	class PopupWidget : public QTextBrowser
	{
		Q_OBJECT
	public:
		PopupWidget(const ThemeHelper::PopupSettings &popupSettings);
		PopupWidget();
		void setData(const QString& title,
					const QString& body,
					QObject *sender,
					const QVariant &data); //size of textbrowser

		void setTheme(const ThemeHelper::PopupSettings &popupSettings);
		virtual void mouseReleaseEvent ( QMouseEvent* ev );
		virtual ~PopupWidget();
		virtual QSize sizeHint() const;
	public slots:
		void onTimeoutReached();
	signals:
		void activated();
		void sizeChanged(const QSize &size);
	private slots:
		//TODO need refactoring in future
		void onAction1Triggered();
		void onAction2Triggered();
	private:
		ThemeHelper::PopupSettings popup_settings;
		void init(const ThemeHelper::PopupSettings &popupSettings);
		QObject *m_sender;
		QVariant m_data;
	};
}
#endif // POPUPWIDGET_H
