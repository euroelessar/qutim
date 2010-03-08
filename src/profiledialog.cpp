#include "profiledialog.h"
#include "ui_profiledialog.h"
#include "libqutim/systeminfo.h"
#include "libqutim/json.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include "profilecreationwizard.h"
#include "libqutim/objectgenerator.h"
#include "libqutim/cryptoservice.h"
#include "libqutim/configbase_p.h"
#include <QCryptographicHash>
#include <QTimer>

namespace qutim_sdk_0_3
{ LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); }

namespace Core
{
ProfileDialog::ProfileDialog(ModuleManager *parent) :
    ui(new Ui::ProfileDialog)
{
	m_manager = parent;
	ui->setupUi(this);
	QDir dir = qApp->applicationDirPath();
	if (dir.exists("profiles")) {
		dir.cd("profiles");
	} else {
		dir = QDir::home();
		dir.mkpath(".config/qutim/profiles");
		dir.cd(".config/qutim/profiles");
	}
	QFileInfo profilesInfo(dir.filePath("profiles.json"));
	if (!profilesInfo.exists() || !profilesInfo.isFile()) {
		ui->toolBox->setCurrentIndex(1);
		ui->toolBox->setItemEnabled(0, false);
	} else {
		QFile file(profilesInfo.absoluteFilePath());
		file.open(QIODevice::ReadOnly);
		QVariantMap value = Json::parse(file.readAll()).toMap();
//		QString last = value.value("last").toString();
		foreach (QVariant var, value.value("list").toList()) {
			QVariantMap map = var.toMap();
			ui->profilesBox->addItem(map.value("id").toString(), QVariant(map));
		}
	}
	setAttribute(Qt::WA_DeleteOnClose, true);
}

ProfileDialog::~ProfileDialog()
{
    delete ui;
}

void ProfileDialog::on_loginButton_clicked()
{
	QVariantMap map = ui->profilesBox->itemData(ui->profilesBox->currentIndex()).toMap();
	QString crypto = map.value("crypto").toString();
	GeneratorList gens = moduleGenerators<CryptoService>();
	CryptoService *service = 0;
	foreach (const ObjectGenerator *gen, gens) {
		if (QLatin1String(gen->metaObject()->className()) == crypto) {
			service = gen->generate<CryptoService>();
			break;
		}
	}

	QString configDir = map.value("configDir").toString();
	QFile file(configDir + "/profilehash");
	if (file.open(QIODevice::ReadOnly)) {
		QString password = ui->passwordEdit->text();
		service->setPassword(password);
		QByteArray data = service->decrypt(file.readAll()).toByteArray();
		QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8()
														   + "5667dd05fbe97bb238711a3af63",
														   QCryptographicHash::Sha1);
		QDataStream in(data);
		QString id;
		QByteArray hash;
		QByteArray cryptoCheck;
		in >> id >> hash >> cryptoCheck;
		if (passwordHash != hash)
			qCritical("Wrong password");
		if (QLatin1String(cryptoCheck) != crypto)
			qCritical("Wrong crypto service");
		if (id != map.value("id").toString())
			qCritical("Wrong profile id");
		if (passwordHash != hash
			|| QLatin1String(cryptoCheck) != crypto
			|| id != map.value("id").toString()) {
			delete service;
			return;
		}

		QVector<QDir> &systemDirs = *system_info_dirs();
		if (map.value("portable").toBool()) {
			QDir dir = qApp->applicationDirPath();
			systemDirs[SystemInfo::ConfigDir] = dir.absoluteFilePath(map.value("configDir").toString());
			systemDirs[SystemInfo::HistoryDir] = dir.absoluteFilePath(map.value("historyDir").toString());
			systemDirs[SystemInfo::ShareDir] = dir.absoluteFilePath(map.value("shareDir").toString());
		} else {
			systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(map.value("configDir").toString());
			systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(map.value("historyDir").toString());
			systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(map.value("shareDir").toString());
		}

		QString config = map.value("config").toString();
		foreach (const ObjectGenerator *gen, moduleGenerators<ConfigBackend>()) {
			const ExtensionInfo info = gen->info();
			ConfigBackend *backend = info.generator()->generate<ConfigBackend>();
			ConfigBackendInfo back = ConfigBackendInfo(metaInfo(backend->metaObject(),
																"Extension"), backend);
			if (config == QLatin1String(backend->metaObject()->className()))
				ConfigPrivate::config_backends.prepend(back);
			else
				ConfigPrivate::config_backends.append(back);
		}

		QTimer::singleShot(0, m_manager, SLOT(initExtensions()));
		deleteLater();
	} else {
		qCritical("Can't open file with hash");
		delete service;
	}
}

void ProfileDialog::on_createButton_clicked()
{
	if (ui->originalPasswordEdit->text() != ui->repeatPasswordEdit->text()) {
		QMessageBox::critical(this, tr("Incorrect password"), tr("Passwords doesn't match each other"));
	} else {
		QWidget *wizard = new ProfileCreationWizard(m_manager, ui->nameEdit->text(),
													ui->originalPasswordEdit->text());
		wizard->show();
		deleteLater();
	}
}

void ProfileDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
}
