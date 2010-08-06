#include "profiledialog.h"
#include "profilelistwidget.h"
#include "ui_profiledialog.h"
#include "libqutim/systeminfo.h"
#include "libqutim/json.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "profilecreationwizard.h"
#include "libqutim/objectgenerator.h"
#include "libqutim/jsonfile.h"
#include "libqutim/cryptoservice.h"
#include "libqutim/config.h"
#include "libqutim/icon.h"
#include <QCryptographicHash>
#include <QTimer>
#include <QScrollBar>

namespace qutim_sdk_0_3
{ 
	LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs(); 
	LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();
}

namespace Core
{
ProfileDialog::ProfileDialog(Config &config, ModuleManager *parent) :
    ui(new Ui::ProfileDialog)
{
	m_manager = parent;
	ui->setupUi(this);

	connect(ui->profileList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChanged(QListWidgetItem *, QListWidgetItem *)));

	QString current = config.value("current", QString());
	int size = config.beginArray("list");
	int itemHeight = 70;

	ui->profileList->setGridSize(QSize(0, itemHeight));
	ui->profileList->setFrameStyle(QFrame::NoFrame);
	ui->profileList->setMinimumSize(ui->profileList->minimumSize().width(), itemHeight);
	
	if (size == 0) {
		// TODO
	} else {
		int cid = 0;
		for (int i = 0; i < size; i++) {
			Config group = config.arrayElement(i);

			QListWidgetItem *item = new QListWidgetItem(ui->profileList);
			item->setSizeHint(QSize(0, itemHeight));
			item->setData(Qt::UserRole + 1, qVariantFromValue(group));

			QString id = group.value("id", QString());
			if (id == current)
				cid = ui->profileList->count() - 1;
			ProfileListWidget *w = new ProfileListWidget(id, group.value("configDir", QString()));
			connect(w, SIGNAL(submit(const QString &)), this, SLOT(login(const QString &)));
			w->setMinimumSize(w->minimumSize().width(), itemHeight);
			ui->profileList->setItemWidget(item, w);
		}
		ui->profileList->setCurrentRow(cid);
	}
	config.endArray();

	// Temporary
	ui->profilesButton->setText("Add profile");
}

ProfileDialog::~ProfileDialog()
{
    delete ui;
}

Config ProfileDialog::profilesInfo()
{
	QFileInfo profilesInfo(ProfilesConfigPath());
	if (!profilesInfo.exists() || !profilesInfo.isFile()) {
		return Config(QVariantMap());
	} else {
		JsonFile file(profilesInfo.absoluteFilePath());
		QVariant var;
		file.load(var);
		return Config(var.toMap());
	}
}

QString ProfileDialog::ProfilesConfigPath()
{
	QDir dir = qApp->applicationDirPath();
	if (!dir.exists("profiles") || !dir.cd("profiles")) {
#if defined(Q_OS_WIN)
		dir = QString::fromLocal8Bit(qgetenv("APPDATA"));
#elif defined(Q_OS_MAC)
		dir = QDir::home().absoluteFilePath("Library/Application Support");
#elif defined(Q_OS_UNIX)
		dir = QDir::home().absoluteFilePath(".config");
#else
# Undefined OS
#endif
		dir.mkpath("qutim/profiles");
		dir.cd("qutim/profiles");
	}
	return dir.filePath("profiles.json");
}

bool ProfileDialog::acceptProfileInfo(Config &config, const QString &password)
{
	QString crypto = config.value("crypto", QString());
	GeneratorList gens = moduleGenerators<CryptoService>();
	CryptoService *service = 0;
	foreach (const ObjectGenerator *gen, gens) {
		if (QLatin1String(gen->metaObject()->className()) == crypto) {
			service = gen->generate<CryptoService>();
			break;
		}
	}

	QString configDir = config.value("configDir", QString());
	QFile file(configDir + "/profilehash");
	if (service && file.open(QIODevice::ReadOnly)) {
		service->setPassword(password, QVariant());
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
		if (id != config.value("id", QString()))
			qCritical("Wrong profile id");
		if (passwordHash != hash
			|| QLatin1String(cryptoCheck) != crypto
			|| id != config.value("id", QString())) {
			delete service;
			return false;
		}

		QVector<QDir> &systemDirs = *system_info_dirs();
		if (config.value("portable", false)) {
			QDir dir = qApp->applicationDirPath();
			systemDirs[SystemInfo::ConfigDir] = dir.absoluteFilePath(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = dir.absoluteFilePath(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = dir.absoluteFilePath(config.value("shareDir", QString()));
		} else {
			systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(config.value("configDir", QString()));
			systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(config.value("historyDir", QString()));
			systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(config.value("shareDir", QString()));
		}

		QString configName = config.value("config", QString());
		QList<ConfigBackend*> &configBackends = get_config_backends();
		foreach (const ObjectGenerator *gen, moduleGenerators<ConfigBackend>()) {
			const ExtensionInfo info = gen->info();
			ConfigBackend *backend = info.generator()->generate<ConfigBackend>();
			if (configName == QLatin1String(backend->metaObject()->className()))
				configBackends.prepend(backend);
			else
				configBackends.append(backend);
		}

		return true;
	} else {
		qCritical("Can't open file with hash");
		delete service;
		return false;
	}
}

void ProfileDialog::login(const QString &password)
{
	QVariant variant = ui->profileList->currentItem()->data(Qt::UserRole + 1);
	Config config = variant.value<Config>();
	JsonFile file(ProfilesConfigPath());
	QVariant var;
	QVariantMap varMap;
	file.load(var);
	varMap = var.toMap();
	Config(&varMap).setValue("current", config.value("id", QString()));
	file.save(varMap);
	if (acceptProfileInfo(config, password)) {
		QTimer::singleShot(0, m_manager, SLOT(initExtensions()));
		deleteLater();
	}
}

void ProfileDialog::on_profilesButton_clicked()
{
	QString name = QInputDialog::getText(this, tr("Enter name"), tr("Profile name:"), QLineEdit::Normal);
	if (name.isEmpty()) {
		QMessageBox::critical(this, tr("Invalid name"), tr("Name can not be empty!"));
		return;
	}

	QString pass = QInputDialog::getText(this, tr("Enter password"), tr("Password:"), QLineEdit::Password);
	QString passr = QInputDialog::getText(this, tr("Repeat password"), tr("Repeat password:"), QLineEdit::Password);

	if (pass != passr) {
		QMessageBox::critical(this, tr("Incorrect password"), tr("Passwords doesn't match each other"));
		return;
	}

	if (pass.isEmpty()) {
		QMessageBox::critical(this, tr("Incorrect password"), tr("Password can not be empty!"));
		return;
	}

	QWizard *wizard = new ProfileCreationWizard(m_manager, name, pass);
#if	defined(Q_OS_SYMBIAN)
	wizard->showMaximized();
#else
	wizard->show();
#endif
	connect(wizard, SIGNAL(accepted()), this, SLOT(deleteLater()));
	connect(wizard, SIGNAL(rejected()), this, SLOT(show()));
	hide();
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

void ProfileDialog::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous)
		((ProfileListWidget*)ui->profileList->itemWidget(previous))->activate(false);
	if (current)
		((ProfileListWidget*)ui->profileList->itemWidget(current))->activate(true);
}
}
