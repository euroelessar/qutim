/********************************************************************************************************
* PROGRAM : QSkinObject
* DATE - TIME : Samstag 30 Dezember 2006 - 12h04
* AUTHOR : Markus Künkle( http://amobos.org )
* FILENAME : QSkinObject.cpp
* LICENSE : GPL
* COMMENTARY : QObject subclass for theming Qt\'s QWidget.
********************************************************************************************************/
#include <QtGui>
#include <cmath>

#include "qskinobject.h"



QSkinObject::QSkinObject( QWidget* wgtParent)
    : QObject()
   
{
	// Widget which will be skinned
	m_background_color = wgtParent->palette().color(QPalette::Window);
	skinWidget = wgtParent;
	contentsRect = skinWidget->rect();
	// Save flags for later unsset the skin
	flags = skinWidget->windowFlags();
#ifdef WIN32
	wlong = GetWindowLong(skinWidget->winId(), GWL_EXSTYLE);
#endif
	// do some initialisation
	skinTimer = new QBasicTimer();
	widgetMask = QPixmap(skinWidget->width()+12, skinWidget->height()+12);
	mousePress = false;
	m_mask_setted = false;
	m_up_layout = true;
	m_margin_down = -1;
	m_margin_left = -1;
	m_margin_up = -1;
	m_margin_right = -1;
	m_button_hovered = false;

	
}

void QSkinObject::startSkinning()
{
	
//	loadSkinIni();
	QPoint pos = skinWidget->pos();
//	skinWidget->setAttribute(Qt::WA_PaintOnScreen);
	skinWidget->setAttribute(Qt::WA_NoSystemBackground);
	//no windows frame
	skinWidget->setWindowFlags(Qt::FramelessWindowHint);
	//loadSkinIni();
	
	
	//mouse tracking on, because the close-button should glow if the cursor comes over it
	skinWidget->setMouseTracking(true);
	
	skinWidget->setContentsMargins ( QPixmap(skinPath + "left.png").width(), 
			QPixmap(skinPath + "up.png").height(),
			QPixmap(skinPath + "right.png").width(), 
			QPixmap(skinPath + "down.png").height());
	
	skinWidget->installEventFilter(this);
	resizeHandler =  new QSkinWidgetResizeHandler(this, skinWidget);
#ifdef WIN32
	alpha = 255;
	setLayered();
#endif

//	skinTimer->start(100, this);
//	skinWidget->show();
	skinWidget->move(pos);
}

void QSkinObject::stopSkinning()
{
	skinWidget->setContentsMargins ( 5, 5, 5, 5);
	skinWidget->clearMask();
	skinWidget->setAttribute(Qt::WA_PaintOnScreen, false);
	skinWidget->setAttribute(Qt::WA_NoSystemBackground, false);
#ifdef WIN32
	SetWindowLong(skinWidget->winId(), 
              GWL_EXSTYLE, 
              wlong);
#endif
	skinWidget->setWindowFlags(flags);
	skinWidget->removeEventFilter(this);
	skinWidget->removeEventFilter(resizeHandler);
	skinTimer->stop();
	skinWidget->setMouseTracking(false);
	skinWidget->show();
}
bool QSkinObject::eventFilter(QObject *o, QEvent *e)
{
//	if(e->type() == QEvent::MouseButtonPress)
//	{
////		skinWidget->repaint();
////		qDebug()<<"press";
//		return false;
//	}
	 if(e->type() == QEvent::Resize)
	{
		resizeEvent(static_cast<QResizeEvent*>(e));
		return false;
	}
	else if(e->type() == QEvent::Paint)
	{
		paintEvent(static_cast<QPaintEvent*>(e));
		return false;
	}
//	else if(e->type() == QEvent::MouseButtonRelease)
//	{
////		skinWidget->repaint();
//		qDebug()<<"release";
////		mousePressEvent(static_cast<QMouseEvent*>(e));
//		return false;
//	}
	else if(e->type() == QEvent::HoverMove || e->type() == QEvent::CursorChange  )
	{
//		updateButtons();
		if ( quitButton.contains(skinWidget->mapFromGlobal(QCursor::pos())) || 
				maxButton.contains(skinWidget->mapFromGlobal(QCursor::pos())) || 
				minButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
		{
			skinWidget->repaint();
		}
		else
			m_button_hovered = true;
		if ( m_button_hovered )
		{
			skinWidget->repaint();
			m_button_hovered = false;
		}
		return false;
	}
	else
	{
		 //resizeHandler->eventFilter(o, e);
		 return skinWidget->eventFilter(o, e);
	}
}

void QSkinObject::manageRegions()
{
	const int quiWidth = QPixmap(skinPath + "close.png").width();
	const int maxWidth = QPixmap(skinPath + "max.png").width();
	const int minWidth = QPixmap(skinPath + "minimise.png").width();
	const int quiHeight = QPixmap(skinPath + "close.png").height();
	const int maxHeight = QPixmap(skinPath + "max.png").height();
	const int minHeight = QPixmap(skinPath + "minimise.png").height();
	const int rightWidth = QPixmap(skinPath + "right").width();
	if ( m_up_layout )
	{
	quitButton 			= QRegion( QRect(skinWidget->width()-quiWidth - m_margin_right, m_margin_up, quiWidth, quiHeight) );
//	maxButton 			= QRegion( QRect(skinWidget->width()-8-quiWidth-maxWidth - rightWidth, 2, maxWidth, maxHeight) );
	minButton 			= QRegion( QRect(skinWidget->width()-quiWidth-minWidth - m_margin_right, m_margin_up, minWidth, minHeight) );
	}
	else
	{
		quitButton 			= QRegion( QRect(skinWidget->width()-quiWidth - m_margin_right, skinWidget->height() - m_margin_down - quiHeight, quiWidth, quiHeight) );
	//	maxButton 			= QRegion( QRect(skinWidget->width()-8-quiWidth-maxWidth - rightWidth, 2, maxWidth, maxHeight) );
		minButton 			= QRegion( QRect(skinWidget->width()-quiWidth-minWidth - m_margin_right, skinWidget->height() - m_margin_down - minHeight, minWidth, minHeight) );
	}
		
}

/*void QSkinObject::closeEvent(QCloseEvent *e)
{

}*/

void QSkinObject::resizeEvent(QResizeEvent *e)
{
	manageRegions();
	m_mask_setted = false;
	e->ignore();
	//skinWidget->resizeEvent(e);
}

void QSkinObject::mousePressEvent(QMouseEvent *e)
 {
	// winwow Mover
	QPoint pos = e->pos();

	//QRect rect = skinWidget->rect();
    if (e->button() == Qt::LeftButton) 
    {

//	mousePress = true;
////	updateButtons();
//	skinWidget->repaint();
//	mousePress = false;
		// buttons
		int button = -1;
		if(quitButton.contains(e->pos()))
		 button = 0 ;
		else if(maxButton.contains(e->pos()))
		 button = 1 ;
		else if(minButton.contains(e->pos()))
		 button = 2 ;
//		if(pos.x() < skinWidget->width() - 5)
//			if(pos.y() < QPixmap(skinPath + "so.png").height() + butPos && pos.y() > 6)
//			{
				switch (button) {
					case 0:
					skinWidget->hide();
					 e->accept();
					 break;
					case 1:
					 if(skinWidget->isMaximized())
						skinWidget->showNormal();
					 else
						skinWidget->showMaximized();
					 e->accept();
					 break;
					case 2: 
					 skinWidget->showMinimized();
					 e->accept();
					 break;
				}
//			}
//			else
//				e->ignore();
		
    }
    else
	mousePress = false;

 }
/*void QSkinObject::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	
}

void QSkinObject::mouseMoveEvent(QMouseEvent *e)
{
 qDebug()<<"MOUSE MOVE";	
}
*/
void QSkinObject::setSkinPath(const QString & skinpath)
{
	skinPath = skinpath;
	skinWidget->update();
	
}
QString QSkinObject::getSkinPath()
{
	return skinPath;
}
void QSkinObject::updateButtons()
{
	QPainter p(&widgetMask);
	p.setRenderHint(QPainter::Antialiasing);
	QPixmap min;
	QPixmap max;
	QPixmap qui;
	if(minButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
	{
		if(!mousePress)
		{
			min = QPixmap(skinPath + "minimise_hover.png");
		}
		else
		{
			min = QPixmap(skinPath + "minimise_click.png");
		}
	}
	else
	{
		min = QPixmap(skinPath + "minimise.png");
	}

	if(quitButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
	{
		if(!mousePress)
			qui = QPixmap(skinPath + "close_hover.png");
		else
			qui = QPixmap(skinPath + "close_click.png");
	}
	else
	{
	 qui = QPixmap(skinPath + "close.png");
	}

//	if(maxButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
//	{
//		if(!mousePress)
//			max = QPixmap(skinPath + "maxButton1.png");
//		else
//			max = QPixmap(skinPath + "maxButton2.png");
//	}
//	else
//	{
//	 max = QPixmap(skinPath + "maxButton.png");
//	}
//	p.drawPixmap(QRect(skinWidget->width() - qui.width() - sr.width() -8, 2, qui.width(), qui.height()),
//	qui,
//	QRect(0,0, qui.width(), qui.height()));
//
//	p.drawPixmap(QRect(skinWidget->width() - qui.width() - max.width()-8 - sr.width(), 2, max.width(), max.height()),
//	max,
//	QRect(0,0, max.width(), max.height()));
//
//	p.drawPixmap(QRect(skinWidget->width() - qui.width()-max.width() - min.width()-8 - sr.width(), 2, min.width(), min.height()),
//	min,
//	QRect(0,0, min.width(), min.height()));
	const int quiWidth = qui.width();
	const int minWidth = min.width();
	const int quiHeight = qui.height();
	const int minHeight = min.height();
	
	if ( m_up_layout )
	{
//			p.drawPixmap(QRect(skinWidget->width() - qui.width() - sr.width() -8, 2, qui.width(), qui.height()),
//			qui,
//			QRect(0,0, qui.width(), qui.height()));
//
//			p.drawPixmap(QRect(skinWidget->width() - qui.width()-max.width() - min.width()-8 - sr.width(), 2, min.width(), min.height()),
//			min,
//			QRect(0,0, min.width(), min.height()));
//		p.drawPixmap(quitButton, qui);
//		p.drawPixmap(minButton, min);
//		qDebug()<<quitButton.
		p.drawPixmap(QRect(skinWidget->width()-quiWidth - m_margin_right, m_margin_up, quiWidth, quiHeight),
				qui);

		p.drawPixmap(QRect(skinWidget->width()-quiWidth-minWidth - m_margin_right, m_margin_up, minWidth, minHeight),
				min,QRect(0,0, min.width(), min.height()));
	}
	else
	{
		p.drawPixmap(QRect(skinWidget->width()-quiWidth - m_margin_right, skinWidget->height() - m_margin_down - quiHeight, quiWidth, quiHeight), qui);
		p.drawPixmap(QRect(skinWidget->width()-quiWidth-minWidth - m_margin_right, skinWidget->height() - m_margin_down - minHeight, minWidth, minHeight),
				min);
	}
	p.end();

}
void QSkinObject::updateStyle()
{
	
	//this makes the alpha blending work. just gdi funktions and a conversation from QPixmap to HBITMAP. not really interresting
	QPixmap up_pix (skinPath + "up.png");
	QPixmap left_pix (skinPath + "left.png");
	QPixmap right_pix (skinPath + "right.png");
	QPixmap down_pix (skinPath + "down.png");
	QPixmap up_left_pix (skinPath + "up_left.png");
	QPixmap up_right_pix (skinPath + "up_right.png");
	QPixmap down_left_pix (skinPath + "down_left.png");
	QPixmap down_right_pix (skinPath + "down_right.png");
//	QPixmap qui (skinPath + "quiButton.png");
	
	//shirina png s knopkami, vysota png
	skinWidget->setMinimumSize (right_pix.width() + left_pix.width() + 128, 
			up_pix.height() + down_pix.height() + 64);
	
	widgetMask = QPixmap(skinWidget->width(), skinWidget->height());
	widgetMask.fill(Qt::transparent);
	//widgetMask.fill(QColor(255,255,0));

	/*#ifdef WIN32
		if(milchglas)
		{
			QImage tmp = QPixmap::grabWindow(QApplication::desktop()->winId(), 
						 skinWidget->geometry().x(), skinWidget->geometry().y(), skinWidget->rect().width(), skinWidget->rect().height()).toImage();
			fastbluralpha(tmp, 4);
			widgetMask = QPixmap::fromImage(tmp);
		}
	#endif*/
	QPainter p(&widgetMask);
	
	int lo = skinWidget->width()-(up_pix.width()-68);
	if(lo < 68)
		lo = 68;
//	p.drawPixmap(QRect(0,0,skinWidget->width(), so.height()), so, QRect(0,0,skinWidget->width(), so.height()));
//	p.drawPixmap(QRect(skinWidget->width()-64, 0,64, so.height()), so, QRect(so.width()-64, 0 , 64, so.height()));
//	p.drawPixmap(QRect(0, 0, 64, so.height()), so, QRect(0, 0 , 64, so.height()));
//	p.drawPixmap(QRect(64, 0, skinWidget->width()-128, so.height()), so, QRect(64, 0, so.width()-128, so.height()));
	
	
	//draw up border
	p.drawPixmap(QRect(0,0,up_left_pix.width(), up_left_pix.height()), up_left_pix,
			QRect(0,0,up_left_pix.width(), up_left_pix.height()));
	p.drawPixmap(QRect(up_left_pix.width(), 0, skinWidget->width() - up_left_pix.width()
			 - up_right_pix.width(), up_pix.height()), up_pix);
	p.drawPixmap(QRect(skinWidget->width() - up_right_pix.width(),0,
			up_right_pix.width(), up_right_pix.height()), up_right_pix);
	
	//draw down border
	p.drawPixmap(QRect(0,skinWidget->height() - down_left_pix.height(),
			down_left_pix.width(), down_left_pix.height()), down_left_pix);
	p.drawPixmap(QRect(down_left_pix.width(),skinWidget->height() - down_left_pix.height(),
			skinWidget->width() - down_left_pix.width() - down_right_pix.width(),
			down_pix.height()), down_pix);
	p.drawPixmap(QRect(skinWidget->width() - down_right_pix.width(),
			skinWidget->height() - down_right_pix.height(),
			down_right_pix.width(), down_right_pix.height()), down_right_pix);
	
	//draw left border
	p.drawPixmap(QRect(0, up_left_pix.height(), left_pix.width(), 
			skinWidget->height()-up_left_pix.height()-down_left_pix.height()), left_pix);
	
	//draw right border
	p.drawPixmap(QRect(skinWidget->width() - right_pix.width(), 
			up_right_pix.height(), right_pix.width(), 
			skinWidget->height()-up_right_pix.height()-down_right_pix.height()), right_pix);
	
//	//p.drawPixmap(QRect(sl.width(), height()-su.height(), width(), su.height()), su);
//	
//	p.drawPixmap(QRect(sl.width(), skinWidget->height() - su.height(), skinWidget->width()-sl.width()-sr.width(), su.height()), 
//				su, 
//				QRect(sl.width(), 0, su.width()-sl.width()-sr.width(), su.height()));
//	p.drawPixmap(QRect(0, skinWidget->height() - su.height(), sl.width(), su.height()),
//				su, 
//				QRect(0, 0 , sl.width(), su.height()));
//				
//	
//	p.drawPixmap(QRect(skinWidget->width()-sr.width(), skinWidget->height() - su.height(), sr.width(), su.height()), 
//				su, 
//				QRect(su.width()-sr.width(), 0 , sr.width(), su.height()));			
//	
//	
//	
//	
//	
//	p.drawPixmap(QRect(skinWidget->width()-sr.width(), so.height(), sr.width(), skinWidget->height()-so.height()-su.height()), sr);
//	
//	
//	p.drawPixmap(QRect(0, so.height(), sl.width(), skinWidget->height()-su.height()-so.height()), sl);
//	
//	p.setFont(titleFont);
//	p.setPen(QPen(titleColor, 2));
	
	contentsRect = QRect(left_pix.width(), up_pix.height(), skinWidget->width() - right_pix.width() - 
			left_pix.width(), skinWidget->height() - up_pix.height() - down_pix.height());
	p.setBrush(m_background_color);
	//	p.setBrush(QColor(255,255,0));
	p.setPen(Qt::transparent);
	p.drawRect(contentsRect);
//	p.drawText(QRect(left_pix.width(), 0, skinWidget->width()-qui.width()*3-8-sl.width()-sr.width(), so.height()), Qt::AlignVCenter, skinWidget->windowTitle());
//	p.drawText(QRect(left_pix.width(), 0, skinWidget->width()-left_pix.width()-right_pix.width(), up_pix.height()), Qt::AlignVCenter, skinWidget->windowTitle());
//	QPixmap close_button = QPixmap(skinPath + "contact_list_close.png");
//	qDebug()<<skinPath + "contact_list_close.png";
//	p.drawPixmap(QRect(100,100,100, 100), close_button);
//	p.setBrush(Qt::yellow);
//	p.drawRect(QRect(100,100,300,100));

	p.end();
	//skinWidget->setMask(widgetMask.mask());
	
	updateButtons();
}

void QSkinObject::loadSkinIni(const QString &path_to_border)
{
//	QSettings s("/home/osiris/testingqutim/qskinobject-0.6.1/qskinobject-0.6.1/bin/skin/skin.dat", QSettings::defaultFormat());
//	s.beginGroup("Skin");
//	milchglas = s.value("Blur", false).toBool();	
//	backgroundColor = QColor(s.value("Hintergrundfarbe", Qt::white).toString());
//	titleColor = QColor(s.value("Titelfarbe", QColor(255,100,0)).toString());
//	QFont font;
//	font.fromString(s.value("Titelschrift", QFont("Comic Sans MS")).toString());
//	skinPath = "/home/osiris/testingqutim/qskinobject-0.6.1/qskinobject-0.6.1/bin/" + s.value("Skinpfad", "/home/osiris/testingqutim/qskinobject-0.6.1/qskinobject-0.6.1/bin/skin/blau/").toString();
//	titleFont = font;
//	s.endGroup();
	QSettings settings(path_to_border + "config.ini",QSettings::defaultFormat());
	settings.beginGroup("buttons");
	m_up_layout = settings.value("up", true).toBool();
	m_margin_down = settings.value("mdown", -1).toInt();
	m_margin_up = settings.value("mup", -1).toInt();
	m_margin_left = settings.value("mleft", -1).toInt();
	m_margin_right = settings.value("mright", -1).toInt();
	settings.endGroup();
	settings.beginGroup("window");
	m_background_color.setNamedColor(settings.value("backcolor", m_background_color).toString());
	settings.endGroup();
	milchglas = false;
	skinPath = path_to_border;
}
void QSkinObject::paintEvent(QPaintEvent *event)
{
	if(skinWidget)
	{
		
		updateStyle();
		QPainter p(&widgetMask);
//
//		QObjectList ol = skinWidget->children();
//		foreach(QObject *o, ol)
//		{
//			if(o->isWidgetType() && o->metaObject()->className() != QMenu::staticMetaObject.className())
//			{
//				
//				QWidget* child = dynamic_cast<QWidget*>(o);  
//				
//				p.drawPixmap(child->geometry(),drawCtrl(child ));
//				
//				
//			}
//				
//		}
		p.end();
		if ( !m_mask_setted )
		{
			skinWidget->setMask(widgetMask.mask());
		    m_mask_setted = true;
		}
/*#ifdef WIN32
		updateAlpha(); 

#else*/
		
		p.begin(skinWidget);
		p.setRenderHint(QPainter::Antialiasing);
		p.drawPixmap(0,0,widgetMask);
		p.end();
//#endif
		
	}

}

QPixmap QSkinObject::drawCtrl(QWidget * widget)
{

	if (!widget)
		return QPixmap();
	QRect r(widget->rect());
	QPixmap res(r.size());
	widget->render(&res, -r.topLeft(), r, QWidget::DrawWindowBackground | QWidget::DrawChildren | QWidget::IgnoreMask);
	return res;

} 

void QSkinObject::timerEvent ( QTimerEvent * event )
{
	if(event->timerId() == skinTimer->timerId())
		skinWidget->repaint();
}

int QSkinObject::customFrameWidth()
{
	return QPixmap(skinPath + "left.png").width();
}

#ifdef WIN32
void QSkinObject::updateAlpha()
{
	
	//this makes the alpha blending work. just gdi funktions and a conversation from QPixmap to HBITMAP. not really interresting
	
		
	 HBITMAP oldBitmap;
	 HBITMAP hBitmap;	
     SIZE size;
     size.cx = widgetMask.width();
     size.cy = widgetMask.height();
     HDC screenDc = GetDC(NULL);
     POINT pointSource;
     pointSource.x = 0;
     pointSource.y = 0; 
     POINT topPos;
     topPos.x = skinWidget->x();
     topPos.y = skinWidget->y();	
     HDC memDc = CreateCompatibleDC(screenDc);
     BLENDFUNCTION blend;
     blend.BlendOp             = AC_SRC_OVER;
     blend.BlendFlags          = 0;
     blend.SourceConstantAlpha = 255;
     blend.AlphaFormat         = AC_SRC_ALPHA;
	 hBitmap = widgetMask.toWinHBITMAP(QPixmap::PremultipliedAlpha);  // grab a GDI handle from this GDI+ bitmap
	 oldBitmap = (HBITMAP)SelectObject(memDc, hBitmap);
	 
	UpdateLayeredWindow(skinWidget->winId(), screenDc,  &topPos,  &size, memDc,  &pointSource, 0, &blend, ULW_ALPHA);
	
	// 
	ReleaseDC( NULL, screenDc);
	if (hBitmap != NULL)
	{
		SelectObject(memDc, oldBitmap);
		//DeleteObject(hBitmap); // The documentation says that we have to use the Windows.DeleteObject... but since there is no such method I use the normal DeleteObject from Win32 GDI and it's working fine without any resource leak.
		DeleteObject(hBitmap);
	}
	DeleteDC(memDc); 
}

void QSkinObject::setLayered()
{
	SetWindowLong(skinWidget->winId(), 
              GWL_EXSTYLE, 
              GetWindowLong(skinWidget->winId(), GWL_EXSTYLE) | WS_EX_LAYERED); //make shure the QAlphaWidget is realy a windows QAlphaWidget form
}
#endif
void QSkinObject::fastbluralpha(QImage &img, int radius)
{
    if (radius < 1) {
        return;
    }

    QRgb *pix = (QRgb*)img.bits();
    int w   = img.width();
    int h   = img.height();
    int wm  = w-1;
    int hm  = h-1;
    int wh  = w*h;
    int div = radius+radius+1;

    int *r = new int[wh];
    int *g = new int[wh];
    int *b = new int[wh];
    int *a = new int[wh];
    int rsum, gsum, bsum, asum, x, y, i, yp, yi, yw;
    QRgb p;
    int *vmin = new int[qMax(w,h)];

    int divsum = (div+1)>>1;
    divsum *= divsum;
    int *dv = new int[256*divsum];
    for (i=0; i < 256*divsum; ++i) {
        dv[i] = (i/divsum);
    }

    yw = yi = 0;

    int **stack = new int*[div];
    for(int i = 0; i < div; ++i) {
        stack[i] = new int[4];
    }


    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius+1;
    int routsum, goutsum, boutsum, aoutsum;
    int rinsum, ginsum, binsum, ainsum;

    for (y = 0; y < h; ++y){
        rinsum = ginsum = binsum = ainsum
               = routsum = goutsum = boutsum = aoutsum
               = rsum = gsum = bsum = asum = 0;
        for(i =- radius; i <= radius; ++i) {
            p = pix[yi+qMin(wm,qMax(i,0))];
            sir = stack[i+radius];
            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);
            
            rbs = r1-abs(i);
            rsum += sir[0]*rbs;
            gsum += sir[1]*rbs;
            bsum += sir[2]*rbs;
            asum += sir[3]*rbs;
            
            if (i > 0){
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }
        }
        stackpointer = radius;

        for (x=0; x < w; ++x) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            a[yi] = dv[asum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (y == 0) {
                vmin[x] = qMin(x+radius+1,wm);
            }
            p = pix[yw+vmin[x]];

            sir[0] = qRed(p);
            sir[1] = qGreen(p);
            sir[2] = qBlue(p);
            sir[3] = qAlpha(p);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[(stackpointer)%div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            ++yi;
        }
        yw += w;
    }
    for (x=0; x < w; ++x){
        rinsum = ginsum = binsum = ainsum 
               = routsum = goutsum = boutsum = aoutsum 
               = rsum = gsum = bsum = asum = 0;
        
        yp =- radius * w;
        
        for(i=-radius; i <= radius; ++i) {
            yi=qMax(0,yp)+x;

            sir = stack[i+radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            sir[3] = a[yi];

            rbs = r1-abs(i);

            rsum += r[yi]*rbs;
            gsum += g[yi]*rbs;
            bsum += b[yi]*rbs;
            asum += a[yi]*rbs;

            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
                ainsum += sir[3];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
                aoutsum += sir[3];
            }

            if (i < hm){
                yp += w;
            }
        }

        yi = x;
        stackpointer = radius;

        for (y=0; y < h; ++y){
            pix[yi] = qRgba(dv[rsum], dv[gsum], dv[bsum], dv[asum]);

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            asum -= aoutsum;

            stackstart = stackpointer-radius+div;
            sir = stack[stackstart%div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            aoutsum -= sir[3];

            if (x==0){
                vmin[y] = qMin(y+r1,hm)*w;
            }
            p = x+vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];
            sir[3] = a[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            ainsum += sir[3];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            asum += ainsum;

            stackpointer = (stackpointer+1)%div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            aoutsum += sir[3];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            ainsum -= sir[3];

            yi += w;
        }
    }
    delete [] r;
    delete [] g;
    delete [] b;
    delete [] a;
    delete [] vmin;
    delete [] dv;

    for(int i = 0; i < div; ++i) {
        delete [] stack[i];
    }
    delete [] stack;
}
