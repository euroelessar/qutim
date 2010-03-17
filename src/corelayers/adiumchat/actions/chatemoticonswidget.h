#ifndef CHATEMOTICONSWIDGET_H
#define CHATEMOTICONSWIDGET_H

#include <QScrollArea>

namespace AdiumChat
{

	class ChatEmoticonsWidget : public QScrollArea
	{
		Q_OBJECT
	public:
		ChatEmoticonsWidget(QWidget *parent = 0);
	public slots:
		void loadTheme();
		void clearEmoticonsPreview();
	protected:
		void showEvent(QShowEvent *);
		void hideEvent(QHideEvent *);
		bool eventFilter(QObject *, QEvent *);
	signals:
		void insertSmile(const QString &code);
	private:
		QWidgetList m_active_emoticons;
	};

}
#endif // CHATEMOTICONSWIDGET_H

