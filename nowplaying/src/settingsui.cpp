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
		connect(ui->accounts, SIGNAL(currentIndexChanged(int)), SLOT(accountChanged(int)));
		connect(ui->change_status, SIGNAL(clicked()), SLOT(stopButtonClicked()));
		connect(ui->for_all_accounts, SIGNAL(toggled(bool)), SLOT(forAllAccountsClicked()));
	}

	SettingsUI::~SettingsUI()
	{
		delete ui;
	}

	void SettingsUI::loadImpl()
	{
		ui->accounts->blockSignals(true);
		ui->for_all_accounts->blockSignals(true);

		updateStatusText();
		Config cfg = config("global");
		m_enableForAllAccounts = cfg.value("enableForAllAccounts", true);
		ui->for_all_accounts->setChecked(m_enableForAllAccounts);
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
		foreach(Player* player, m_manager->players())
			ui->players->addItem(player->playerName());
		Player *currentPlayer = m_manager->currentPlayer();
		if (currentPlayer)
			ui->players->setCurrentIndex(ui->players->findText(currentPlayer->playerName()));

		ui->accounts->blockSignals(false);
		ui->for_all_accounts->blockSignals(false);
	}

	void SettingsUI::saveImpl()
	{
		Config cfg = config("global");
		cfg.setValue("isWorking", m_manager->isWorking());
		cfg.setValue("player", ui->players->currentText());
		cfg.setValue("enableForAllAccounts", m_enableForAllAccounts);
		saveState();
		foreach (AccountTuneSettings *w, m_settingWidgets)
			w->saveConfigs();
		m_manager->loadSettings();
	}


	void SettingsUI::cancelImpl()
	{
		foreach (AccountTuneSettings *w, m_settingWidgets)
			w->clearStates();
		loadImpl();
	}

	void SettingsUI::accountChanged(int index)
	{
		saveState();
		m_currentAccount = m_accounts.value(index);
		updateFields();
	}

	void SettingsUI::stopButtonClicked()
	{
		m_manager->setState(!m_manager->isWorking());
		updateStatusText();
	}

	void SettingsUI::forAllAccountsClicked()
	{
		saveState();
		m_enableForAllAccounts = ui->for_all_accounts->isChecked();
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
			AccountTuneStatus *factory = m_currentAccount->factory();
			if (factory) {
				AccountTuneSettings *w = m_settingWidgets.value(factory);
				if (w) {
					w->loadState(m_currentAccount);
					for (int i = 0, c = ui->protocols->count(); i < c; ++i)
						ui->protocols->setTabEnabled(i, ui->protocols->widget(i) == w);
				}
			}
		}
	}

	void SettingsUI::updateStatusText()
	{
		if (m_manager->isWorking()) {
			ui->status_text->setText(" " + tr("working"));
			ui->change_status->setText(tr("Stop"));
		} else {
			ui->status_text->setText(" " + tr("not working"));
			ui->change_status->setText(tr("Start"));
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
				AccountTuneStatus *factory = m_currentAccount->factory();
				if (factory) {
					AccountTuneSettings *w = m_settingWidgets.value(factory);
					if (w)
						w->saveState(m_currentAccount);
				}
			}
		}
	}

} }
