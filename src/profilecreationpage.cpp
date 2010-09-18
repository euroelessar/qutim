#include "profilecreationpage.h"
#include "ui_profilecreationpage.h"
#include "libqutim/cryptoservice.h"
#include "libqutim/objectgenerator.h"
#include "libqutim/extensioninfo.h"
#include <QDebug>
#include <QStringBuilder>
#include <QCryptographicHash>
#include "libqutim/json.h"
#include "libqutim/systeminfo.h"
#include "libqutim/config.h"

using namespace qutim_sdk_0_3;
namespace qutim_sdk_0_3
{
	LIBQUTIM_EXPORT QVector<QDir> *system_info_dirs();
	LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();
}

namespace Core
{
ProfileCreationPage::ProfileCreationPage(const QString &password, bool singleProfile, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::ProfileCreationPage), m_singleProfile(singleProfile), m_is_valid(false)
{
    ui->setupUi(this);
	m_password = password;
	registerField("name", ui->nameEdit);
	registerField("id", ui->idEdit);
	registerField("portable", ui->portableBox);
	registerField("configDir", ui->configEdit);
	registerField("historyDir", ui->historyEdit);
	registerField("dataDir", ui->dataEdit);
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
	Q_ASSERT(ui->cryptoBox->count() > 0);
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
	Q_ASSERT(ui->configBox->count() > 0);
	if (m_singleProfile)
		rebaseDirs();
}

ProfileCreationPage::~ProfileCreationPage()
{
    delete ui;
}

bool ProfileCreationPage::validatePage()
{
	//FIXME Elessar, WTF? why the generators are run on several times? 
	if (m_is_valid)
		return true; //dummy
	QDir dir;
	dir.mkpath(ui->configEdit->text());
	dir.mkpath(ui->historyEdit->text());
	dir.mkpath(ui->dataEdit->text());
	QVector<QDir> &systemDirs = *system_info_dirs();
	systemDirs[SystemInfo::ConfigDir] = QDir::cleanPath(ui->configEdit->text());
	systemDirs[SystemInfo::HistoryDir] = QDir::cleanPath(ui->historyEdit->text());
	systemDirs[SystemInfo::ShareDir] = QDir::cleanPath(ui->dataEdit->text());
	QFile file(SystemInfo::getDir(SystemInfo::ConfigDir).absoluteFilePath("profilehash"));
	if (file.exists() || !file.open(QIODevice::WriteOnly))
		return false;
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
	for (int i = 0; i < ui->configBox->count(); i++) {
		ExtensionInfo extInfo = ui->configBox->itemData(i).value<ExtensionInfo>();
		ConfigBackend *backend = extInfo.generator()->generate<ConfigBackend>();
		if (i == ui->configBox->currentIndex())
			configBackends.prepend(backend);
		else
			configBackends.append(backend);
	}
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
	QString profiles = QLatin1String(m_singleProfile ? "" : "profiles/");
	if (!m_singleProfile)
		profiles += ui->idEdit->text();
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
}
