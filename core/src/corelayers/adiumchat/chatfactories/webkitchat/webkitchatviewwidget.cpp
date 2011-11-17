/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
** Copyright (C) 2011 Aleksey Sidorov gorthauer87@yandex.ru
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
#include "webkitchatviewwidget.h"
#include "chatstyleoutput.h"
#include <QKeyEvent>
#include <QWebView>
#include <QWebFrame>
#include <QVBoxLayout>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{

WebkitChatViewWidget::WebkitChatViewWidget() :
	QFrame(),
	ChatViewWidget(),
	m_view(new QWebView),
	mousePressed(false)
{
	new QVBoxLayout(this);
	layout()->addWidget(m_view);
	layout()->setMargin(0);
	m_view->installEventFilter(this);
	setFrameStyle(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);
	QTimer::singleShot(0, this, SLOT(initScrolling()));
}

void WebkitChatViewWidget::initScrolling()
{
	if(QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller,
								  "enableScrolling",
								  Q_ARG(QObject*, m_view));
}

void WebkitChatViewWidget::setViewController(QObject *controller)
{
	//save scrollbar state
	if (m_view->page()) {
		QWebFrame *frame = m_view->page()->mainFrame();
		if(frame->scrollBarValue(Qt::Vertical) == frame->scrollBarMaximum(Qt::Vertical))
			frame->setProperty("scrollbarAtEnd", true);
		else {
			frame->setProperty("scrollbarPos",frame->scrollBarValue(Qt::Vertical));
			frame->setProperty("scrollbarAtEnd", false);
		}
	}

	ChatStyleOutput *newPage = qobject_cast<ChatStyleOutput*>(controller);
	if (newPage) {
		//nice hack for new sessions
		QWebFrame *frame = newPage->mainFrame();
		if (!frame->property("scrollbarPos").toBool())
			frame->setProperty("scrollbarAtEnd", true);

        //newPage->setParent(m_view);
        m_view->page()->setView(0);
		m_view->setPage(newPage);
#if QTWEBKIT_VERSION >= QTWEBKIT_VERSION_CHECK(2, 2, 0)
		//HACK workaround for blank chat logs
		m_view->setHtml(m_view->page()->mainFrame()->toHtml());
#endif
		QTimer::singleShot(0, this, SLOT(scrollBarWorkaround()));
	} else
		m_view->setPage(0);
}

bool WebkitChatViewWidget::event(QEvent *event)
{
	return QFrame::event(event);
}

bool WebkitChatViewWidget::eventFilter(QObject *obj, QEvent *event)
{
	if(obj->metaObject() == &QWebView::staticMetaObject) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->matches(QKeySequence::Copy)) {
				m_view->triggerPageAction(QWebPage::Copy);
				return true;
			}
		}
	}
#ifdef QUTIM_MOBILE_UI
	switch (event->type()) {
	case QEvent::MouseButtonPress:
	    if (static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
		mousePressed = true;
	    break;
	case QEvent::MouseButtonRelease:
	    if (static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
		mousePressed = false;
	    break;
	case QEvent::Gesture:
		 return true;
	    break;
	case QEvent::MouseMove:
	    if (mousePressed)
		return true;
	    break;

	    break;
	default:
	    break;
	}
	return false;
#endif
	return QFrame::eventFilter(obj,event);
}

void WebkitChatViewWidget::scrollBarWorkaround()
{
	QWebFrame *frame = m_view->page()->mainFrame();	
	if(frame->property("scrollbarAtEnd").toBool())
		frame->setScrollPosition(QPoint(0,frame->scrollBarMaximum(Qt::Vertical)));
	else
		frame->setScrollPosition(QPoint(0,frame->property("scrollbarPos").toInt()));
}

}
}

