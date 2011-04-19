#include "framebutton.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFile>

class FrameButtonPrivate
{
public:
	//QGraphicsDropShadowEffect effect;
};

FrameButton::FrameButton(QWidget *parent) :
	QPushButton(parent), d_ptr(new FrameButtonPrivate)
{
	Q_D(FrameButton);
	//d->effect.setColor(QColor(255, 50, 50));
	//d->effect.setBlurRadius(22);
	//d->effect.setOffset(1);
	//d->effect.setEnabled(false);
	//setGraphicsEffect(&d_func()->effect);
	setAutoFillBackground(false);

	QFile file(":/sevenlist/pushbutton.css");
	if (file.open(QIODevice::ReadOnly))
		setStyleSheet(file.readAll());

	setMaximumHeight(22);
	setMinimumSize(50, 22);

	//setStyleSheet(("QPushButton { "
	//			   "border: 3px;"
	//			   "border-image: url(:/sevenlist/redbutton.png);"
	//			   "}"));

	//setIconSize(QSize(12,12));
}

FrameButton::~FrameButton()
{

}

void FrameButton::enterEvent(QEvent *e)
{	
	QPushButton::enterEvent(e);
	//d_func()->effect.setEnabled(true);
}

void FrameButton::leaveEvent(QEvent *e)
{	
	QPushButton::leaveEvent(e);
	//d_func()->effect.setEnabled(false);
}


