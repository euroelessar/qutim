#include "soundthemeselector.h"
#include "ui_soundthemeselector.h"
#include <libqutim/notificationslayer.h>
#include <QStandardItemModel>
#include <libqutim/localizedstring.h>
#include <libqutim/icon.h>
#include <QAbstractItemDelegate>

namespace Core
{
	using namespace qutim_sdk_0_3;


	SoundThemeSelector::SoundThemeSelector() :
			ui(new Ui::SoundThemeSelector),
			m_model(new QStandardItemModel(this))
	{
		ui->setupUi(this);
		connect(ui->themeSelector,SIGNAL(currentIndexChanged(QString)),SLOT(currentIndexChanged(QString)));
		ui->treeView->setModel(m_model);
		connect(ui->treeView,SIGNAL(clicked(QModelIndex)),SLOT(onClicked(QModelIndex)));
	}

	SoundThemeSelector::~SoundThemeSelector()
	{
		delete ui;
	}

	void SoundThemeSelector::changeEvent(QEvent *e)
	{
		QWidget::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
	
	void SoundThemeSelector::loadImpl()
	{
		QStringList theme_list =  Sound::themeList();
		ui->themeSelector->addItems(theme_list);
		int index = ui->themeSelector->findText(Sound::currentThemeName());
		ui->themeSelector->setCurrentIndex(index);
	}
	void SoundThemeSelector::saveImpl()
	{
		Sound::setTheme(ui->themeSelector->currentText());
	}
	void SoundThemeSelector::cancelImpl()
	{

	}
	
	void SoundThemeSelector::currentIndexChanged(const QString& text)
	{
		emit modifiedChanged(true);
		m_model->clear();
		fillModel(Sound::theme(text));
		ui->treeView->resizeColumnToContents(0);
		ui->treeView->resizeColumnToContents(1);
		ui->treeView->resizeColumnToContents(2);
	}

	void SoundThemeSelector::fillModel(const SoundTheme &theme)
	{
		QStringList headers;
		headers.append(QT_TRANSLATE_NOOP("Notifications", "Type"));
		headers.append(QT_TRANSLATE_NOOP("SoundTheme", "Filename"));
		headers.append(QT_TRANSLATE_NOOP("SoundTheme", "Preview"));
		m_model->setHorizontalHeaderLabels(headers);

		//TODO move to libqutim
		LocalizedString strings[] =
		{
			QT_TRANSLATE_NOOP("Notifications", "User online"),
			QT_TRANSLATE_NOOP("Notifications", "User offline"),
			QT_TRANSLATE_NOOP("Notifications", "Status changed"),
			QT_TRANSLATE_NOOP("Notifications", "User has birthday today!"),
			QT_TRANSLATE_NOOP("Notifications", "qutIM Launched"),
			QT_TRANSLATE_NOOP("Notifications", "Message recieved"),
			QT_TRANSLATE_NOOP("Notifications", "Message sent"),
			QT_TRANSLATE_NOOP("Notifications", "System notification"),
			QT_TRANSLATE_NOOP("Notifications", "Typing"),
			QT_TRANSLATE_NOOP("Notifications", "Message blocked"),
			QT_TRANSLATE_NOOP("Notifications", "Count")
		};

		for (int i = 0, size = sizeof(strings)/sizeof(LocalizedString); i < size; i++) {
			Notifications::Type type = static_cast<Notifications::Type>(1 << i);
			QList<QStandardItem *> items;

			QStandardItem *item = new QStandardItem(strings[i]);
			item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Type"));
			items << item;

			QString path = theme.path(type);

			item = new QStandardItem(path.section("/",-1));
			item->setToolTip(path);
			items << item;

			item = new QStandardItem();
			item->setEditable(false);
			item->setIcon(Icon("media-playback-start"));
			item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Play"));
			item->setEnabled(!theme.path(type).isNull());
			item->setData(Qt::UserRole,type);
			items << item;

			m_model->appendRow(items);
		}

	}

	void SoundThemeSelector::onClicked(const QModelIndex &index)
	{
		if (!index.data(Qt::ItemIsEnabled).toBool())
			return;
		int type = index.data(Qt::UserRole).value<int>();
		if (type) {
			SoundTheme theme =  Sound::theme(ui->themeSelector->currentText());
			theme.play(static_cast<Notifications::Type>(type));
		}
	}

}
