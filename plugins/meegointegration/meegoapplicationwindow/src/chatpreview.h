#ifndef MEEGOINTEGRATION_CHATPREVIEW_H
#define MEEGOINTEGRATION_CHATPREVIEW_H

#include "../../../adiumwebview/lib/webkitmessageviewcontroller.h"
#include "../../../adiumwebview/lib/webkitpreviewloader.h"
#include <QFontDatabase>

namespace MeegoIntegration {

class ChatPreview : public QObject
{
	Q_OBJECT
	Q_PROPERTY(Ureen::ChatSession *session READ session CONSTANT)
	Q_PROPERTY(QVariantList messages READ messages CONSTANT)
    Q_PROPERTY(QStringList fontFamilies READ fontFamilies CONSTANT)
    Q_PROPERTY(QVariantList fontSizes READ fontSizes CONSTANT)
public:
	ChatPreview(QObject *parent = 0);
	~ChatPreview();
	
	Ureen::ChatSession *session() const;
    QVariantList messages() const;
    QStringList fontFamilies() const;
    QVariantList fontSizes() const;
    
private:
    QFontDatabase m_database;
    WebKitPreview::Ptr m_preview;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_CHATPREVIEW_H
