/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "settingsui.h"
#include "player.h"
#include <qutim/objectgenerator.h>
#include "accounttunestatus.h"
#include "ui_nowplayingsettings.h"

namespace qutim_sdk_0_3 {

namespace nowplaying {

	SettingsUI::SettingsUI() :
		ui(new Ui::NowPlayingSettings), m_manager(NowPlaying::instance())
	{
		ui->setupUi(this);
		foreach (AccountTuneStatus *factory, m_manager->factories()) {
			AccountTuneSettings *w = factory->settingsWidget();
			if (!w)
				continue;
			w->setParent(this);
			m_settingWidgets.insert(factory, w);
			ui->protocols->addTab(w, w->windowIcon(), w->windowTitle());
		}
		listenChildrenStates(QWidgetList() << ui->accounts);
	}

	SettingsUI::~SettingsUI()
	{
		delete ui;
	}

	void SettingsUI::loadImpl()
	{
		ui->accounts->blockSignals(true);
		ui->forAllAccounts->blockSignals(true);

		updateStatusText();
		ui->forAllAccounts->setChecked(m_manager->forAllAccounts());
		ui->accounts->clear();

		m_accounts = m_manager->accounts();
		foreach (AccountTuneStatus *account, m_accounts)
			ui->accounts->addItem(account->account()->id());
		if (!m_accounts.size()) {
			ui->mode_settigs->setCurrentWidget(ui->no_accounts);
			return;
		} else {
			m_currentAccount = m_accounts.first();
		}

		updateFields();

		ui->players->clear();
		ui->players->addItem(tr("No player"));
		foreach(PlayerFactory *factory, m_manager->playerFactories()) {
			const QMap<QString,Player::Info> map = factory->players();
			QMap<QString,Player::Info>::const_iterator it = map.constBegin();
			for (;it != map.constEnd(); it++) {
				const Player::Info &info = it.value();
				if (!info.settings.isNull()) {
					QWidget *widget = AbstractDataForm::get(info.settings);
					if (widget) {
						m_playerWidgets.insert(it.key(), widget);
						ui->playerSettings->addWidget(widget);
					}
				}
				ui->players->addItem(info.icon, info.name, it.key());
			}
		}
		Player *currentPlayer = m_manager->currentPlayer();
		if (currentPlayer) {
			QString playerId = m_manager->currentPlayerId();
			ui->players->setCurrentIndex(ui->players->findData(playerId));
		}

		ui->accounts->blockSignals(false);
		ui->forAllAccounts->blockSignals(false);
	}

	void SettingsUI::saveImpl()
	{
		Config cfg = config("global");
		cfg.setValue("isWorking", m_manager->isWorking());
		cfg.setValue("player", ui->players->itemData(ui->players->currentIndex()));
		cfg.setValue("enableForAllAccounts", m_enableForAllAccounts);
		saveState();
		foreach (AccountTuneSettings *w, m_settingWidgets)
			w->saveConfigs();
		m_manager->loadSettings();
		if (m_manager->currentPlayer()) {
			QWidget *widget = ui->playerSettings->currentWidget();
			if (AbstractDataForm *form = qobject_cast<AbstractDataForm*>(widget))
				m_manager->currentPlayer()->applySettings(form->item());
		}
	}


	void SettingsUI::cancelImpl()
	{
		foreach (AccountTuneSettings *w, m_settingWidgets)
			w->clearStates();
		loadImpl();
	}

	void SettingsUI::on_accounts_currentIndexChanged(int index)
	{
		saveState();
		m_currentAccount = m_accounts.value(index);
		updateFields();
	}

	void SettingsUI::on_changeStatus_clicked()
	{
		m_manager->setState(!m_manager->isWorking());
		updateStatusText();
	}

	void SettingsUI::on_forAllAccounts_clicked()
	{
		saveState();
		m_enableForAllAccounts = ui->forAllAccounts->isChecked();
		updateFields();
	}

	void SettingsUI::updateFields()
	{
		if (m_enableForAllAccounts) {
			QHashIterator<AccountTuneStatus*, AccountTuneSettings*> itr(m_settingWidgets);
			int i = 0;
			while (itr.hasNext()) {
				itr.next();
				itr.value()->loadState(itr.key());
				ui->protocols->setTabEnabled(i++, true);
			}
		} else if (m_currentAccount) {
			AccountTuneStatus *factory = m_currentAccount.data()->factory();
			if (factory) {
				AccountTuneSettings *w = m_settingWidgets.value(factory);
				if (w) {
					w->loadState(m_currentAccount.data());
					for (int i = 0, c = ui->protocols->count(); i < c; ++i)
						ui->protocols->setTabEnabled(i, ui->protocols->widget(i) == w);
				}
			}
		}
	}
	
	void SettingsUI::on_playerSettings_currentChanged(int index)
	{
		QString playerId = ui->players->itemData(index).toString();
		QWidget *widget = m_playerWidgets.value(playerId);
		if (widget)
			ui->playerSettings->setCurrentWidget(widget);
		else
			ui->playerSettings->setCurrentIndex(0);
	}

	void SettingsUI::updateStatusText()
	{
		if (m_manager->isWorking()) {
			ui->status_text->setText(" " + tr("working"));
			ui->changeStatus->setText(tr("Stop"));
		} else {
			ui->status_text->setText(" " + tr("not working"));
			ui->changeStatus->setText(tr("Start"));
		}
	}

	void SettingsUI::saveState()
	{
		if (m_enableForAllAccounts) {
			QHashIterator<AccountTuneStatus*, AccountTuneSettings*> itr(m_settingWidgets);
			while (itr.hasNext()) {
				itr.next();
				itr.value()->saveState(itr.key());
			}
		} else {
			if (m_currentAccount) {
				AccountTuneStatus *factory = m_currentAccount.data()->factory();
				if (factory) {
					AccountTuneSettings *w = m_settingWidgets.value(factory);
					if (w)
						w->saveState(m_currentAccount.data());
				}
			}
		}
	}

} }

