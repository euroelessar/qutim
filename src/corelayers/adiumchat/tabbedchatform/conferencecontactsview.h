#ifndef CONFERENCECONTACTSVIEW_H
#define CONFERENCECONTACTSVIEW_H

#include <QListView>

namespace Core
{
namespace AdiumChat
{

class ChatSessionImpl;
class ConferenceContactsView : public QListView
{
    Q_OBJECT
public:
    explicit ConferenceContactsView(QWidget *parent = 0);
	void setSession(ChatSessionImpl *session);
protected:
	virtual bool event(QEvent *event);
private:
	ChatSessionImpl *m_session;
};

}
}
#endif // CONFERENCECONTACTSVIEW_H
