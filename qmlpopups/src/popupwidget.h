#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qmlpopups.h"
class QmlView;
namespace QmlPopups
{
	enum PopupWidgetFlag
	{
		None = 0x0,
		Preview = 0x1,
		AeroBackground = 0x2,
		Transparent = 0x4,
		AeroBlurBehind = 0x6,
	};
	Q_DECLARE_FLAGS(PopupWidgetFlags,PopupWidgetFlag)

	class PopupWidget : public QWidget
	{
		Q_OBJECT
		Q_PROPERTY(int action READ isActivated WRITE activate)
	public:
		explicit PopupWidget();
		virtual QSize sizeHint() const;
		void setData(const QString& title,const QString& body,QObject *sender);
		void activate(int action);
		int isActivated() {return 0;};//TODO
	public slots:
		void onTimeoutReached();
	signals:
		void PopupResized(const QSize &size);
		void activated();
	private slots:
		void onSceneResized(QSize size);
	private:
		QmlView *view;
		QSize m_size_hint;
		QObject *m_sender;
	};
}

#endif // WIDGET_H
