#ifndef XTOOLBAR_H
#define XTOOLBAR_H

#include <QToolBar>

class QMenu;
class XToolBar : public QToolBar
{
	Q_OBJECT
public:
    XToolBar ( QWidget* parent = 0 );
protected:
	virtual void contextMenuEvent(QContextMenuEvent* );
private:
	QMenu *m_context_menu;
private slots:
	void onActionTriggered(QAction *action);
	void onAnimationActionTriggered(bool checked);
	int getSizePos (const int &size);
	void onSizeActionTriggered(QAction *action);
	void onToolButtonStyleActionTriggered(QAction *action);
};

#endif // XTOOLBAR_H
