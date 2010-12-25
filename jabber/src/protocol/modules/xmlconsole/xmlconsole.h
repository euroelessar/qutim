#ifndef XMLCONSOLE_H
#define XMLCONSOLE_H

#include <QtGui/QWidget>
#include "sdk/jabber.h"
#include <jreen/client.h>

namespace Ui {
    class XmlConsole;
}

namespace Jabber
{
	class XmlConsole : public QWidget, public JabberExtension, public jreen::XmlStreamHandler
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
	public:
		XmlConsole(QWidget *parent = 0);
		~XmlConsole();

		void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
		virtual void handleStreamBegin();
		virtual void handleStreamEnd();
		virtual void handleIncomingData(const char *data, qint64 size);
		virtual void handleOutgoingData(const char *data, qint64 size);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::XmlConsole *m_ui;
		jreen::Client *m_client;
		QByteArray m_incoming;
		QByteArray m_outgoing;
	
private slots:
    void on_filterButton_clicked();
};
}

#endif // XMLCONSOLE_H
