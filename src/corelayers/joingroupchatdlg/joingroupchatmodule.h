#ifndef JOINGROUPCHATMODULE_H
#define JOINGROUPCHATMODULE_H
#include <QObject>
#include <libqutim/actiongenerator.h>

namespace Core
{

	class JoinGroupChat;
	using namespace qutim_sdk_0_3;
	
	class JoinGroupChatModule : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "JoinGroupChat")
		Q_CLASSINFO("Uses", "IconLoader")
		Q_CLASSINFO("Uses", "ContactList")
	public:
		explicit JoinGroupChatModule();
		virtual ~JoinGroupChatModule();
	public slots:
		void onJoinGroupChatTriggered();
	private:
		QPointer<JoinGroupChat> m_chat;
	};
	
	class JoinGroupChatGenerator : public ActionGenerator
	{
	public:
		JoinGroupChatGenerator(QObject *module);
		virtual void showImpl(QAction* action, QObject* obj);
	};	
	
}
#endif // JOINGROUPCHATMODULE_H
