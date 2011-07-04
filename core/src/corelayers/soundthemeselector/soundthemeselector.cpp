#include "soundthemeselector.h"
#include "ui_soundthemeselector.h"
#include <qutim/sound.h>
#include <QStandardItemModel>
#include <qutim/localizedstring.h>
#include <qutim/icon.h>
#include <QAbstractItemDelegate>
#include <QMetaEnum>

namespace Core
{
using namespace qutim_sdk_0_3;


SoundThemeSelector::SoundThemeSelector() :
	ui(new Ui::SoundThemeSelector),
	m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	connect(ui->themeSelector, SIGNAL(currentIndexChanged(QString)), SLOT(currentIndexChanged(QString)));
	ui->treeView->setModel(m_model);
	connect(ui->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClicked(QModelIndex)));
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
	ui->treeView->resizeColumnToContents(1);
}

void SoundThemeSelector::fillModel(const SoundTheme &theme)
{
	QStringList headers;
	headers.append(QT_TRANSLATE_NOOP("Notifications", "Type"));
	headers.append(QT_TRANSLATE_NOOP("SoundTheme", "Preview"));
	m_model->setHorizontalHeaderLabels(headers);

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QList<QStandardItem *> items;

		QStandardItem *item = new QStandardItem(Notification::typeString(type));
		item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Type"));
		item->setSelectable(false);
		items << item;

		item = new QStandardItem();
		item->setEditable(false);
		item->setIcon(Icon("media-playback-start"));
		item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Play"));
		item->setEnabled(!theme.path(type).isNull());
		item->setSelectable(false);
		items << item;

		m_model->appendRow(items);
	}
	ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
}

void SoundThemeSelector::onClicked(const QModelIndex &index)
{
	if (index.column() != 1 || !(index.flags() & Qt::ItemIsEnabled))
		return;

	SoundTheme theme =  Sound::theme(ui->themeSelector->currentText());
	theme.play(static_cast<Notification::Type>(index.row()));
}

}
