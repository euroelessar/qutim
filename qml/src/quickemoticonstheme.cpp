#include "quickemoticonstheme.h"
#include <QUrl>
#include <QHash>
#include <QVariantMap>
#include <QStringList>

namespace qutim_sdk_0_3 {

QuickEmoticonsTheme::QuickEmoticonsTheme(QObject *parent) :
    QObject(parent)
{
}

QString QuickEmoticonsTheme::themeName() const
{
    return m_themeName;
}

QVariantList QuickEmoticonsTheme::emoticons() const
{
    return m_emoticons;
}

void QuickEmoticonsTheme::setThemeName(const QString &themeName)
{
    if (m_themeName != themeName) {
        m_themeName = themeName;
        m_theme = EmoticonsTheme(themeName);
        emit themeNameChanged(themeName);

        m_emoticons.clear();
        const auto emoticonsMap = m_theme.emoticonsMap();
        for (auto it = emoticonsMap.begin(); it != emoticonsMap.end(); ++it) {
            const QString &file = it.key();
            const QStringList &names = it.value();
            m_emoticons.append(QVariantMap({
                { QStringLiteral("url"), QUrl::fromLocalFile(file) },
                { QStringLiteral("names"), names }
            }));
        }
        emit emoticonsChanged(m_emoticons);
    }
}

} // namespace qutim_sdk_0_3
