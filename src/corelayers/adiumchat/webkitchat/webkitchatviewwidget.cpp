#include "webkitchatviewwidget.h"
#include "chatstyleoutput.h"
#include <QKeyEvent>
#include <QWebView>
#include <QVBoxLayout>

namespace Core
{
namespace AdiumChat
{

WebkitChatViewWidget::WebkitChatViewWidget() :
	QFrame(),
	ChatViewWidget(),
	m_view(new QWebView)
{
	new QVBoxLayout(this);
	layout()->addWidget(m_view);
	layout()->setMargin(0);
	m_view->installEventFilter(this);
	setFrameStyle(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);
}

void WebkitChatViewWidget::setViewController(QObject *controller)
{
	//FIXME don't use dynamic_cast, may be use qvariant ?
	ChatStyleOutput *new_page = qobject_cast<ChatStyleOutput*>(controller);
	if(new_page) {
		m_view->page()->setView(0);
		m_view->setPage(new_page);
	}
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
	return QFrame::eventFilter(obj,event);
}

}
}
