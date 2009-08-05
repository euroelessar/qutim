#ifndef Console_H
#define Console_H

#include <QCloseEvent>
#include "ui_console.h"

class Console : public QWidget
{
    Q_OBJECT
    
private:
	Ui::Console ui;
public:
	Console(QWidget *parent=0);
	void appendMsg(const QString &xml, QtMsgType type);
protected:
	virtual void closeEvent(QCloseEvent *event);
};

#endif
