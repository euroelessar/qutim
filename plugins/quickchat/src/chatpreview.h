#ifndef MEEGOINTEGRATION_CHATPREVIEW_H
#define MEEGOINTEGRATION_CHATPREVIEW_H

#include <qutim/chatsession.h>
#include <QFontDatabase>

namespace QuickChat {

class ChatPreview : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qutim_sdk_0_3::ChatSession *session READ session CONSTANT)
	Q_PROPERTY(QVariantList messages READ messages CONSTANT)
    Q_PROPERTY(QStringList fontFamilies READ fontFamilies CONSTANT)
    Q_PROPERTY(QVariantList fontSizes READ fontSizes CONSTANT)
public:
	ChatPreview(QObject *parent = 0);
	~ChatPreview();
	
	qutim_sdk_0_3::ChatSession *session() const;
    QVariantList messages() const;
    QStringList fontFamilies() const;
    QVariantList fontSizes() const;
    
private:
    QFontDatabase m_database;
//    WebKitPreview::Ptr m_preview;
};

} // namespace QuickChat

#endif // MEEGOINTEGRATION_CHATPREVIEW_H
