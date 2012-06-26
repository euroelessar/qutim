#include "chatpreview.h"
#include <qutim/thememanager.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

ChatPreview::ChatPreview(QObject *parent)
	: QObject(parent)
{
	QDir shareDir = ThemeManager::path(QLatin1String("data"), QLatin1String("webview"));
	Q_ASSERT(shareDir.exists(QLatin1String("Preview.plist")));
	m_preview = WebKitPreviewLoader().loadPreview(shareDir.filePath(QLatin1String("Preview.plist")));
}

ChatPreview::~ChatPreview()
{
}

ChatSession *ChatPreview::session() const
{
	return m_preview->session.data();
}

QVariantList ChatPreview::messages() const
{
	QVariantList messages;
	foreach (const Message &message, m_preview->messages)
		messages << qVariantFromValue(message);
    return messages;
}

QStringList ChatPreview::fontFamilies() const
{
    return m_database.families();
}

QVariantList ChatPreview::fontSizes() const
{
    QVariantList result;
    foreach (int size, m_database.standardSizes())
        result << size;
    return result;
}

} // namespace MeegoIntegration
