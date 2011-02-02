 #ifndef QFINGERSWIPEGESTURE_H
 #define QFINGERSWIPEGESTURE_H

 #include <QGesture>
 #include <QGestureRecognizer>
 #include <QPointF>

 class FingerSwipeGestureRecognizer : public QGestureRecognizer
 {
 public:
	 FingerSwipeGestureRecognizer();

	 QGesture *create(QObject *target);
	 QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
	 void reset(QGesture *state);
 };

 class FingerSwipeGesture : public QGesture
 {
 public:
	 FingerSwipeGesture(QObject *parent = 0);
	 virtual ~FingerSwipeGesture();

	 bool isLeftToRight() const;
	 bool isRightToLeft() const;

 private:
	 QPointF m_startPos;
	 QPointF m_lastPos;
	 QPointF m_currentPos;
	 bool m_triggered;
	 bool m_cancelled;

	 friend class FingerSwipeGestureRecognizer;
 };

 #endif
