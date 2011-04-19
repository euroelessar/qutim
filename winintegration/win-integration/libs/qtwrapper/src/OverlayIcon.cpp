#include "OverlayIcon.h"
#include "../../apilayer/src/ApiOverlayIcon.h"
#include <QPixmap>
#include <QIcon>
#include <QWidget>

OverlayIcon::OverlayIcon(QWidget *w, QObject *o)
	: QObject(o)
{
	changeWindow(w);
}

void OverlayIcon::changeWindow(QWidget *w)
{
	m_window = w;
}

void OverlayIcon::set(const QIcon &icon)
{
	set(icon.pixmap(16, 16));
}

void OverlayIcon::set(const QPixmap &icon)
{
	if (!m_window)
		return;
	HICON hIcon = icon.scaled(16, 16).toWinHICON();
	SetOverlayIcon(m_window->winId(), hIcon, 0);
	DestroyIcon(hIcon);
}

void OverlayIcon::set(const QString &str)
{
	set(QPixmap(str));
}

void OverlayIcon::set(QWidget *w, const QIcon &i)
{
	OverlayIcon oi(w);
	oi.set(i);
}

void OverlayIcon::set(QWidget *w, const QPixmap &p)
{
	OverlayIcon oi(w);
	oi.set(p);
}

void OverlayIcon::set(QWidget *w, const QString &s)
{
	OverlayIcon oi(w);
	oi.set(s);
}

void OverlayIcon::clear()
{
	if (m_window)
		ClearOverlayIcon(m_window->winId());
}

void OverlayIcon::clear(QWidget *w)
{
	if (w)
		ClearOverlayIcon(w->winId());
}
