#ifndef QUTIM_SDK_0_3_QUICKEMOTICONS_H
#define QUTIM_SDK_0_3_QUICKEMOTICONS_H

#include <QObject>
#include <QStringList>

namespace qutim_sdk_0_3 {

class QuickEmoticons : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(QStringList themeList READ themeList NOTIFY themeListChanged)
public:
    explicit QuickEmoticons(QObject *parent = 0);
    
    QString themeName() const;
    void setThemeName(const QString &themeName);
    QStringList themeList() const;
    
signals:
    void themeListChanged(const QStringList &themeList);
    void themeNameChanged(const QString &themeName);
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKEMOTICONS_H
