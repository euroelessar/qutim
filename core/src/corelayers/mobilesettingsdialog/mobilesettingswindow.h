/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef XSETTINGSWINDOW_H
#define XSETTINGSWINDOW_H

#include <QMainWindow>
#include <qutim/settingslayer.h>
#include <slidingstackedwidget.h>

class QListWidgetItem;
class QStackedWidget;
class QActionGroup;
class QModelIndex;
namespace qutim_sdk_0_3 {
class ActionToolBar;
}

namespace Core
{

using namespace qutim_sdk_0_3;

struct MobileSettingsWindowPrivate;
class MobileSettingsWindow : public QMainWindow
{
    Q_OBJECT
public:
	explicit MobileSettingsWindow(const SettingsItemList &settings,QObject *controller);
	void update(const SettingsItemList &settings);
	virtual ~MobileSettingsWindow();
public slots:
	void save();
	void cancel();
protected:
	void loadSettings(const SettingsItemList &settings);
	void ensureActions();
    virtual void closeEvent(QCloseEvent *ev);
private:
	QListWidgetItem *get(Settings::Type);
	QScopedPointer<MobileSettingsWindowPrivate> p;
private slots:
	void onCategoryActivated(const QModelIndex &index);
	void onCategoryActivated(QListWidgetItem *item);
	void onCurrentItemActivated(const QModelIndex &index);
    void onModifiedChanged(bool haveChanges);
	void onWidgetDestroyed(QObject *obj);
	void slideUp();
	void slideDown(QWidget *w);
	void fingerGesture(enum SlidingStackedWidget::SlideDirection);
	void initScrolling();
};

}

#endif // XSETTINGSWINDOW_H

