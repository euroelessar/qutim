#include "mmenu.h"
#include <QMenuBar>
#include <qutim/debug.h>
#include <qutim/servicemanager.h>
#include <qutim/menucontroller.h>
#include <qutim/account.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	MMenu::MMenu()
	{
		QMenuBar *menuBar = new QMenuBar(0);
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle(tr("File"));
		menuBar->insertMenu(menuBar->actions().at(0), menu);
		if (MenuController *cl = ServiceManager::getByName<MenuController *>("ContactList"))
			menu->addActions(cl->menu()->actions());
		menu = new QMenu(menuBar);
		menu->setTitle(tr("Edit"));
		menuBar->insertMenu(menuBar->actions().at(1), menu);
		menu = new QMenu(menuBar);
		menu->setTitle(tr("Accounts"));
		menuBar->insertMenu(menuBar->actions().at(2), menu);
		foreach(Account *account, Account::all())
			menu->addMenu(account->menu());
		menu = new QMenu(menuBar);
		menu->setTitle(tr("Opened chats"));
		menuBar->insertMenu(menuBar->actions().at(3), menu);
		menu = new QMenu(menuBar);
		menu->setTitle(tr("Roster"));
		menuBar->insertMenu(menuBar->actions().at(4), menu);
	}  

	MMenu::~MMenu()
	{
	}
}
