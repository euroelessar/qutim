#ifndef TABBEDCHATBEHAVIOR_H
#define TABBEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <chatlayer/chatedit.h>
#include "../tabbedchatwidget.h"

namespace Ui {
class TabbedChatBehavior;
}
class QButtonGroup;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class TabbedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit TabbedChatBehavior();
	virtual ~TabbedChatBehavior();
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
	Ui::TabbedChatBehavior *ui;
	AdiumChat::SendMessageKey m_send_message_key;
	QButtonGroup *m_group;
	AdiumChat::ChatFlags m_flags;
};

}
}
#endif // TABBEDCHATBEHAVIOR_H
