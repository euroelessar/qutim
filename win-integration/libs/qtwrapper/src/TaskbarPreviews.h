#include "global.h"
#include <QWidget>
#include <QMap>
#include <QSize>
#include <qt_windows.h>

class W7QTEXPORT PreviewProvider : public QObject {
	Q_OBJECT

public:
	virtual QPixmap IconicPreview(QWidget *tab, QWidget *owner, QSize)
	{
		Q_UNUSED(tab);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap LivePreview(QWidget *tab, QWidget *owner)
	{
		Q_UNUSED(tab);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap IconicPreview(unsigned tabid, QWidget *owner, QSize)
	{
		Q_UNUSED(tabid);
		Q_UNUSED(owner);
		return QPixmap();
	}

	virtual QPixmap LivePreview(unsigned tabid, QWidget *owner)
	{
		Q_UNUSED(tabid);
		Q_UNUSED(owner);
		return QPixmap();
	}
};

class W7QTEXPORT TaskbarPreviews : public QObject // TODO: обработка случаев, когда таб - отдельное окно
{
	Q_OBJECT

	enum TabType {
		ttWidget, // is a widget
		ttWindow, // is a window
		ttVirtual, // has no widget/window so PP must be specified
		ttUnknown = 100500
	};

	struct _tab
	{
		_tab();
		HWND tabHandle;
		QWidget *internalTabWidget;
		QWidget *userTabWidget;
		QWidget *userTabOwner;
		PreviewProvider *pp;
		TabType type;
		unsigned id;
	};

	class TabsList : public QList<_tab>
	{
	public:
		//TabsList();
		void insert(QWidget *internal, QWidget *userTab, QWidget *tabOwner, PreviewProvider *pp = 0);
		QWidget *internal(HWND internalHwnd);
		QWidget *internal(QWidget *user);
		QWidget *internal(unsigned id);
		TabType type(HWND internalHwnd);
		QWidget *user(HWND internalHwnd);
		unsigned id(HWND internalHwnd);
		QWidget *owner(HWND internalHwnd);
		PreviewProvider *previews(HWND internalHwnd);
	};

	TabsList m_tabs;

	TaskbarPreviews();

	friend class Win7EventFilter;
	QPixmap IconicThumbnail(HWND, QSize);
	QPixmap IconicLivePreviewBitmap(HWND);
	bool WasTabActivated(HWND);
	bool WasTabRemoved(HWND);

public:
	static TaskbarPreviews *instance();
	static unsigned tabAdd    (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	static unsigned tabAddVirtual(PreviewProvider *pp, QWidget *owner, const QString &title, QWidget *before = 0);
	static void tabActivate   (QWidget *tab);
	static void tabActivate   (unsigned tabid);
	static void tabRemove     (QWidget *tab);
	static void tabRemove     (unsigned tabid);
	static void tabOrderChange(QWidget *tab,   QWidget *before);
	static void tabOrderChange(unsigned tabid, unsigned beforeid);
	static void tabOrderChange(QWidget *tab,   unsigned beforeid);
	static void tabOrderChange(unsigned tabid, QWidget *before);
	static void tabsClear();
	static void tabPreviewsRefresh(QWidget *tab);
	static void tabPreviewsRefresh(unsigned tabid);
	static void tabSetTitle(unsigned tabid, QString &title);
	static void tabSetTitle(QWidget *tab,   QString &customTitle);

public slots:
	unsigned addTab (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	unsigned addVirtualTab(PreviewProvider *pp, QWidget *owner, const QString &title = "", QWidget *before = 0);
	void removeTab  (QWidget *tab);
	void removeTab  (unsigned tabid);
	void activateTab(QWidget *tab);
	void activateTab(unsigned tabid);
	void changeOrder(QWidget *tab,   QWidget *before);
	void changeOrder(unsigned tabid, unsigned beforeid);
	void changeOrder(QWidget *tab,   unsigned beforeid);
	void changeOrder(unsigned tabid, QWidget *before);
	void clear();
	void widgetDestroyed();
	void refreshPreviews(QWidget *tab);
	void refreshPreviews(unsigned tabid);
	void setTabTitle(unsigned tabid, QString &title);
	void setTabTitle(QWidget *tab,   QString &customTitle);

signals:
	void tabActivated(QWidget *tab);
	void tabActivated(unsigned id);
	void tabAboutToRemove(QWidget *tab, bool *ignore);
	void tabAboutToRemove(unsigned id,  bool *ignore);
};
