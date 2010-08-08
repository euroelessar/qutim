#include "xsettingswindow.h"
#include <QListWidget>
#include <QStackedWidget>
#include <libqutim/actiontoolbar.h>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <libqutim/icon.h>
#include <libqutim/settingswidget.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>

namespace Core
{

struct ActionEntry
{
	//TODO move to libqutim
	ActionEntry(const LocalizedString &t,const QIcon &i)
	{
		text = t;
		icon = i;
	}
	ActionEntry() {}
	LocalizedString text;
	QIcon icon;
};

typedef QMap<Settings::Type,ActionEntry> ActionEntryMap;

static ActionEntryMap init_entry_map()
{
	ActionEntryMap map;
	map.insert(Settings::General,ActionEntry(QT_TRANSLATE_NOOP("Settings","General"),Icon("preferences-system")));
	map.insert(Settings::Protocol,ActionEntry(QT_TRANSLATE_NOOP("Settings","Protocols"),Icon("applications-internet")));
	map.insert(Settings::Appearance,ActionEntry(QT_TRANSLATE_NOOP("Settings","Appearance"),Icon("applications-graphics")));
	map.insert(Settings::Plugin,ActionEntry(QT_TRANSLATE_NOOP("Settings","Plugins"),Icon("applications-other")));
	map.insert(Settings::Special,ActionEntry(QT_TRANSLATE_NOOP("Settings","Special"),QIcon()));
	map.insert(Settings::Invalid,ActionEntry(QT_TRANSLATE_NOOP("Settings","Invalid"),QIcon()));
	return map;
}

ActionEntryMap *entries()
{
	static ActionEntryMap map (init_entry_map());
	return &map;
}

struct XSettingsWindowPrivate
{
	ActionToolBar *toolBar;
	QActionGroup *group;
	QStackedWidget *stackedWidget;
	QListWidget *listWidget;
	QDialogButtonBox *buttonBox;
	QObject *controller;
	QMap<Settings::Type,SettingsItem*> items;
	QList<SettingsWidget*> modifiedWidgets;
	QMap<Settings::Type,QAction*> actionMap;
	QAction *currentAction;
};
	
XSettingsWindow::XSettingsWindow(const qutim_sdk_0_3::SettingsItemList& settings, QObject* controller) :
	p(new XSettingsWindowPrivate)
{
	setAttribute(Qt::WA_DeleteOnClose);
	p->controller = controller;
	//setup ui
	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	QVBoxLayout *l = new QVBoxLayout(w);
	w->setLayout(l);
	QSize desktop_size = qApp->desktop()->size();
	resize(desktop_size.width()/2,desktop_size.height()*2/3);
	centerizeWidget(this);
	//init widgets
	QSplitter *splitter = new QSplitter(Qt::Horizontal,w);
	p->listWidget = new QListWidget(w);
	p->stackedWidget = new QStackedWidget(w);
	splitter->addWidget(p->listWidget);
	splitter->addWidget(p->stackedWidget);
	splitter->setSizes(QList<int>() << 80  << 250);
	l->addWidget(splitter);
	//init buttonBox
// 	QFrame *line = new QFrame(w);
// 	line->setFrameShape(QFrame::HLine);
// 	line->setFrameShadow(QFrame::Sunken);
// 	l->addWidget(line);
	p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel,Qt::Horizontal,w);
	l->addWidget(p->buttonBox);
	p->buttonBox->hide();
	//init actiontoolbar
	p->toolBar = new ActionToolBar(w);
	p->toolBar->setIconSize(QSize(32,32));
	p->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addToolBar(Qt::TopToolBarArea,p->toolBar);
	setUnifiedTitleAndToolBarOnMac(true);
	p->group = new QActionGroup(w);
	p->group->setExclusive(true);
	//connections
	connect(p->group,SIGNAL(triggered(QAction*)),SLOT(onGroupActionTriggered(QAction*)));
	connect(p->listWidget,
			SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			SLOT(onCurrentItemChanged(QListWidgetItem*))
		   );
	connect(p->buttonBox,SIGNAL(accepted()),SLOT(save()));
	connect(p->buttonBox,SIGNAL(rejected()),SLOT(cancel()));
	loadSettings(settings);
}

	
void XSettingsWindow::update(const qutim_sdk_0_3::SettingsItemList& settings)
{
	//TODO add cleaner
	qDeleteAllLater(p->items);
	p->items.clear();
	loadSettings(settings);
}

XSettingsWindow::~XSettingsWindow()
{

}

void XSettingsWindow::loadSettings(const qutim_sdk_0_3::SettingsItemList& settings)
{
	foreach (SettingsItem *item,settings) {
		//QListWidgetItem *list_item = new QListWidgetItem(item->icon(),item->text(),p->listWidget);
		p->items.insertMulti(item->type(),item);
	}
	ensureActions();
	if (p->group->actions().count())
		p->group->actions().first()->trigger();
}

QAction* XSettingsWindow::get(Settings::Type type)
{
	ActionEntry entry = entries()->value(type);
	QAction *action = p->actionMap.value(type);
	if (!action) {
		action = new QAction(entry.icon,entry.text.toString(),this);
		action->setCheckable(true);
		p->actionMap.insert(type,action);
		p->group->addAction(action);
	}
	return action;
}


void XSettingsWindow::ensureActions()
{
	qDeleteAll(p->actionMap);
	p->actionMap.clear();
	foreach (Settings::Type type,p->items.keys()) {
		QAction *a = get(type);
		//small spike
		if (!p->toolBar->actions().contains(a)) {
			p->toolBar->addAction(a);
			if (type == Settings::General)
				p->toolBar->addSeparator();
		}
	}
	p->toolBar->setVisible(p->actionMap.count() > 1);
}

void XSettingsWindow::onGroupActionTriggered(QAction *a )
{
	p->currentAction = a;
	//remove old settings widgets
	QWidget *c = p->stackedWidget->currentWidget();
	for (int index = 0;index!=p->stackedWidget->count();index++) {
		SettingsWidget *w = static_cast<SettingsWidget*>(p->stackedWidget->widget(index));
		if (w != c && !p->modifiedWidgets.contains(w))
			w->deleteLater();
	}
	p->listWidget->clear();
	SettingsItemList list = p->items.values(p->actionMap.key(a));
	foreach (SettingsItem *item,list) {
		QListWidgetItem *list_item = new QListWidgetItem(item->icon(),item->text(),p->listWidget);
		list_item->setData(Qt::UserRole,reinterpret_cast<qptrdiff>(item));
	}
	if (p->listWidget->count() > 1)
		p->listWidget->show();
	else
		p->listWidget->hide();
	p->listWidget->setCurrentRow(0); //TODO save current row
}

void XSettingsWindow::onCurrentItemChanged(QListWidgetItem *item)
{
	//FIXME don't use reinterpret_cast
	if (!item)
		return;
	qptrdiff ptr = item->data(Qt::UserRole).value<qptrdiff>();
	if (!ptr)
		return;
	SettingsItem *settingsItem = reinterpret_cast<SettingsItem*>(ptr);

	SettingsWidget *w = settingsItem->widget();	
	if (p->stackedWidget->indexOf(w) == -1) {
		p->stackedWidget->addWidget(w);
		w->load();
		w->setController(p->controller);
		connect(w,SIGNAL(modifiedChanged(bool)),SLOT(onModifiedChanged(bool)));
	}
	p->stackedWidget->setCurrentWidget(w);
	setWindowTitle(tr("qutIM settings - %1").arg(settingsItem->text()));
}

void XSettingsWindow::onModifiedChanged(bool haveChanges)
{
	SettingsWidget *w = qobject_cast<SettingsWidget*>(sender());
	Q_ASSERT(w);
	if (haveChanges)
		p->modifiedWidgets.append(w);
	p->buttonBox->show();
}

void XSettingsWindow::closeEvent(QCloseEvent* ev)
{
	if (p->modifiedWidgets.count()) {
		int ret = QMessageBox::question(this,
										tr("Apply Settings - System Settings"),
										tr("The settings of the current module have changed. \n Do you want to apply the changes or discard them?"),
										QMessageBox::Apply,
										QMessageBox::Discard,
										QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Apply:
				save();
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				cancel();
				ev->ignore();
				break;
			default:
				break;
		}
	}
}

void XSettingsWindow::save()
{
	QWidget *c = p->stackedWidget->currentWidget();
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}
	p->buttonBox->close();
}

void XSettingsWindow::cancel()
{
	QWidget *c = p->stackedWidget->currentWidget();	
	while (p->modifiedWidgets.count()) {
		SettingsWidget *widget = p->modifiedWidgets.takeFirst();
		widget->save();
		if (widget != c)
			widget->deleteLater();
	}	
	p->buttonBox->close();
}


}
