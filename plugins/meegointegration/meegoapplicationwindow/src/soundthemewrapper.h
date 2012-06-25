#ifndef SOUNDTHEMEWRAPPER_H
#define SOUNDTHEMEWRAPPER_H

#include <QObject>
#include <qutim/sound.h>
#include <qutim/notification.h>

namespace MeegoIntegration
{
class SoundThemeWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ themeName WRITE setThemeName NOTIFY nameChanged)
public:
    explicit SoundThemeWrapper(QObject *parent = 0);
    
    void setThemeName(const QString &themeName);
    QString themeName() const;
    
public slots:
    QString path(int type) const;
	void play(int type) const;
    static QString currentName();
    static void setCurrentName(const QString &name);
    static QStringList themeList();
    static QString typeText(int type);
	static QString descriptionText(int type);
    
signals:
    void nameChanged(const QString &name);
    
private:
    qutim_sdk_0_3::SoundTheme m_theme;
};
}

#endif // SOUNDTHEMEWRAPPER_H
