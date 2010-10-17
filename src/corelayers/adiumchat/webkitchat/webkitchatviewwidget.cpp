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

void WebkitChatViewWidget::setViewController(ChatViewController *controller)
{
	//FIXME don't use dynamic_cast
	ChatStyleOutput *new_page = dynamic_cast<ChatStyleOutput*>(controller);
	if(new_page) {
		page()->setView(0);
		setPage(new_page);
	}

}

}
}
