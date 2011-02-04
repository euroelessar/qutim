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
public slots:
	void show();
protected:
	void changeEvent(QEvent *e);
private:
	void stackProcess(const QByteArray &data, bool incoming);
	//		void process(const QByteArray &data, bool incoming);

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

	//		struct Environment
	//		{
	//			Environment() : bodyColor(0), tagColor(0), attributeColor(0), paramColor(0)
	//			{
	//				html.reserve(1024 * 16);
	//			}
	//			void appendText(const QString &text, const QLatin1String &color);
	//			void appendAttribute(const QString &name, const QStringRef &value);

	//			QXmlStreamReader reader;
	//			State state;
	//			int depth;
	//			XmlNode current;
	//			QString html;
	//			QXmlStreamReader::TokenType last;
	//			QString xmlns;
	//			QLatin1String bodyColor;
	//			QLatin1String tagColor;
	//			QLatin1String attributeColor;
	//			QLatin1String paramColor;
	//		};

	struct StackToken
	{
		StackToken(QXmlStreamReader &reader)
		{
			type = reader.tokenType();
			if (type == QXmlStreamReader::StartElement) {
				QStringRef tmp = reader.name();
				startTag.namePointer = new QString(*tmp.string());
				startTag.name = new QStringRef(startTag.namePointer, tmp.position(), tmp.length());
				tmp = reader.namespaceUri();
				startTag.xmlnsPointer = new QString(*tmp.string());
				startTag.xmlns = new QStringRef(startTag.xmlnsPointer, tmp.position(), tmp.length());
				startTag.attributes = new QXmlStreamAttributes(reader.attributes());
				startTag.empty = false;
			} else if (type == QXmlStreamReader::Characters) {
				QStringRef tmp = reader.text();
				charachters.textPointer = new QString(*tmp.string());
				charachters.text = new QStringRef(charachters.textPointer, tmp.position(), tmp.length());
			} else if (type == QXmlStreamReader::EndElement) {
				QStringRef tmp = reader.name();
				endTag.namePointer = new QString(*tmp.string());
				endTag.name = new QStringRef(endTag.namePointer, tmp.position(), tmp.length());
			}
		}
		~StackToken()
		{
			if (type == QXmlStreamReader::StartElement) {
				delete startTag.namePointer;
				delete startTag.name;
				delete startTag.xmlnsPointer;
				delete startTag.xmlns;
				delete startTag.attributes;
			} else if (type == QXmlStreamReader::Characters) {
				delete charachters.textPointer;
				delete charachters.text;
			} else if (type == QXmlStreamReader::EndElement) {
				delete endTag.namePointer;
				delete endTag.name;
			}
		}

		QXmlStreamReader::TokenType type;
		union {
			struct {
				QString *namePointer;
				QStringRef *name;
				QString *xmlnsPointer;
				QStringRef *xmlns;
				QXmlStreamAttributes *attributes;
				bool empty;
			} startTag;
			struct {
				QString *textPointer;
				QStringRef *text;
			} charachters;
			struct {
				QString *namePointer;
				QStringRef *name;
			} endTag;
		};
	};

	struct StackEnvironment
	{
		QXmlStreamReader reader;
		State state;
		int depth;
		QList<StackToken*> tokens;
		QColor bodyColor;
		QColor tagColor;
		QColor attributeColor;
		QColor paramColor;
	};

	Ui::XmlConsole *m_ui;
	jreen::Client *m_client;
	QList<XmlNode> m_nodes;
	//		Environment m_incoming;
	//		Environment m_outgoing;
	StackEnvironment m_stackIncoming;
	StackEnvironment m_stackOutgoing;
	//		QLatin1String m_bracketsColor;
	QColor m_stackBracketsColor;
	int m_filter;
	
private slots:
	void on_lineEdit_textChanged(const QString &);
	void onActionGroupTriggered(QAction *action);
};
}

#endif // XMLCONSOLE_H
