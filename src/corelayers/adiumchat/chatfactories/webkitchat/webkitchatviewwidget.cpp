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
	if(m_view->page()) {
		QWebFrame *frame = m_view->page()->mainFrame();
		if(frame->scrollBarValue(Qt::Vertical) == frame->scrollBarMaximum(Qt::Vertical))
			frame->setProperty("scrollbarAtEnd", true);
		else {
			frame->setProperty("scrollbarPos",frame->scrollBarValue(Qt::Vertical));
			frame->setProperty("scrollbarAtEnd", false);
		}
	}

	ChatStyleOutput *newPage = qobject_cast<ChatStyleOutput*>(controller);
	if(newPage) {
		//nice hack for new sessions
		QWebFrame *frame = newPage->mainFrame();
		if (!frame->property("scrollbarPos").toBool())
			frame->setProperty("scrollbarAtEnd",true);

		m_view->page()->setView(0);
		m_view->setPage(newPage);
		QTimer::singleShot(0,this,SLOT(scrollBarWorkaround()));
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
