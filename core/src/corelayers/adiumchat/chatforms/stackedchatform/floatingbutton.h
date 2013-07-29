/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef FLOATINGBUTTON_H
#define FLOATINGBUTTON_H

#include <qtoolbutton.h>
#include <qevent.h>
#include <qutim/icon.h>

namespace Core
{
    namespace AdiumChat
    {
	class FloatingButton : public QToolButton
	{
	    Q_OBJECT
	public:
	    inline explicit FloatingButton(int type,QWidget *parent);

	protected:
	    inline bool eventFilter(QObject *obj, QEvent *ev);
	    int m_type;
	};

	FloatingButton::FloatingButton(int type,QWidget *parent)
	    : QToolButton(parent)
	{
	    Q_ASSERT(parent);

	    if ((type==0)||(type==3)) {
#ifdef Q_WS_MAEMO_5
		// set the next icon from Maemo's theme
		setIcon(QIcon::fromTheme(QLatin1String("general_forward")));
#else
		setIcon(qutim_sdk_0_3::Icon("arrow-right"));
#endif
	    }
	    else if (type==1) {
#ifdef Q_WS_MAEMO_5
		// set the close icon from Maemo's theme
		setIcon(QIcon::fromTheme(QLatin1String("camera_overlay_close")));
#else
		setIcon(qutim_sdk_0_3::Icon("dialog-close"));
#endif
	    }
	    else if (type==2) {
#ifdef Q_WS_MAEMO_5
		// set the close icon from Maemo's theme
		setIcon(QIcon::fromTheme(QLatin1String("general_contacts_button")));
#else
		setIcon(qutim_sdk_0_3::Icon("view-user-offline-kopete"));
#endif
	    }
	    else if (type==4) {
#ifdef Q_WS_MAEMO_5
		// set the keyboard icon from Maemo's theme
		setIcon(QIcon::fromTheme(QLatin1String("call_dialpad_button")));
#endif
	    }

	    m_type=type;

	    setIconSize(QSize(48,48));

	    // ensure that our size is fixed to our ideal size
	    setFixedSize(sizeHint());

	    // set the background to 0.5 alpha
	    QPalette pal = palette();
	    QColor backgroundColor = pal.color(backgroundRole());
	    backgroundColor.setAlpha(32);
	    pal.setColor(backgroundRole(), backgroundColor);
	    setPalette(pal);

	    // ensure that we're painting our background
	    setAutoFillBackground(true);

	    // install an event filter to listen for the parent's events
	    parent->installEventFilter(this);
	}

	bool FloatingButton::eventFilter(QObject *obj, QEvent *ev)
	{
	    if (obj != parent())
		return QToolButton::eventFilter(obj, ev);

	    QWidget *parent = parentWidget();

	    switch (ev->type()) {
	    case QEvent::Resize:
		if (m_type==0) {
		    move(parent->width() - width(), parent->height()/2 - height());
		} else if (m_type==1){
		    move(parent->width() - width(),0);
		} else if (m_type==2){
		    move(parent->width() - width(),height());
		} else if (m_type==3){
		    move(parent->width() - width(),height()*2);
		} else if (m_type==4){
		    move(parent->width() - width(),height()*3);
		}
		break;
	default:
		break;
	    }

	    return QToolButton::eventFilter(obj, ev);
	}
    }
}
#endif

