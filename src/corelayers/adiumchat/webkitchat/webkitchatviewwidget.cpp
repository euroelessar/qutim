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

}
}
