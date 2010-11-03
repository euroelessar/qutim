#ifndef CHATBEHAVIOR_H
#define CHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <chatlayer/chatedit.h>
#include <tabbedchatform/tabbedchatwidget.h>

namespace Ui {
class ChatBehavior;
}
class QButtonGroup;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class ChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit ChatBehavior();
	virtual ~ChatBehavior();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void onButtonClicked(int id);
	void onValueChanged();
private:
	void setFlags(AdiumChat::ChatFlags,bool set = true);
	Ui::ChatBehavior *ui;
	AdiumChat::SendMessageKey m_send_message_key;
	QButtonGroup *m_group;
	AdiumChat::ChatFlags m_flags;
};

}
}
#endif // CHATBEHAVIOR_H
