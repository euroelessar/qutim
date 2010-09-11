#ifndef WIDGET_H
#define WIDGET_H

#include "backend.h"
#include <QVariant>
class QDeclarativeView;
namespace Core
{
namespace KineticPopups
{
enum PopupWidgetFlag
{
	None = 0x0,
	Preview = 0x1,
	AeroBackground = 0x2,
	Transparent = 0x4,
	AeroBlurBehind = 0x8
};
Q_DECLARE_FLAGS(PopupWidgetFlags,PopupWidgetFlag)

class PopupWidget : public AbstractPopupWidget
{
	Q_OBJECT
public:
	explicit PopupWidget();
	PopupWidget(const QString &themeName); //for generate preview only
	virtual QSize sizeHint() const;
	void setTheme(const QString &themeName);
	void setData(const QString& title,
				 const QString& body,
				 QObject *sender,
				 const QVariant &data); //size of textbrowser
	Q_INVOKABLE QString parseEmoticons(const QString &text); //wrapper for qml
public slots:
	void accept();
	void ignore();
signals:
	void sizeChanged(const QSize &size);
	void activated();
private slots:
	void onSceneResized(QSize size);
private:
	QSize m_size_hint;
	QObject *m_sender;
	QVariant m_data;
	QDeclarativeView *m_view;
};
}
}
#endif // WIDGET_H
