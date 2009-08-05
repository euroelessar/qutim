/********************************************************************************************************
* PROGRAM : QSkinObject
* DATE - TIME : Samstag 30 Dezember 2006 - 12h04
* AUTHOR : Markus Künkle( http://amobos.org )
* FILENAME : QSkinObject.h
* LICENSE : GPL
* COMMENTARY : QObject subclass for theming Qt\'s QWidget.
********************************************************************************************************/
#ifndef QSkinObject_H
#define QSkinObject_H
#include "qskinwidgetresizehandler.h"
#include <QtGui>
#ifdef WIN32
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#include <windows.h>
#endif
class QSkinWidgetResizeHandler;
class  QSkinObject : public QObject
{
    Q_OBJECT
    friend class QSkinWidgetResizeHandler;
public:
    	QSkinObject(QWidget* wgtParent);
	~QSkinObject(){}
	void setSkinPath(const QString & skinpath);
	QString getSkinPath();
	int customFrameWidth();
	void loadSkinIni(const QString &path_to_border);
	QWidget *getWidget() { return skinWidget; }
public slots:
	void updateStyle();
	void updateButtons();
	void startSkinning();
	void stopSkinning();
protected:
	bool eventFilter(QObject *o, QEvent *e);
	//Events to filter
	//svoid mouseMoveEvent(QMouseEvent *event);
    	void mousePressEvent(QMouseEvent *event);
    	//void mouseReleaseEvent(QMouseEvent *mouseEvent);
    	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *e);
	//void closeEvent(QCloseEvent *e);


	void manageRegions();
	QPixmap drawCtrl(QWidget * widget);
	QRegion childRegion;
	void timerEvent ( QTimerEvent * event );
private:
    	QPoint dragPosition;
	QPixmap widgetMask;//the pixmap, in which the ready frame is stored on pressed? 
	QString skinPath;
	QFont titleFont;
	QColor titleColor;
	QColor backgroundColor;
	bool milchglas;
    	bool gotMousePress;	
	QRegion quitButton;
	QRegion maxButton;
	QRegion minButton;
	QRect contentsRect;
	QSkinWidgetResizeHandler * resizeHandler;
	bool mousePress;
	QBasicTimer *skinTimer;
	QWidget *skinWidget;
	void fastbluralpha(QImage &img, int radius);
	Qt::WindowFlags flags;
	int wlong;
	bool m_mask_setted;
	bool m_up_layout;
	int m_margin_right;
	int m_margin_up;
	int m_margin_down;
	int m_margin_left;
	bool m_button_hovered;
	QColor m_background_color;
#ifdef WIN32
public slots:
	void setLayered();
	void updateAlpha();
private:
	double alpha;
#endif
};
#endif

