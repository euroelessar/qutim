#ifndef FRAMEBUTTON_H
#define FRAMEBUTTON_H

#include <QPushButton>

class FrameButtonPrivate;
class FrameButton : public QPushButton
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(FrameButton)
public:
	explicit FrameButton(QWidget *parent = 0);
	virtual ~FrameButton();
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
private:
	QScopedPointer<FrameButtonPrivate> d_ptr;
};

#endif // FRAMEBUTTON_H
