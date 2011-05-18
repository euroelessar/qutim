#include "chatviewfactory.h"
#include <qutim/servicemanager.h>
#include <qutim/debug.h>

namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
ChatViewFactory *ChatViewFactory::instance()
{
	ChatViewFactory *f = ServiceManager::getByName<ChatViewFactory*>("ChatViewFactory");
	Q_ASSERT(f);
	return f;
}

UrlTokenList ChatViewFactory::parseUrls(const QString &text)
{
	UrlTokenList result;
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
	                          "([a-z]+(\\+[a-z]+)?://|www\\.)"
	                          "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
	                          "(:\\d+)?"
	                          "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@'&=~-]*"
	                          "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*)?"
	                          "(#[\\w\\+\\.\\[\\]!%\\$/\\\\\\(\\)\\|,:;@&=~-]*)?)?",
	                          Qt::CaseInsensitive);
	Q_ASSERT(linkRegExp.isValid());
	int pos = 0;
	int lastPos = 0;
	while (((pos = linkRegExp.indexIn(text, pos)) != -1)) {
		UrlToken tok = { text.midRef(lastPos, pos - lastPos), QString() };
		if (!tok.text.isEmpty()) {
			if (!result.isEmpty() && result.last().url.isEmpty()) {
				QStringRef tmp = result.last().text;
				result.last().text = QStringRef(tmp.string(), tmp.position(), tmp.size() + tok.text.size());
			} else {
				result << tok;
			}
		}
		QString link = linkRegExp.cap(0);
		tok.text = text.midRef(pos, link.size());
		pos += link.size();
		if (link.startsWith(QLatin1String("www."), Qt::CaseInsensitive))
			link.prepend(QLatin1String("http://"));
		else if(!link.contains(QLatin1String("//")))
			link.prepend(QLatin1String("mailto:"));
		tok.url = link;
		result << tok;
		lastPos = pos;
	}
	if (!result.isEmpty() && result.last().url.isEmpty()) {
		result.last().text = text.midRef(result.last().text.position());
	} else {
		UrlToken tok = { text.midRef(lastPos), QString() };
		result << tok;
	}
	return result;
}
}

}
