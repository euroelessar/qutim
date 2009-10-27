#include <QtDebug>
#include <QTextDocument>
#include <QStringList>
#include "chatstyle.h"
#include "chatstyleoutput.h"
#include <libqutim/libqutim_global.h>
#include "chatsessionimpl.h"
#include <QDateTime>
#include <libqutim/configbase.h>

namespace AdiumChat
{
	
	ChatStyleOutput::ChatStyleOutput ()
	{
		loadSettings();
 		ChatStyleGenerator generator (m_current_style_path,m_current_variant);
 		m_current_style = generator.getChatStyle();
	}

	void ChatStyleOutput::loadSettings()
	{
		ConfigGroup adium_chat = Config("appearance").group("adiumChat/style");
		m_current_style_path = getThemePath(adium_chat.value<QString>("name","default"),
											"webkitstyle");
		m_current_variant = adium_chat.value<QString>("variant","default");
		m_current_datetime_format = adium_chat.value<QString>("datetimeFormat","hh:mm:ss dd/MM/yyyy");
	}

	void ChatStyleOutput::reloadStyle()
	{
	}

	ChatStyleOutput::~ChatStyleOutput()
	{

	}

	StyleVariants ChatStyleOutput::getVariants() const
	{
		return m_current_style.variants;
	}

	QString ChatStyleOutput::getMainCSS()
	{
		return m_current_style.mainCSS;
	}

	QString ChatStyleOutput::getVariantCSS()
	{
	if(!m_current_variant.isEmpty())
		return "Variants/" + m_current_variant + ".css";
	else
		return QString();
	}

	void ChatStyleOutput::preparePage ( QWebPage* page ) const
	{
		//FIXME
	}

	QStringList ChatStyleOutput::getPaths()
	{
		QStringList paths;
		paths << m_current_style.baseHref;
		paths << m_current_style.baseHref + "Variants/";
		paths << m_current_style.baseHref + "Images/";
		paths << m_current_style.baseHref + "Incoming/";
		paths << m_current_style.baseHref + "Outgoing/";
		paths << m_current_style.baseHref + "styles/";

		return paths;
	}

	QString ChatStyleOutput::makeSkeleton ( const QString& _chatName, const QString& _ownerName,
											const QString& _partnerName, const QString& _ownerIconPath,
											const QString& _partnerIconPath, const QDateTime& datetime,
											const QString& _time)
	{
		QString headerHTML = m_current_style.headerHtml;
		QString footerHTML = m_current_style.footerHtml;

		QString generalSkeleton = m_current_style.templateHtml;
		generalSkeleton.replace(generalSkeleton.indexOf("%@"),2,m_current_style.baseHref);
		generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,m_current_style.footerHtml);
		generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,m_current_style.headerHtml);
		generalSkeleton.replace(generalSkeleton.lastIndexOf("%@"),2,getVariantCSS());
		if(generalSkeleton.contains("%@"))
			generalSkeleton.replace(generalSkeleton.indexOf("%@"),2,"@import url( \"main.css\" );");
	//	generalSkeleton.replace("%rep2%", "main.css");
	//	generalSkeleton.replace("%rep3%", "Variants/" + variantUsedName + ".css");
	//	generalSkeleton.replace("%rep4%", headerHTML);

		generalSkeleton = generalSkeleton.replace("%chatName%", Qt::escape(_chatName));
		generalSkeleton = generalSkeleton.replace("%sourceName%", Qt::escape(_ownerName));
		generalSkeleton = generalSkeleton.replace("%destinationName%", Qt::escape(_partnerName));
		generalSkeleton = generalSkeleton.replace("%timeOpened%", Qt::escape(_time));

		static QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
		int pos=0;
		while((pos=timeRegExp.indexIn(generalSkeleton, pos)) != -1)
			generalSkeleton.replace(pos, timeRegExp.cap(0).length(), Qt::escape(convertTimeDate(timeRegExp.cap(1), datetime)));

		if(_ownerIconPath == "")
			generalSkeleton = generalSkeleton.replace("%outgoingIconPath%", "outgoing_icon.png");
		else
	#if defined(Q_OS_WIN32) //TODO FIXME remove hacks!
			generalSkeleton = generalSkeleton.replace("%outgoingIconPath%", _ownerIconPath);
	#else
			generalSkeleton = generalSkeleton.replace("%outgoingIconPath%", "file://" + _ownerIconPath);
	#endif

		if(_partnerIconPath == "")
			generalSkeleton = generalSkeleton.replace("%incomingIconPath%", "incoming_icon.png");
		else
	#if defined(Q_OS_WIN32)
			generalSkeleton = generalSkeleton.replace("%incomingIconPath%", _partnerIconPath);
	#else
			generalSkeleton = generalSkeleton.replace("%incomingIconPath%", "file://" + _partnerIconPath);
	#endif
		return generalSkeleton;
	}

	void ChatStyleOutput::setVariant ( const QString& _variantName )
	{
		m_current_variant = _variantName;
	}

	QString ChatStyleOutput::makeMessage ( const QString& _name, const QString& _message,
										   const bool& _direction, const QDateTime& datetime,
										   const QString& _avatarPath, const bool& _aligment, const QString& _senderID,
										   const QString& _service, const bool& _sameSender, bool _history )
	{
		// prepare values, so they could be inserted to html code
		QString html;

		if(_history)
		{
			if ( _direction )
				html = _sameSender ? m_current_style.nextOutgoingHistoryHtml : m_current_style.outgoingHistoryHtml;
			else
				html = _sameSender ? m_current_style.nextIncomingHistoryHtml : m_current_style.incomingHistoryHtml;
		}
		else
		{
			if ( _direction )
				html = _sameSender ? m_current_style.nextOutgoingHtml : m_current_style.outgoingHtml;
			else
				html = _sameSender ? m_current_style.nextIncomingHtml : m_current_style.incomingHtml;
		}

		QString avatarPath = _avatarPath.isEmpty() ? QLatin1String(":/icons/qutim_64") : avatarPath; //FIXME set buddy icon

		// Replace %sender% to name
		html = html.replace("%sender%", _name);
		// Replace %senderScreenName% to name
		html = html.replace("%senderScreenName%", Qt::escape(_senderID));
		// Replace %time% to time //FIXME
		QString _time = datetime.toString(m_current_datetime_format); //HACK something fucking unknown !
		html = html.replace("%time%", Qt::escape(_time));
		// Replace %time{X}%
		static QRegExp timeRegExp("%time\\{([^}]*)\\}%");
		int pos=0;
		while((pos=timeRegExp.indexIn(html, pos)) != -1)
			html.replace(pos, timeRegExp.cap(0).length(), Qt::escape(convertTimeDate(timeRegExp.cap(1), datetime)));
		// Replace %service% to protocol name
		// TODO: have to get protocol global value somehow
		html = html.replace("%service%", _service);
		// Replace %protocolIcon% to sender statusIcon path
		// TODO: find icon to add here
		html = html.replace("%senderStatusIcon%", "");
		// Replace userIconPath
		if(avatarPath == "")
		{
			if(_direction)
				avatarPath = (m_current_style.baseHref + "Outgoing/buddy_icon.png");
			else
				avatarPath = (m_current_style.baseHref + "Incoming/buddy_icon.png");

		}
		html = html.replace("%userIconPath%", avatarPath);

		// search for background colors and change them, so CSS would stay clean
		QString bgColor = "inherit";
		static QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
		int textPos=0;
		while((textPos=textBackgroundRegExp.indexIn(html, textPos)) != -1)
		{
			html = html.replace(textPos, textBackgroundRegExp.cap(0).length(), bgColor);
		}

		// Replace %messageDirection% with "rtl"(Right-To-Left) or "ltr"(Left-to-right)
		html = html.replace("%messageDirection%", _aligment ? "ltr" : "rtl" );

		// Replace %messages%, replacing last to avoid errors if messages contains tags
		QString message = _message;
		html = html.replace("%message%", message.replace("\\","\\\\").remove('\r').replace("%","&#37;")+"&nbsp;");

		return html;
	}
	
	QString ChatStyleOutput::makeAction ( const QString& _name, const QString& _message,
									  const bool& _direction, const QDateTime& datetime,
									  const QString& _time, const QString& _avatarPath,
									  const bool& _aligment, const QString& _senderID, const QString& _service )
	{
		QString html = _direction ? m_current_style.outgoingActionHtml:m_current_style.incomingActionHtml;

		QString avatarPath = _avatarPath;

		// Replace %sender% to name
		html = html.replace("%sender%", _name);
		// Replace %senderScreenName% to name
		html = html.replace("%senderScreenName%", Qt::escape(_senderID));
		// Replace %time% to time
		html = html.replace("%time%", Qt::escape(_time));
		// Replace %time{X}%
		static QRegExp timeRegExp("%time\\{([^}]*)\\}%");
		int pos=0;
		while((pos=timeRegExp.indexIn(html, pos)) != -1)
			html.replace(pos, timeRegExp.cap(0).length(), Qt::escape(convertTimeDate(timeRegExp.cap(1), datetime)));
		// Replace %service% to protocol name
		// TODO: have to get protocol global value somehow
		html = html.replace("%service%", _service);
		// Replace %protocolIcon% to sender statusIcon path
		// TODO: find icon to add here
		html = html.replace("%senderStatusIcon%", "");
		// Replace userIconPath
		if(avatarPath == "")
		{
			if(_direction)
				avatarPath = (m_current_style.baseHref + "Outgoing/buddy_icon.png");
			else
				avatarPath = (m_current_style.baseHref + "Incoming/buddy_icon.png");

		}
		html = html.replace("%userIconPath%", avatarPath);

		// search for background colors and change them, so CSS would stay clean
		QString bgColor = "inherit";
		static QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
		int textPos=0;
		while((textPos=textBackgroundRegExp.indexIn(html, textPos)) != -1)
		{
			html = html.replace(textPos, textBackgroundRegExp.cap(0).length(), bgColor);
		}

		// Replace %messageDirection% with "rtl"(Right-To-Left) or "ltr"(Left-to-right)
		html = html.replace("%messageDirection%", _aligment ? "ltr" : "rtl" );

		// Replace %messages%, replacing last to avoid errors if messages contains tags
		QString message = _message;
		html = html.replace("%message%", message.replace("\\","\\\\").remove('\r').replace("%","&#37;")+"&nbsp;");

		return html;
	}

	QString ChatStyleOutput::makeStatus ( const QString& _message, const QDateTime& datetime, const QString& _time )
	{
		QString html = m_current_style.statusHtml;
		// Replace %time% to time
		html = html.replace("%time%", Qt::escape(_time));
		// Replace %time{X}%
		static QRegExp timeRegExp("%time\\{([^}]*)\\}%");
		int pos=0;
		while((pos=timeRegExp.indexIn(html, pos)) != -1)
			html.replace(pos, timeRegExp.cap(0).length(), Qt::escape(convertTimeDate(timeRegExp.cap(1), datetime)));
		// Replace %message%'s, replacing last to avoid errors if messages contains tags
		html = html.replace("%message%", Qt::escape(_message).replace("\\","\\\\").remove('\r').replace("%","&#37;").replace("\n","<br/>")+"&nbsp;");
		return html;
	}

	QString ChatStyleOutput::findEmail ( const QString& _sourceHTML )
	{
		QString html = _sourceHTML;
		static QRegExp emailRegExp("((?:\\w+\\.)*\\w+@(?:\\w+\\.)*\\w+)");
		emailRegExp.indexIn(html);
		for(int i=0;i<emailRegExp.numCaptures();i++)
		{
			QString email = emailRegExp.cap(i);
			email = "<a href=\"mailto:" + email + "\">" + email + "</a>";
			html.replace(emailRegExp.cap(i), email);
		}
		return html;
	}
	
	QString ChatStyleOutput::findWebAddress ( const QString& _sourceHTML )
	{
		QString html = _sourceHTML;
		static QRegExp linkRegExp("(([a-z]+://|www\\d?\\.)[^\\s]+)");
		int pos = 0;
		while((pos=linkRegExp.indexIn(html, pos)) != -1)
		{
			QString link = linkRegExp.cap(0);
			link = "<a href='" + link + "' target='_blank'>" + link + "</a>";
			html.replace(linkRegExp.cap(0), link);
			pos += link.count();
		}
		return html;
	}

}
