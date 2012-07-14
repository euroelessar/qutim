#ifndef MEEGOINTEGRATION_CHATVIEWSTYLE_H
#define MEEGOINTEGRATION_CHATVIEWSTYLE_H

#include <QObject>
#include "../../../adiumwebview/lib/webkitmessageviewstyle.h"

namespace MeegoIntegration
{

class ChatViewStyle : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString stylePath READ stylePath WRITE setStylePath NOTIFY stylePathChanged)
	Q_PROPERTY(QString customStyle READ customStyle WRITE setCustomStyle NOTIFY customStyleChanged)
	Q_PROPERTY(QStringList variants READ variants NOTIFY variantsChanged)
	Q_PROPERTY(QString defaultVariant READ defaultVariant NOTIFY defaultVariantChanged)
	Q_PROPERTY(QString activeVariant READ activeVariant WRITE setActiveVariant NOTIFY activeVariantChanged)
	Q_PROPERTY(bool showUserIcons READ showUserIcons WRITE setShowUserIcons NOTIFY showUserIconsChanged)
	Q_PROPERTY(bool showHeader READ showHeader WRITE showHeader NOTIFY showHeaderChanged)
    Q_PROPERTY(QString defaultFontFamily READ defaultFontFamily NOTIFY defaultFontFamilyChanged)
    Q_PROPERTY(int defaultFontSize READ defaultFontSize NOTIFY defaultFontSizeChanged)
public:
    explicit ChatViewStyle(WebKitMessageViewStyle *style, QObject *parent = 0);
    ~ChatViewStyle();
    
    QString stylePath() const
    {
        return m_stylePath;
    }
    
    QString customStyle() const
    {
        return m_customStyle;
    }
    
    QStringList variants() const
    {
        return m_style->variants();
    }
    
    QString defaultVariant() const
    {
        return m_style->defaultVariant();
    }
    
    QString activeVariant() const
    {
        return m_style->activeVariant();
    }
    
    bool showUserIcons() const
    {
        return m_style->showUserIcons();
    }
    
    bool showHeader() const
    {
        return m_style->showHeader();
    }
    
    void setStylePath(QString stylePath)
    {
        if (m_stylePath != stylePath) {
            m_stylePath = stylePath;
            m_style->setStylePath(stylePath);
            emit stylePathChanged(stylePath);
        }
    }
    
    void setCustomStyle(QString customStyle)
    {
        if (m_customStyle != customStyle) {
            m_customStyle = customStyle;
            m_style->setCustomStyle(customStyle);
            emit customStyleChanged(customStyle);
        }
    }
    
    void setActiveVariant(QString activeVariant)
    {
        if (m_style->activeVariant() != activeVariant) {
            m_style->setActiveVariant(activeVariant);
            emit activeVariantChanged(activeVariant);
        }
    }
    
    void setShowUserIcons(bool showUserIcons)
    {
        if (m_style->showUserIcons() != showUserIcons) {
            m_style->setShowUserIcons(showUserIcons);
            emit showUserIconsChanged(showUserIcons);
        }
    }
    
    void showHeader(bool showHeader)
    {
        if (m_style->showHeader() != showHeader) {
            m_style->setShowHeader(showHeader);
            emit showHeaderChanged(showHeader);
        }
    }
    
    QString defaultFontFamily() const
    {
        return m_style->defaultFontFamily();
    }
    
    int defaultFontSize() const
    {
        return m_style->defaultFontSize();
    }
    
public slots:
    QString scriptForChangingVariant();
	QString scriptForSettingCustomStyle();
    
signals:
    void stylePathChanged(QString stylePath);
    void customStyleChanged(QString customStyle);
    void variantsChanged(QStringList variants);
    void defaultVariantChanged(QString defaultVariant);
    void activeVariantChanged(QString activeVariant);
    void showUserIconsChanged(bool showUserIcons);
    void showHeaderChanged(bool showHeader);
    void defaultFontFamilyChanged(QString defaultFontFamily);
    void defaultFontSizeChanged(int defaultFontSize);
    
private:
    WebKitMessageViewStyle *m_style;
    QString m_stylePath;
    QString m_customStyle;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_CHATVIEWSTYLE_H
