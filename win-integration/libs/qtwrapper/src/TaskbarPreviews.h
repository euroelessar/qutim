#include "global.h"
#include <QWidget>
#include <QMap>
#include <QSize>
#include <QFlags>
#include <qt_windows.h>
#include "..\..\apilayer\src\ApiTaskbarPreviewsWAttributes.h"

Q_DECLARE_FLAGS(WindowAttributes, WindowAttribute)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowAttributes)

class WTQTEXPORT PreviewProvider : public QObject {
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

class WTQTEXPORT TaskbarPreviews : public QObject
{
	Q_OBJECT

	enum TabType {
		ttNotSet = 0,
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
	static void setWindowAttributes(QWidget *window, WindowAttributes wa);
	static void tabActivate   (QWidget *tab);
	static void tabActivate   (unsigned tabid);
	static void tabOrderChange(QWidget *tab,   QWidget *before);
	static void tabOrderChange(QWidget *tab,   unsigned beforeid);
	static void tabOrderChange(unsigned tabid, QWidget *before);
	static void tabOrderChange(unsigned tabid, unsigned beforeid);
	static void tabPreviewsRefresh(QWidget *tab);
	static void tabPreviewsRefresh(unsigned tabid);
	static void tabRemove     (QWidget *tab);
	static void tabRemove     (unsigned tabid);
	static void tabsClear();
	static void tabSetTitle   (QWidget *tab,   QString &customTitle);
	static void tabSetTitle   (unsigned tabid, QString &title);

public slots:
	unsigned addTab (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	unsigned addVirtualTab(PreviewProvider *pp, QWidget *owner, const QString &title = "", QWidget *before = 0);
	void activateTab(QWidget *tab);
	void activateTab(unsigned tabid);
	void changeOrder(QWidget *tab,   QWidget *before);
	void changeOrder(QWidget *tab,   unsigned beforeid);
	void changeOrder(unsigned tabid, QWidget *before);
	void changeOrder(unsigned tabid, unsigned beforeid);
	void clear();
	void refreshPreviews(QWidget *tab);
	void refreshPreviews(unsigned tabid);
	void removeTab  (QWidget *tab);
	void removeTab  (unsigned tabid);
	void setTabTitle(QWidget *tab,   QString &customTitle);
	void setTabTitle(unsigned tabid, QString &title);
	void widgetDestroyed();

signals:
	void tabAboutToRemove(QWidget *tab, bool *ignore);
	void tabAboutToRemove(unsigned id,  bool *ignore);
	void tabActivated(QWidget *tab);
	void tabActivated(unsigned id);
};

