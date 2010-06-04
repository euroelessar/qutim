#ifndef CHATBEHAVIOR_H
#define CHATBEHAVIOR_H

#include <QWidget>
#include <libqutim/settingswidget.h>
#include "../chatforms/abstractchatwidget.h"

namespace Ui {
    class ChatBehavior;
}
class QButtonGroup;
namespace Core
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
		void onValueChanged(int value);
	private:
		Ui::ChatBehavior *ui;
		AdiumChat::SendMessageKey m_send_message_key;
		QButtonGroup *m_group;
	};

}
#endif // CHATBEHAVIOR_H
