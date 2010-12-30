#include "xmlconsole.h"
#include "ui_xmlconsole.h"
#include <qutim/account.h>
#include <qutim/icon.h>
#include <QMenu>
#include <QActionGroup>
#include <QStringBuilder>
#include <QDebug>
#include <QTextLayout>
#include <QPlainTextDocumentLayout>

using namespace jreen;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	XmlConsole::XmlConsole(QWidget *parent) :
		QWidget(parent),
		m_ui(new Ui::XmlConsole),
		m_client(0),
		m_filter(0x1f)
	{
		m_ui->setupUi(this);
		qDebug("%s", Q_FUNC_INFO);
		m_incoming.depth = 0;
		m_outgoing.depth = 0;
		QTextDocument *doc = m_ui->xmlBrowser->document();
		doc->setDocumentLayout(new QPlainTextDocumentLayout(doc));
		doc->clear();
		QTextCursor cur(doc);
		cur.movePosition(QTextCursor::Start);
		QTextFrameFormat format = doc->rootFrame()->frameFormat();
		format.setBackground(QColor(0x888888));
		format.setMargin(0);
		doc->rootFrame()->setFrameFormat(format);
		QMenu *menu = new QMenu(m_ui->filterButton);
		menu->setSeparatorsCollapsible(false);
		menu->addSeparator()->setText(tr("Filter"));
		QActionGroup *group = new QActionGroup(menu);
		QAction *disabled = group->addAction(menu->addAction(tr("Disabled")));
		disabled->setCheckable(true);
		disabled->setData(Disabled);
		QAction *jid = group->addAction(menu->addAction(tr("By JID")));
		jid->setCheckable(true);
		jid->setData(ByJid);
		QAction *xmlns = group->addAction(menu->addAction(tr("By namespace uri")));
		xmlns->setCheckable(true);
		xmlns->setData(ByXmlns);
		QAction *attrb = group->addAction(menu->addAction(tr("By all attributes")));
		attrb->setCheckable(true);
		attrb->setData(ByAllAttributes);
		disabled->setChecked(true);
		connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onActionGroupTriggered(QAction*)));
		menu->addSeparator()->setText(tr("Visible stanzas"));
		group = new QActionGroup(menu);
		group->setExclusive(false);
		QAction *iq = group->addAction(menu->addAction(tr("Information query")));
		iq->setCheckable(true);
		iq->setData(XmlNode::Iq);
		iq->setChecked(true);
		QAction *message = group->addAction(menu->addAction(tr("Message")));
		message->setCheckable(true);
		message->setData(XmlNode::Message);
		message->setChecked(true);
		QAction *presence = group->addAction(menu->addAction(tr("Presence")));
		presence->setCheckable(true);
		presence->setData(XmlNode::Presence);
		presence->setChecked(true);
		QAction *custom = group->addAction(menu->addAction(tr("Custom")));
		custom->setCheckable(true);
		custom->setData(XmlNode::Custom);
		custom->setChecked(true);
		connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onActionGroupTriggered(QAction*)));
		m_ui->filterButton->setMenu(menu);
//		doc->setDocumentMargin(0);
	}

	XmlConsole::~XmlConsole()
	{
		delete m_ui;
	}

	void XmlConsole::init(Account *account, const JabberParams &params)
	{
		qDebug("%s", Q_FUNC_INFO);
		m_client = params.item<Client>();
		account->addAction(new ActionGenerator(Icon("utilities-terminal"),
											   QT_TRANSLATE_NOOP("Jabber", "Xml console"),
											   this, SLOT(show())),
						   "Additional");
		m_client->addXmlStreamHandler(this);
	}
	
	void XmlConsole::handleStreamBegin()
	{
		m_incoming.reader.clear();
		m_outgoing.reader.clear();
		m_incoming.depth = 0;
		m_outgoing.depth = 0;
		m_incoming.xmlns.clear();
		m_outgoing.xmlns.clear();
	}
	
	void XmlConsole::handleStreamEnd()
	{
		m_incoming.reader.clear();
		m_outgoing.reader.clear();
		m_incoming.depth = 0;
		m_outgoing.depth = 0;
	}
	
	void XmlConsole::handleIncomingData(const char *data, qint64 size)
	{
		process(QByteArray::fromRawData(data, size), true);
	}
	
	void XmlConsole::handleOutgoingData(const char *data, qint64 size)
	{
		process(QByteArray::fromRawData(data, size), false);
	}
	
	QString generate_space(int depth)
	{
		depth *= 2;
		QString space;
		while (depth--)
			space += QLatin1String("&nbsp;");
		return space;
	}
	
	void XmlConsole::process(const QByteArray &data, bool incoming)
	{
		Environment *d = &(incoming ? m_incoming : m_outgoing);
		d->reader.addData(data);
		while (d->reader.readNext() > QXmlStreamReader::Invalid) {
			switch(d->reader.tokenType()) {
			case QXmlStreamReader::StartElement:
				if (d->last == QXmlStreamReader::StartElement)
					d->html.append(QLatin1String("&gt;"));
				if (d->depth == 1) {
					d->html.clear();
					d->current.type = XmlNode::Custom;
					if (d->reader.name() == QLatin1String("iq"))
						d->current.type = XmlNode::Iq;
					else if (d->reader.name() == QLatin1String("presence"))
						d->current.type = XmlNode::Presence;
					else if (d->reader.name() == QLatin1String("message"))
						d->current.type = XmlNode::Message;
					d->current.incoming = incoming;
					d->current.time = QDateTime::currentDateTime();
					d->current.xmlns.clear();
					d->current.xmlns << d->reader.namespaceUri().toString();
					d->current.attributes.clear();
					foreach (const QXmlStreamAttribute &attrb, d->reader.attributes())
						d->current.attributes << attrb.value().toString();
					QLatin1String jid = QLatin1String(incoming ? "from" : "to");
					d->current.jid = d->reader.attributes().value(jid).toString();
				}
				if (!d->html.isEmpty())
					d->html.append(QLatin1String("<br/>"));
				d->html.append(generate_space(d->depth));
				d->html.append(QLatin1String("&lt;<b><font color=#800000>"));
				d->html.append(Qt::escape(d->reader.name().toString()));
				d->html.append(QLatin1String("</font></b>"));
				if (d->xmlns != d->reader.namespaceUri() || d->depth == 1) {
					d->xmlns = d->reader.namespaceUri().toString();
					d->html.append(QLatin1String(" <b><font color=#000080>xmlns</font></b>=&quot;"));
					d->html.append(Qt::escape(d->xmlns));
					d->html.append(QLatin1String("&quot;"));
				}
				foreach (const QXmlStreamAttribute &attr, d->reader.attributes()) {
					d->html.append(QLatin1String(" <b><font color=#000080>"));
					d->html.append(Qt::escape(attr.name().toString()));
					d->html.append(QLatin1String("</font></b>=&quot;"));
					d->html.append(Qt::escape(attr.value().toString()));
					d->html.append(QLatin1String("&quot;"));
				}
				d->depth++;
				break;
			case QXmlStreamReader::EndElement:
				d->depth--;
				// &#47; is equal for '/'
				if (d->last == QXmlStreamReader::StartElement) {
					d->html.append(QLatin1String("&#47;&gt;"));
				} else {
					if (d->last != QXmlStreamReader::Characters) {
						d->html.append("<br/>");
						d->html.append(generate_space(d->depth));
					}
					d->html.append(QLatin1String("&lt;&#47;<b><font color=#800000>"));
					d->html.append(Qt::escape(d->reader.name().toString()));
					d->html.append(QLatin1String("</font></b>&gt;"));
				}
				if (d->depth == 1) {
					QTextDocument *doc = m_ui->xmlBrowser->document();
					QTextCursor cur(doc);
					cur.beginEditBlock();
					cur.movePosition(QTextCursor::End);
//					if (!m_nodes.isEmpty())
						cur.insertBlock();
					
					QTextBlockFormat format = cur.block().blockFormat();
					format.setBottomMargin(1);
					format.setTopMargin(0);
					format.setRightMargin(0);
					format.setLeftMargin(0);
					QColor color(incoming ? 0xc8ffc8 : 0xc8c8ff);
					format.setBackground(color);
					cur.setBlockFormat(format);
					
					d->current.block = cur.block();
					d->current.lineCount = cur.block().lineCount();
//					qDebug() << d->current.lineCount;
					d->html.append(QLatin1String("<br/>"));
					cur.insertHtml(d->html);
					cur.endEditBlock();
					
					m_nodes << d->current;
				}
				break;
			case QXmlStreamReader::Characters: {
				if (d->last == QXmlStreamReader::StartElement)
					d->html.append(QLatin1String("&gt;"));
				QString text = Qt::escape(d->reader.text().toString());
				if (text.contains(QLatin1Char('\n'))) {
					QString space = generate_space(d->depth);
					space.prepend("<br/>");
					d->html.append(space);
					text.replace(QLatin1String("\n"), space);
					text.append("<br/>");
					text.append(generate_space(d->depth - 1));
				}
				d->html.append(text);
				break; }
			default:
				break;
			}
			d->last = d->reader.tokenType();
		}
		if (!incoming && d->depth > 1) {
			qFatal("outgoing depth %d on\n\"%s\"", d->depth,
				   qPrintable(QString::fromUtf8(data, data.size())));
		}
	}

	void XmlConsole::changeEvent(QEvent *e)
	{
		QWidget::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
}

void Jabber::XmlConsole::onActionGroupTriggered(QAction *action)
{
	int type = action->data().toInt();
	if (type >= 0x10) {
		m_filter = (m_filter & 0xf) | type;
		m_ui->lineEdit->setEnabled(type != 0x10);
	} else {
		m_filter = m_filter ^ type;
	}
	on_lineEdit_textChanged(m_ui->lineEdit->text());
}

void Jabber::XmlConsole::on_lineEdit_textChanged(const QString &text)
{
	qDebug() << Q_FUNC_INFO;
	int filterType = m_filter & 0xf0;
	JID filterJid = (filterType == ByJid) ? text : QString();
	qDebug() << ("0x" + QString::number(filterType, 16));
    for (int i = 0; i < m_nodes.size(); i++) {
		XmlNode &node = m_nodes[i];
		bool ok = true;
		switch (filterType) {
		case ByXmlns:
			ok = node.xmlns.contains(text);
			break;
		case ByJid:
			ok = node.jid.full() == filterJid.full() || node.jid.bare() == filterJid.full();
			break;
		case ByAllAttributes:
			ok = node.attributes.contains(text);
			break;
		default:
			break;
		}
		ok &= bool(node.type & m_filter);
		node.block.setVisible(ok);
		node.block.setLineCount(ok ? node.lineCount : 0);
//		qDebug() << node.block.lineCount();
	}
	QAbstractTextDocumentLayout *layout = m_ui->xmlBrowser->document()->documentLayout();
	Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(layout));
	qobject_cast<QPlainTextDocumentLayout*>(layout)->requestUpdate();
}
