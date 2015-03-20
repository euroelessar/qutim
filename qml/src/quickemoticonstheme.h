#ifndef QUTIM_SDK_0_3_QUICKEMOTICONSTHEME_H
#define QUTIM_SDK_0_3_QUICKEMOTICONSTHEME_H

#include <QObject>
#include <qutim/emoticons.h>
#include <QVariantList>

namespace qutim_sdk_0_3 {

class QuickEmoticonsTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(QVariantList emoticons READ emoticons NOTIFY emoticonsChanged)
public:
    explicit QuickEmoticonsTheme(QObject *parent = 0);
    
    QString themeName() const;
    QVariantList emoticons() const;
    
signals:
    void themeNameChanged(const QString &themeName);
    void emoticonsChanged(const QVariantList &emoticons);
    
public slots:
    void setThemeName(const QString &themeName);
    
private:
    EmoticonsTheme m_theme;
    QString m_themeName;
    QVariantList m_emoticons;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKEMOTICONSTHEME_H
