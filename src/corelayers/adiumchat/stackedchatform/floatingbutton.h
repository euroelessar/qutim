#ifndef FLOATINGBUTTON_H
#define FLOATINGBUTTON_H

#include <QtGui/qtoolbutton.h>
#include <QtGui/qevent.h>
#include <qutim/icon.h>

namespace Core
{
    namespace AdiumChat
    {
	class FloatingButton : public QToolButton
	{
	    Q_OBJECT
	public:
	    inline explicit FloatingButton(int type,QWidget *parent);
	    
	protected:
	    inline bool eventFilter(QObject *obj, QEvent *ev);
	    int m_type;
	};
	
	FloatingButton::FloatingButton(int type,QWidget *parent)
	    : QToolButton(parent)
	{
	    Q_ASSERT(parent);
	    
	    if (type==0) {
#ifdef Q_WS_MAEMO_5
		// set the next icon from Maemo's theme
		setIcon(qutim_sdk_0_3::Icon("general_forward"));
#else
		setIcon(qutim_sdk_0_3::Icon("arrow-right"));
#endif
	    }
	    else if (type==1) {
#ifdef Q_WS_MAEMO_5
		// set the close icon from Maemo's theme
		setIcon(qutim_sdk_0_3::Icon("camera_overlay_close"));
#else
		setIcon(qutim_sdk_0_3::Icon("dialog-close"));
#endif
	    }
	    
	    m_type=type;

	    setIconSize(QSize(48,48));
	    
	    // ensure that our size is fixed to our ideal size
	    setFixedSize(sizeHint());
	    
	    // set the background to 0.5 alpha
	    QPalette pal = palette();
	    QColor backgroundColor = pal.color(backgroundRole());
	    backgroundColor.setAlpha(32);
	    pal.setColor(backgroundRole(), backgroundColor);
	    setPalette(pal);
	    
	    // ensure that we're painting our background
	    setAutoFillBackground(true);
	    
	    // install an event filter to listen for the parent's events
	    parent->installEventFilter(this);
	}
	
	bool FloatingButton::eventFilter(QObject *obj, QEvent *ev)
	{
	    if (obj != parent())
		return QToolButton::eventFilter(obj, ev);
	    
	    QWidget *parent = parentWidget();
	    
	    switch (ev->type()) {
	    case QEvent::Resize:
		if (m_type==0) {
		    move(parent->width() - width(),
			 parent->height()/2 - height());
		} else if (m_type==1){
		    move(parent->width() - width(),0);
		}
		break;
	default:
		break;
	    }
	    
	    return QToolButton::eventFilter(obj, ev);
	}
    }
}
#endif
