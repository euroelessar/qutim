#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qmlpopups.h"
class QmlView;
namespace QmlPopups
{
	class PopupWidget : public QWidget
	{
		Q_OBJECT
	public:
		explicit PopupWidget();
		virtual QSize sizeHint() const;
		void setData(const QString& title,const QString& body,QObject *sender);
	public slots:
		void onTimeoutReached();
	signals:
		void PopupResized(const QSize &size);
		void Activated();
	private slots:
		void onSceneResized(QSize size);
	private:
		QmlView *view;
		QSize m_size_hint;
	};
}

#endif // WIDGET_H
