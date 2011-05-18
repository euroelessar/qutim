#ifndef WEBKITCHATVIEWWIDGET_H
#define WEBKITCHATVIEWWIDGET_H
#include <QFrame>
#include <chatlayer/chatviewfactory.h>

class QWebView;
namespace Core
{
namespace AdiumChat
{

class WebkitChatViewWidget : public QFrame, public ChatViewWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
    WebkitChatViewWidget();
	virtual void setViewController(QObject *controller);
private slots:
	void scrollBarWorkaround();
	void initScrolling();
protected:
	bool event(QEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	QWebView *m_view;
	bool mousePressed;
};

}
}
#endif // WEBKITCHATVIEWWIDGET_H
