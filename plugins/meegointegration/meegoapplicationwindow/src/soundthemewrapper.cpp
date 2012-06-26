#include "soundthemewrapper.h"
#include <qutim/notification.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

SoundThemeWrapper::SoundThemeWrapper(QObject *parent) :
    QObject(parent)
{
}

void SoundThemeWrapper::setThemeName(const QString &themeName)
{
    if (m_theme.themeName() != themeName) {
        m_theme = SoundTheme(themeName);
        emit nameChanged(m_theme.themeName());
    }
}

QString SoundThemeWrapper::themeName() const
{
    return m_theme.themeName();
}

QString SoundThemeWrapper::path(int type) const
{
    return m_theme.path(static_cast<Notification::Type>(type));
}

void SoundThemeWrapper::play(int type) const
{
    m_theme.play(static_cast<Notification::Type>(type));
}

QString SoundThemeWrapper::currentName()
{
    return Sound::currentThemeName();
}

void SoundThemeWrapper::setCurrentName(const QString &name)
{
    Sound::setTheme(name);
}

QStringList SoundThemeWrapper::themeList()
{
    return Sound::themeList();
}

QString SoundThemeWrapper::typeText(int type)
{
    return Notification::typeText(static_cast<Notification::Type>(type));
}

QString SoundThemeWrapper::descriptionText(int type)
{
    return Notification::descriptionText(static_cast<Notification::Type>(type));
}
}
