#ifndef STACKEDCHATBEHAVIOR_H
#define STACKEDCHATBEHAVIOR_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <chatlayer/chatedit.h>
#include <stackedchatform/stackedchatwidget.h>

namespace Ui {
class StackedChatBehavior;
}
class QButtonGroup;
namespace Core
{
namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class StackedChatBehavior : public SettingsWidget
{
	Q_OBJECT

public:
	explicit StackedChatBehavior();
	virtual ~StackedChatBehavior();
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
	Ui::StackedChatBehavior *ui;
	AdiumChat::SendMessageKey m_send_message_key;
	QButtonGroup *m_group;
	AdiumChat::ChatFlags m_flags;
};

}
}
#endif // STACKEDCHATBEHAVIOR_H
