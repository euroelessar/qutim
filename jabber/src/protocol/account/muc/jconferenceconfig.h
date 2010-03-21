#ifndef JCONFERENCECONFIG_H
#define JCONFERENCECONFIG_H

#include <gloox/mucroomconfighandler.h>
#include <qutim/icon.h>
#include <QWidget>

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	using namespace gloox;

	class JMUCSession;
	struct JConferenceConfigPrivate;

	class JConferenceConfig : public QWidget, public MUCRoomConfigHandler
	{
		Q_OBJECT
		public:
			JConferenceConfig(MUCRoom *room, QWidget *parent = 0);
			~JConferenceConfig();
			void handleMUCConfigForm(MUCRoom *room, const DataForm &form);
			void handleMUCConfigList(MUCRoom *room, const MUCListItemList &items, MUCOperation operation) {}
			void handleMUCConfigResult(MUCRoom *room, bool success, MUCOperation operation) {}
			void handleMUCRequest(MUCRoom *room, const DataForm &form) {}
		protected:
			void closeEvent(QCloseEvent *);
		signals:
			void destroyDialog();
		private slots:
			void on_applyButton_clicked();
			void on_okButton_clicked();
		private:
			QScopedPointer<JConferenceConfigPrivate> p;
	};
}

#endif // JCONFERENCECONFIG_H
