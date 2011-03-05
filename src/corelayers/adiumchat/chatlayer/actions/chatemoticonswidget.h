#ifndef CHATEMOTICONSWIDGET_H
#define CHATEMOTICONSWIDGET_H

#include <QScrollArea>
#include <QAction>
#include <QPointer>

namespace Core
{
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

class EmoAction : public QAction
{
	Q_OBJECT
public:
	EmoAction(QObject *parent = 0);
signals:
	void insertSmile(const QString &code);
private:
	QPointer<ChatEmoticonsWidget> m_emoticons_widget;
private slots:
	void onInsertSmile(const QString &code);
	void triggerEmoticons();
};

}
}
#endif // CHATEMOTICONSWIDGET_H

