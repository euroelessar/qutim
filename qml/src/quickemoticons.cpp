#include "quickemoticons.h"
#include <qutim/emoticons.h>

namespace qutim_sdk_0_3 {

QuickEmoticons::QuickEmoticons(QObject *parent) :
    QObject(parent)
{
}

QString QuickEmoticons::themeName() const
{
    return Emoticons::currentThemeName();
}

void QuickEmoticons::setThemeName(const QString &themeName)
{
    if (Emoticons::currentThemeName() != themeName) {
        Emoticons::setTheme(themeName);
        emit themeNameChanged(themeName);
    }
}

QStringList QuickEmoticons::themeList() const
{
    return Emoticons::themeList();
}

} // namespace qutim_sdk_0_3
