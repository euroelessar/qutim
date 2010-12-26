#ifndef XMLCONSOLE_H
#define XMLCONSOLE_H

#include <QtGui/QWidget>
#include "sdk/jabber.h"
#include <jreen/client.h>
#include <jreen/jid.h>
#include <QXmlStreamReader>
#include <QDateTime>
#include <QTextBlock>

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
		void process(bool incoming);
		
		struct XmlNode
		{
			enum Type
			{
				Iq = 1,
				Presence = 2,
				Message = 4,
				Custom = 8
			};
			QDateTime time;
			Type type;
			bool incoming;
			QSet<QString> xmlns;
			jreen::JID jid;
			QSet<QString> attributes;
			QTextBlock block;
			int lineCount;
		};
		enum FilterType
		{
			Disabled = 0x10,
			ByJid = 0x20,
			ByXmlns = 0x30,
			ByAllAttributes = 0x40
		};

		enum State
		{
			WaitingForStanza,
			ReadFeatures,
			ReadStanza,
			ReadCustom
		};

		struct Environment
		{
			QXmlStreamReader reader;
			State state;
			int depth;
			XmlNode current;
			QString html;
			QXmlStreamReader::TokenType last;
			QString xmlns;
		};

		Ui::XmlConsole *m_ui;
		jreen::Client *m_client;
		QList<XmlNode> m_nodes;
		Environment m_incoming;
		Environment m_outgoing;
		int m_filter;
	
	private slots:
		void on_lineEdit_textChanged(const QString &);
		void onActionGroupTriggered(QAction *action);
	};
}

#endif // XMLCONSOLE_H
