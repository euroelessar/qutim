#ifndef CAPTCHASENDER_H
#define CAPTCHASENDER_H

#include <QObject>

namespace qutim_sdk_0_3 {
	class Message;
	class ChatSession;
class ChatUnit;
}

namespace Antispam
{
	using namespace qutim_sdk_0_3;
	
	class CaptchaSender : public QObject
	{
		Q_OBJECT
	public:
		CaptchaSender(QObject* parent = 0);
	public slots:
		void messageReceived(qutim_sdk_0_3::Message*);
		void sendQuestion(qutim_sdk_0_3::Message* message);
		void checkAnswer(qutim_sdk_0_3::Message*);
	private:
	};

}

#endif // CAPTCHASENDER_H
