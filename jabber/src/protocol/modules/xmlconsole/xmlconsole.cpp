#include "xmlconsole.h"
#include "ui_xmlconsole.h"
#include <qutim/account.h>
#include <qutim/icon.h>
#include <QStringBuilder>

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	XmlConsole::XmlConsole(QWidget *parent) :
		QWidget(parent),
		m_ui(new Ui::XmlConsole),
		m_client(0)
	{
		m_ui->setupUi(this);
		qDebug("%s", Q_FUNC_INFO);
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
		m_client->logInstance().registerLogHandler(LogLevelDebug,
												   LogAreaXmlIncoming | LogAreaXmlOutgoing,
												   this);
	}

	void XmlConsole::handleLog(LogLevel, LogArea area, const std::string &message)
	{
		QString xml = QString::fromStdString(message);
		bool in = (area == LogAreaXmlIncoming);
		QString html = QLatin1Literal("<font color=\"")
					   % (in ? QLatin1Literal("yellow") : QLatin1Literal("red"))
					   % QLatin1Literal("\">")
					   % (Qt::escape(xml)
						  .replace("\n","<br/>")
						  .replace("&gt;&lt;","&gt;<br/>&lt;"))
					   % QLatin1Literal("</font><br/><br/>") ;
		m_ui->xmlBrowser->append(html);
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
