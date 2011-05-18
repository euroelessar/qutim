#ifndef CONFERENCECONTACTSVIEW_H
#define CONFERENCECONTACTSVIEW_H

#include <QListView>
#include "chatlayer_global.h"

namespace Core
{
namespace AdiumChat
{

class ChatSessionImpl;
class ConferenceContactsViewPrivate;
class ADIUMCHAT_EXPORT ConferenceContactsView : public QListView
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(ConferenceContactsView)
public:
    explicit ConferenceContactsView(QWidget *parent = 0);
	void setSession(ChatSessionImpl *session);
	virtual ~ConferenceContactsView();
protected:
	virtual bool event(QEvent *event);
	void changeEvent(QEvent *);
private:
	QScopedPointer<ConferenceContactsViewPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_activated(const QModelIndex &))
	Q_PRIVATE_SLOT(d_func(), void _q_init_scrolling())
	Q_PRIVATE_SLOT(d_func(), void _q_insert_nick())
};

}
}
#endif // CONFERENCECONTACTSVIEW_H
