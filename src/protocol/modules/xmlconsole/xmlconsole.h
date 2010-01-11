#ifndef XMLCONSOLE_H
#define XMLCONSOLE_H

#include <QtGui/QWidget>
#include "sdk/jabber.h"
#include <gloox/client.h>

namespace Ui {
    class XmlConsole;
}

namespace Jabber
{
	class XmlConsole : public QWidget, public JabberExtension, public gloox::LogHandler
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
	public:
		XmlConsole(QWidget *parent = 0);
		~XmlConsole();

		void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
		void handleLog(gloox::LogLevel level, gloox::LogArea area, const std::string &message);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::XmlConsole *m_ui;
		gloox::Client *m_client;
	};
}

#endif // XMLCONSOLE_H
