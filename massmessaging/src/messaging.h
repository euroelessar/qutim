#ifndef SIMPLEPLUGIN_H
#define SIMPLEPLUGIN_H
#include <qutim/plugin.h>
#include <QAction>

namespace MassMessaging
{

	using namespace qutim_sdk_0_3;
	
	
	class Manager;
	class MessagingDialog;
		
	class MassMessaging : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "MassMessaging")
	public:
		virtual void init();
		virtual bool load();
		virtual bool unload();
	private slots:
		void onActionTriggered();
	private:
		QPointer<MessagingDialog> m_dialog;
	};

}
#endif
