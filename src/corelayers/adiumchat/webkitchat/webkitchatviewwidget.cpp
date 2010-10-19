#include "webkitchatviewwidget.h"
#include "chatstyleoutput.h"

namespace Core
{
namespace AdiumChat
{

WebkitChatViewWidget::WebkitChatViewWidget() :
	QWebView(),
	ChatViewWidget()
{
}

void WebkitChatViewWidget::setViewController(QObject *controller)
{
	//FIXME don't use dynamic_cast, may be use qvariant ?
	ChatStyleOutput *new_page = qobject_cast<ChatStyleOutput*>(controller);
	if(new_page) {
		page()->setView(0);
		setPage(new_page);
	}
}

bool WebkitChatViewWidget::event(QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->matches(QKeySequence::Copy)) {
			triggerPageAction(QWebPage::Copy);
			return true;
		}
	}
	return QWebView::event(event);
}

}
}
