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

#include "migrationstartpage.h"
#include "ui_migrationstartpage.h"
#include <QDir>
#include <QFile>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QStringBuilder>
#include <qutim/configbase.h>
#include <qutim/systeminfo.h>
#include <qutim/debug.h>

namespace Core
{
using namespace qutim_sdk_0_3;

MigrationStartPage::MigrationStartPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::MigrationStartPage)
{
	ui->setupUi(this);
	setTitle(tr("Migration wizard"));
	setSubTitle(tr("qutIM has discovered configuration from 0.2 version, "
				   "choose profile to import history and configuration from."));
	m_protos
			<< ProtoInfo("icq", "ICQ", "ICQ", "ICQ")
			<< ProtoInfo("irc", "IRC", "IRC", "IRC")
			<< ProtoInfo("jabber", "Jabber", "Jabber", "jabber")
			<< ProtoInfo("msn", "MSN", "MSN", "MSN")
			<< ProtoInfo("mrim", "Mail.Ru IM", "MRIM", "mrim")
			<< ProtoInfo("vkontakte", "VKontakte", "VKontakte", "vkontakte")
			<< ProtoInfo("twitter", "Twitter", "Twitter", "twitter");
}

MigrationStartPage::~MigrationStartPage()
{
	delete ui;
}

void MigrationStartPage::initializePage()
{
	// Let's magic begin!
	QDir configPath = qApp->applicationDirPath();

	if (!configPath.cd("config")) {
		QString userPath;
#ifdef Q_OS_WIN
		userPath = QString::fromLocal8Bit(qgetenv("APPDATA"));
#else
		QByteArray env = qgetenv("XDG_CONFIG_HOME");
		if (env.isEmpty()) {
			userPath = QDir::homePath();
			userPath += QLatin1Char('/');
#ifdef Q_WS_QWS
			userPath += QLatin1String("Settings");
#else
			userPath += QLatin1String(".config");
#endif
		} else if (env.startsWith('/')) {
			userPath = QString::fromLocal8Bit(env);
		} else {
			userPath = QDir::homePath();
			userPath += QLatin1Char('/');
			userPath += QString::fromLocal8Bit(env);
		}
#endif
		userPath += QLatin1String("/qutim");
		configPath = userPath;
	}
	if (!configPath.exists()) {
		setSubTitle(tr("Unable to find profile dir"));
		setEnabled(false);
		ui->importBox->setChecked(false);
	} else {
		debug() << configPath;
		QFileInfoList list = configPath.entryInfoList(QStringList() << "qutim.*",
													  QDir::Dirs | QDir::NoDotAndDotDot);
		QSettings settings(configPath.filePath("qutimsettings.ini"), QSettings::IniFormat);
		settings.beginGroup("profiles");
		QStringList knownProfiles = settings.value("list").toStringList();
		QString currentProfile = knownProfiles.value(settings.value("last", -1).toInt());
		debug() << knownProfiles << currentProfile << configPath.filePath("qutimsettings.ini");
		// We have list of profiles but it may be corrupted so check every path
		foreach (const QFileInfo &info, list) {
			QString profileName = info.fileName().section('.', 1);
			if (profileName.isEmpty())
				continue;
			ui->profileBox->addItem(profileName, info.absoluteFilePath());
			if (profileName == currentProfile)
				ui->profileBox->setCurrentIndex(ui->profileBox->count() - 1);
		}
		if (!ui->profileBox->count()) {
			setSubTitle(tr("Unable to find any profile"));
			setEnabled(false);
			ui->importBox->setChecked(false);
		} else {
			ui->importBox->setChecked(true);
		}
	}
}

void MigrationStartPage::on_profileBox_currentIndexChanged(int index)
{
	QDir dir = ui->profileBox->itemData(index).toString();
	for (int i = 0; i < ui->accountsList->count(); i++) {
		QListWidgetItem *item = ui->accountsList->item(i);
		ui->accountsList->removeItemWidget(item);
		delete item;
	}
	foreach (const ProtoInfo &protocol, m_protos) {
		if (!dir.exists(protocol.lower + "settings.ini"))
			continue;
		QSettings protoSettings(dir.filePath(protocol.lower + "settings.ini"),
								QSettings::IniFormat);
		protoSettings.beginGroup("accounts");
		debug() << protocol.name << protoSettings.value("list").toStringList();
		foreach (const QString &account, protoSettings.value("list").toStringList()) {
			QString accFile = dir.filePath(protocol.config % '.' % account % "/accountsettings.ini");
			debug() << accFile << QFile::exists(accFile);
			if (QFile::exists(accFile)) {
				QListWidgetItem *item = new QListWidgetItem(account % " (" % protocol.name % ")",
															ui->accountsList);
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
				item->setCheckState(Qt::Checked);
				item->setData(Qt::UserRole, accFile);
				item->setData(Qt::UserRole + 1, account);
				item->setData(Qt::UserRole + 2, protocol.lower);
			}
		}
		protoSettings.endGroup();
	}
}

bool MigrationStartPage::validatePage()
{
	if (!ui->importBox->isChecked())
		return true;
	//	Firstly copy accounts
	debug() << Q_FUNC_INFO << SystemInfo::getPath(SystemInfo::ConfigDir);
	for (int i = 0; i < ui->accountsList->count(); i++) {
		QListWidgetItem *item = ui->accountsList->item(i);
		if (item->checkState() != Qt::Checked)
			continue;
		QSettings settings(item->data(Qt::UserRole).toString(), QSettings::IniFormat);
		settings.beginGroup("main");
		QString password;
		QString protocol = item->data(Qt::UserRole + 2).toString();
		if (protocol == "icq") {
			const uchar crypter[] = {
				0x10, 0x67, 0x56, 0x78, 0x85, 0x14, 0x87,
				0x11, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45
			};
			QByteArray tmpPass = settings.value("password").toByteArray();
			QByteArray roastedPass;
			for (int i = 0, size = qMin(tmpPass.length(), int(sizeof(crypter))); i < size; i++) {
				roastedPass[i] = tmpPass.at(i) ^ crypter[i];
			}
			password = QString::fromLocal8Bit(roastedPass);
		} else {
			password = settings.value("password").toString();
		}
		Config config(protocol % QLatin1Char('.')
					  % item->data(Qt::UserRole + 1).toString()
					  % QLatin1Literal("/account"));
		config.group("general").setValue("passwd", password, Config::Crypted);
		config.sync();
		config = Config(protocol);
		ConfigGroup group = config.group("general");
		QStringList accounts = group.value("accounts", QStringList())
				<< item->data(Qt::UserRole + 1).toString();
		accounts.removeDuplicates();
		group.setValue("accounts", accounts);
		group.sync();
	}
	//	Then copy history of known protocols
	QDir historyDir = ui->profileBox->itemData(ui->profileBox->currentIndex()).toString() + "/history";
	if (historyDir.exists()) {
		QTime time;
		time.start();
		QDir newHistoryDir = SystemInfo::getDir(SystemInfo::HistoryDir);
		foreach (const ProtoInfo &protocol, m_protos) {
			QFileInfoList list = historyDir.entryInfoList(QStringList(protocol.history + ".*"),
														  QDir::Dirs | QDir::NoDotAndDotDot);
			foreach (const QFileInfo &info, list) {
				QString account = info.fileName().section(".", 1);
				newHistoryDir.mkpath(protocol.lower % "." % account);
				foreach (const QFileInfo &file, QDir(info.absoluteFilePath())
						 .entryInfoList(QStringList("*.json"), QDir::Files)) {
					QFile::copy(file.absoluteFilePath(),
								newHistoryDir.filePath(protocol.lower
													   % "." % account
													   % '/' % file.fileName()));
				}
			}
		}
		debug() << "History was imported by " << time.elapsed() << "ms";
	}
	//	TODO: Copy styles and other stuff
	return true;
}

void MigrationStartPage::changeEvent(QEvent *e)
{
	QWizardPage::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
}

