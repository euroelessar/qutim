#include "global.h"
#include <QWidget>
#include <QMap>
#include <QSize>
#include <qt_windows.h>

class W7QTEXPORT PreviewProvider : public QObject {
	Q_OBJECT

public:
	virtual QPixmap IconicPreview(QWidget *tab, QWidget *owner, QSize) = 0;
	virtual QPixmap LivePreview(QWidget *tab, QWidget *owner) = 0;
};

class W7QTEXPORT TaskbarPreviews : public QObject // TODO: обработка случаев, когда таб - отдельное окно
{
	Q_OBJECT

	struct _tab
	{
		HWND tabHandle;
		QWidget *internalTabWidget;
		QWidget *userTabWidget;
		QWidget *userTabOwner;
		PreviewProvider *pp;
	};

	class TabsList : public QList<_tab>
	{
	public:
		//TabsList();
		void insert(QWidget *internal, QWidget *userTab, QWidget *tabOwner, PreviewProvider *pp = 0);
		QWidget *internal(HWND internalHandle);
		QWidget *internal(QWidget *user);
		QWidget *user(HWND internalHandle);
		QWidget *owner(HWND internalHandle);
		PreviewProvider *previews(HWND internalHandle);
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
	static void tabAdd        (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	static void tabActivate   (QWidget *tab);
	static void tabRemove     (QWidget *tab);
	static void tabOrderChange(QWidget *tab, QWidget *before);
	static void tabsClear();
	static void tabPreviewsRefresh(QWidget *tab);

public slots:
	void addTab     (QWidget *tab, QWidget *owner, const QString &title = "", QWidget *before = 0, PreviewProvider *pp = 0);
	void activateTab(QWidget *tab);
	void removeTab  (QWidget *tab);
	void clear();
	void changeOrder(QWidget *tab, QWidget *before);
	void widgetDestroyed();
	void refreshPreviews(QWidget *tab);

signals:
	void tabActivated(QWidget *tab);
	void tabAboutToRemove(QWidget *tab, bool *ignore);
};
