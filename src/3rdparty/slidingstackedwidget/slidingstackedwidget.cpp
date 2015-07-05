/****************************************************************************
**
** Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
**
** $QT_BEGIN_LICENSE:LGPL$
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "slidingstackedwidget.h"
#include "fingerswipegesture.h"

SlidingStackedWidget::SlidingStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
{
    if (parent!=0) {
		m_mainwindow = parent;
	}
	else {
		m_mainwindow = this;
		qDebug()<<"ATTENTION: untested mainwindow case !";
	}
	//parent should not be 0; not tested for any other case yet !!
#ifdef Q_OS_SYMBIAN
#ifndef __S60_50__
	qDebug()<< "WARNING: ONLY TESTED AND 5TH EDITION";
#endif //__S60_50__
#endif //Q_OS_SYMBIAN
	//Now, initialize some private variables with default values
	m_vertical=false;
	//setVerticalMode(true);
	m_speed=500;
	m_animationtype = QEasingCurve::OutBack;  //check out the QEasingCurve documentation for different styles
	m_now=0;
	m_next=0;
	m_wrap=false;
	m_pnow=QPoint(0,0);
	m_active=false;

	fingerSwipeGestureType = (Qt::GestureType)0;
	fingerSwipeGestureType = QGestureRecognizer::registerRecognizer( new FingerSwipeGestureRecognizer() );
	grabGesture(fingerSwipeGestureType);
}


SlidingStackedWidget::~SlidingStackedWidget(){
}

void SlidingStackedWidget::setVerticalMode(bool vertical) {
	m_vertical=vertical;
}

void SlidingStackedWidget::setSpeed(int speed) {
	m_speed = speed;
}

void SlidingStackedWidget::setAnimation(enum QEasingCurve::Type animationtype) {
	m_animationtype = animationtype;
}

void SlidingStackedWidget::setWrap(bool wrap) {
	m_wrap=wrap;
}

void SlidingStackedWidget::slideInNext() {
	int now=currentIndex();
	if (m_wrap||(now<count()-1)) {
		// count is inherit from QStackedWidget
		slideInIdx(now+1);
	}
}


void SlidingStackedWidget::slideInPrev() {
	int now=currentIndex();
	if (m_wrap||(now>0)) {
		slideInIdx(now-1);
	}
}

void SlidingStackedWidget::slideInIdx(int idx, SlideDirection direction) {
	//int idx, t_direction direction=Automatic

	if (idx>count()-1) {
		direction=m_vertical ? TopToBottom : RightToLeft;
		idx = (idx) % count();
	}
	else if (idx<0) {
		direction= m_vertical ? BottomToTop: LeftToRight;
		idx = (idx+count()) % count();
	}

	slideInWgt(widget(idx),direction);
}


void SlidingStackedWidget::slideInWgt(QWidget * newwidget,SlideDirection  direction) {

	if (m_active) {
		return; // at the moment, do not allow re-entrance before an animation is completed.
		//other possibility may be to finish the previous animation abrupt, or
		//to revert the previous animation with a counter animation, before going ahead
		//or to revert the previous animation abrupt
		//and all those only, if the newwidget is not the same as that of the previous running animation.
	}
	else m_active=true;

	enum SlideDirection directionhint;
	int now=currentIndex();		//currentIndex() is a function inherited from QStackedWidget
	int next=indexOf(newwidget);
	if (now==next) {
		m_active=false;
		return;
	}
	else if (now<next){
		directionhint=m_vertical ? TopToBottom : RightToLeft;
	}
	else {
		directionhint=m_vertical ? BottomToTop : LeftToRight;
	}
	if (direction == Automatic) {
		direction=directionhint;
	}
	//NOW....
	//calculate the shifts

	int offsetx=frameRect().width(); //inherited from mother
	int offsety=frameRect().height();//inherited from mother

	//the following is important, to ensure that the new widget
	//has correct geometry information when sliding in first time
	widget(next)->setGeometry ( 0,  0, offsetx, offsety );

	if (direction == BottomToTop)  {
		offsetx=0;
		offsety=-offsety;
	}
	else if (direction == TopToBottom) {
		offsetx=0;
		//offsety=offsety;
	}
	else if (direction == RightToLeft) {
		offsetx=-offsetx;
		offsety=0;
	}
	else if (direction == LeftToRight) {
		//offsetx=offsetx;
		offsety=0;
	}
	//re-position the next widget outside/aside of the display area
	QPoint pnext=widget(next)->pos();
	QPoint pnow=widget(now)->pos();
	m_pnow=pnow;

	widget(next)->move(pnext.x()-offsetx,pnext.y()-offsety);
	//make it visible/show
	widget(next)->show();
	widget(next)->raise();

	//animate both, the now and next widget to the side, using animation framework
	QPropertyAnimation *animnow = new QPropertyAnimation(widget(now), "pos");

	animnow->setDuration(m_speed);
	animnow->setEasingCurve(m_animationtype);
	animnow->setStartValue(QPoint(pnow.x(), pnow.y()));
	animnow->setEndValue(QPoint(offsetx+pnow.x(), offsety+pnow.y()));
	QPropertyAnimation *animnext = new QPropertyAnimation(widget(next), "pos");
	animnext->setDuration(m_speed);
	animnext->setEasingCurve(m_animationtype);
	animnext->setStartValue(QPoint(-offsetx+pnext.x(), offsety+pnext.y()));
	animnext->setEndValue(QPoint(pnext.x(), pnext.y()));

	QParallelAnimationGroup *animgroup = new QParallelAnimationGroup;

	animgroup->addAnimation(animnow);
	animgroup->addAnimation(animnext);

	QObject::connect(animgroup, SIGNAL(finished()),this,SLOT(animationDoneSlot()));
	m_next=next;
	m_now=now;
	m_active=true;
	animgroup->start();

	//note; the rest is done via a connect from the animation ready;
	//animation->finished() provides a signal when animation is done;
	//so we connect this to some post processing slot,
	//that we implement here below in animationDoneSlot.
}


void SlidingStackedWidget::animationDoneSlot(void) {
	//when ready, call the QStackedWidget slot setCurrentIndex(int)
	setCurrentIndex(m_next);  //this function is inherit from QStackedWidget
	//then hide the outshifted widget now, and  (may be done already implicitely by QStackedWidget)
	widget(m_now)->hide();
	//then set the position of the outshifted widget now back to its original
	widget(m_now)->move(m_pnow);
	//so that the application could also still call the QStackedWidget original functions/slots for changings
	//widget(m_now)->update();
	//setCurrentIndex(m_next);  //this function is inherit from QStackedWidget
	m_active=false;
	emit animationFinished();
}

bool SlidingStackedWidget::event(QEvent *event)
{
	if (event->type() == QEvent::TouchBegin) {
		event->accept();
		return true;
	}

	if (event->type() == QEvent::Gesture) {
		QGestureEvent *ge = static_cast<QGestureEvent*>(event);

		if (QGesture *gesture = ge->gesture(fingerSwipeGestureType)) {
			FingerSwipeGesture *swipe = static_cast<FingerSwipeGesture*>(gesture);
			if (swipe->state() == Qt::GestureFinished) {
				if (swipe->isLeftToRight()) {
					emit fingerGesture(LeftToRight);
				}
				else if (swipe->isRightToLeft()) {
					emit fingerGesture(RightToLeft);
				}
				else if (swipe->isBottomToTop()) {
					emit fingerGesture(BottomToTop);
				}
				else if (swipe->isTopToBottom()) {
					emit fingerGesture(TopToBottom);
				}
			}

			ge->setAccepted(gesture, true);
			return true;
		}
	}

	//for debug
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->modifiers() == Qt::ControlModifier) {
			if (keyEvent->key() == Qt::Key_Left) {
				emit fingerGesture(LeftToRight);
			} else if (keyEvent->key() == Qt::Key_Right)
				emit fingerGesture(RightToLeft);
		}
	}

	return QStackedWidget::event(event);
}

/* REFERENCES

http://doc.trolltech.com/4.6/animation-overview.html#easing-curves
http://doc.trolltech.com/4.6/qpropertyanimation.html
http://doc.trolltech.com/4.6/qanimationgroup.html

*/




