#ifndef CHATSTYLE_H
#define CHATSTYLE_H
#include <QString>
#include <QColor>
#include <QMap>

namespace AdiumChat
{
	typedef QMap<QString,QString> StyleVariants;
	struct ChatStyle
	{
		QString styleName;
		QString baseHref;
		QString currentVariantPath;
		QString templateHtml;
		QString headerHtml;
		QString footerHtml;
		QString incomingHtml;
		QString nextIncomingHtml;
		QString outgoingHtml;
		QString nextOutgoingHtml;
		QString incomingHistoryHtml;
		QString nextIncomingHistoryHtml;
		QString outgoingHistoryHtml;
		QString nextOutgoingHistoryHtml;
		QString incomingActionHtml;
		QString outgoingActionHtml;
		QString statusHtml;
		QString mainCSS;
		QColor backgroundColor;
		StyleVariants variants;
		bool backgroundIsTransparent;
		bool isValid();
	};
	
	struct ChatStyleGeneratorPrivate;
	class ChatStyleGenerator
	{
	public:
		ChatStyleGenerator (const QString &stylePath, const QString &variant = QString());
		~ChatStyleGenerator();
		ChatStyle getChatStyle () const;
	private:
		QScopedPointer<ChatStyleGeneratorPrivate> d;
	};
}
#endif // CHATSTYLE_H
