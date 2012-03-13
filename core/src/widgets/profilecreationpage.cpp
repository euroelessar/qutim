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

#include "profilecreationpage.h"
#include "ui_profilecreationpage.h"
#include "submitpage.h"
#include <qutim/cryptoservice.h>
#include <qutim/objectgenerator.h>
#include <qutim/extensioninfo.h>
#include <QDebug>
#include <QStringBuilder>
#include <QCryptographicHash>
#include <qutim/json.h>
#include <qutim/systeminfo.h>
#include <qutim/config.h>
#include <qutim/profile.h>
#include <QMessageBox>
#include <QFileDialog>
#include <qutim/debug.h>
#include "profiledialog.h"

using namespace qutim_sdk_0_3;
namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs();
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();
}

namespace Core
{
ProfileCreationPage::ProfileCreationPage(QWidget *parent) :
    QWizardPage(parent),
	ui(new Ui::ProfileCreationPage),m_is_valid(false)
{
    ui->setupUi(this);
	QDir dir = QApplication::applicationDirPath();
	QFileInfo dirInfo = dir.absolutePath();
	if (!dirInfo.isWritable()) {
		ui->portableBox->setVisible(false);
		ui->portableBox->setChecked(false);
	} else if (dir.cd(QLatin1String("profiles"))) {
		ui->portableBox->setVisible(false);
		ui->portableBox->setChecked(true);
	} else {
		Config cfg = ProfileDialog::profilesInfo();
		// We don't have portable profiles, but have some system-wide
		if (cfg.beginArray(QLatin1String("list")) > 0) {
			ui->portableBox->setVisible(false);
			ui->portableBox->setChecked(false);
		}
	}
	registerField("name", ui->nameEdit);
	registerField("id", ui->idEdit);
	registerField("portable", ui->portableBox);
	registerField("configDir", ui->configEdit);
	registerField("historyDir", ui->historyEdit);
	registerField("dataDir", ui->dataEdit);

	connect(ui->dataButton, SIGNAL(clicked()), SLOT(onPathSelectTriggered()));
	connect(ui->configButton, SIGNAL(clicked()), SLOT(onPathSelectTriggered()));
	connect(ui->historyButton, SIGNAL(clicked()), SLOT(onPathSelectTriggered()));
}

void ProfileCreationPage::initializePage()
{
	m_password = wizard()->property("password").toString();
	m_singleProfile = wizard()->property("singleProfile").toBool();
	bool first = true;
	if (!m_singleProfile) {
		registerField("crypto", ui->cryptoBox, "currentText");
	} else {
		ui->label_6->hide();
		ui->cryptoBox->hide();
		ui->cryptoDescription->hide();
	}
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<CryptoService>()) {
		const ExtensionInfo info = gen->info();
		if (!m_singleProfile
				|| info.generator()->metaObject()->className() == QLatin1String("Core::NoCryptoService")) {
			ui->cryptoBox->addItem(info.icon(), info.name(), qVariantFromValue(info));
			if (first) {
				ui->cryptoBox->setCurrentIndex(0);
				ui->cryptoDescription->setText(info.description());
				first = false;
			}
		}
	}
	Q_ASSERT_X(ui->cryptoBox->count() > 0, "ProfileCreationPage::initializePage", "Have not been found any crypto plugin");
	first = true;
	foreach (const ObjectGenerator *gen, ObjectGenerator::module<ConfigBackend>()) {
		const ExtensionInfo info = gen->info();
		ui->configBox->addItem(info.icon(), info.name(), qVariantFromValue(info));
		if (first) {
			ui->configBox->setCurrentIndex(0);
			ui->configDescription->setText(info.description());
			first = false;
		}
	}
	Q_ASSERT_X(ui->cryptoBox->count() > 0, "ProfileCreationPage::initializePage", "Have not been found any config plugin");
	rebaseDirs();
}

ProfileCreationPage::~ProfileCreationPage()
{
    delete ui;
}

bool ProfileCreationPage::createDirs(QString &path)
{
	QDir dir;
	if (!dir.mkpath(ui->configEdit->text())) {
		path = ui->configEdit->text();
		return false;
	} else if (!dir.mkpath(ui->historyEdit->text())) {
		path = ui->historyEdit->text();
		return false;
	} else if (!dir.mkpath(ui->dataEdit->text())) {
		path = ui->dataEdit->text();
		return false;
	}
	return true;
}

bool ProfileCreationPage::validatePage()
{
	//FIXME Elessar, WTF? why the generators are run on several times?
	if (m_is_valid)
		return true; //dummy

	QString str;
	if (!createDirs(str)) {
		QMessageBox::warning(this,
							 tr("Error"),
							 tr("Unable to create directory %1. "
								"Maybe you are trying to create a directory "
								"in a protected system directory").arg(str));
		return false;
	}

	QVector<QDir> &systemDirs = *system_info_dirs();
	systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(ui->configEdit->text());
	systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(ui->historyEdit->text());
	systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(ui->dataEdit->text());
	QFile file(SystemInfo::getDir(SystemInfo::ConfigDir).absoluteFilePath("profilehash"));
	if (file.exists()) {
		int ret = QMessageBox::question(this,
										tr("Warning"),
										tr("Profile already exists, overwrite?"),
										QMessageBox::Yes,
										QMessageBox::No);
		if(ret != QMessageBox::Yes)
			return false;
	}
	if(!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,tr("Warning"),tr("Unable to create profile hash"));
		return false;
	}
	ExtensionInfo info = ui->cryptoBox->itemData(ui->cryptoBox->currentIndex()).value<ExtensionInfo>();
	CryptoService *service = info.generator()->generate<CryptoService>();
	QByteArray data;
	QVariant serviceData = service->generateData(ui->idEdit->text());
	m_cryptoName = service->metaObject()->className();
	{
		QDataStream out(&data, QIODevice::WriteOnly);
		// We shouldn't store password as is
		QByteArray passwordHash = QCryptographicHash::hash(m_password.toUtf8()
														   + "5667dd05fbe97bb238711a3af63",
														   QCryptographicHash::Sha1);
		out << ui->idEdit->text() << passwordHash << m_cryptoName;
	}
	service->setPassword(m_password, serviceData);
	QVariant hash = service->crypt(data);
	file.write(hash.toByteArray());
	file.flush();
	file.close();
	m_password.clear();
	info = ui->configBox->itemData(ui->configBox->currentIndex()).value<ExtensionInfo>();
	QList<ConfigBackend*> &configBackends = get_config_backends();
	debug() << configBackends.count();
	for (int i = 0; i < ui->configBox->count(); i++) {
		ExtensionInfo extInfo = ui->configBox->itemData(i).value<ExtensionInfo>();
		ConfigBackend *backend = extInfo.generator()->generate<ConfigBackend>();
		if (i == ui->configBox->currentIndex())
			configBackends.prepend(backend);
		else
			configBackends.append(backend);
	}
	debugClearConfig();
	
	m_is_valid = true;
	return true;
}

void ProfileCreationPage::on_portableBox_toggled(bool)
{
	rebaseDirs();
}

void ProfileCreationPage::on_idEdit_textChanged(const QString &)
{
	rebaseDirs();
}

void ProfileCreationPage::on_cryptoBox_currentIndexChanged(int index)
{
	ExtensionInfo info = ui->cryptoBox->itemData(index).value<ExtensionInfo>();
	ui->cryptoDescription->setText(info.description());
}

void ProfileCreationPage::on_configBox_currentIndexChanged(int index)
{
	ExtensionInfo info = ui->configBox->itemData(index).value<ExtensionInfo>();
	ui->configDescription->setText(info.description());
}

void ProfileCreationPage::rebaseDirs()
{
	QString profiles;
	if (!m_singleProfile) {
		profiles += QLatin1String("profiles/");
		profiles += ui->idEdit->text();
	}
	if (ui->portableBox->isChecked()) {
		QDir dir = qApp->applicationDirPath();
		ui->dataEdit->setText(dir.absoluteFilePath("share"));
		dir = QDir::cleanPath(dir.absolutePath() % "/" % profiles);
		ui->configEdit->setText(dir.absoluteFilePath("config"));
		ui->historyEdit->setText(dir.absoluteFilePath("history"));
	} else {
#if defined(Q_OS_WIN)
		QDir dir = QString::fromLocal8Bit(qgetenv("APPDATA"));
		ui->dataEdit->setText(dir.absolutePath() % "/qutim/share/");
		dir = QDir::cleanPath(dir.absolutePath() % "/qutim/" % profiles);
		ui->configEdit->setText(m_singleProfile ? dir.absolutePath() : dir.absoluteFilePath("config"));
		ui->historyEdit->setText(dir.absoluteFilePath("history"));
#elif defined(Q_OS_MAC)
		QDir dir = QDir::homePath() + "/Library/Application Support/qutIM";
		ui->dataEdit->setText(dir.absoluteFilePath("share"));
		dir = QDir::cleanPath(dir.absolutePath() % "/" % profiles);
		ui->configEdit->setText(dir.absoluteFilePath("config"));
		ui->historyEdit->setText(dir.absoluteFilePath("history"));
#elif defined(Q_OS_UNIX)
		QDir dir = QDir::home();
		ui->dataEdit->setText(dir.absoluteFilePath(".local/share/qutim"));
		dir = dir.absoluteFilePath(".config/qutim/" % profiles);
		ui->configEdit->setText(m_singleProfile ? dir.absolutePath() : dir.absoluteFilePath("config"));
		ui->historyEdit->setText(dir.absoluteFilePath("history"));
#else
# error Strange os.. yeah..
#endif
	}
}

void ProfileCreationPage::changeEvent(QEvent *e)
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

void ProfileCreationPage::onPathSelectTriggered()
{
	QLineEdit *edit = 0;
	QObject *s = sender();

	if (s == ui->configButton)
		edit = ui->configEdit;
	else if (s == ui->historyButton)
		edit = ui->historyEdit;
	else if (s == ui->dataButton)
		edit = ui->dataEdit;
	else
		return;

	QString dir = QFileDialog::getExistingDirectory(this, tr("Select path"), edit->text());
	if (!dir.isEmpty())
		edit->setText(dir);
}

}

