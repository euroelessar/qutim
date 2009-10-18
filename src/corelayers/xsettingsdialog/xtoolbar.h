#ifndef XTOOLBAR_H
#define XTOOLBAR_H

#include <QToolBar>

class XToolBar : public QToolBar
{
	Q_OBJECT
public:
    XToolBar ( QWidget* parent = 0 );
private slots:
	void onActionTriggered(QAction *action);
};

#endif // XTOOLBAR_H
