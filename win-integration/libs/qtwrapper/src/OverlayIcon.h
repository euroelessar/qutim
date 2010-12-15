#ifndef OVERLAYICON_H
#define OVERLAYICON_H

#include "global.h"
#include <QObject>

class QIcon;
class QWidget;
class QPixmap;

class W7QTEXPORT OverlayIcon : public QObject
{
	QWidget *m_window;

public:
	OverlayIcon(QWidget *, QObject *parent = 0);
	static void set(QWidget *, const QIcon &);
	static void set(QWidget *, const QPixmap &);
	static void set(QWidget *, const QString &path);
	static void clear(QWidget *);
	void set(const QIcon &);
	void set(const QPixmap &);
	void set(const QString &path);
	void clear();
	void changeWindow(QWidget*);
};

#endif // OVERLAYICON_H
