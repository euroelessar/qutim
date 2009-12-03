#include "emoticonsselector.h"
#include "ui_emoticonsselector.h"
#include <libqutim/emoticons.h>
#include <qmovie.h>
#include <QLabel>

EmoticonsSelector::EmoticonsSelector() : ui(new Ui::emoticonsSelector)
{
	ui->setupUi(this);
	connect(ui->themeSelector,SIGNAL(currentIndexChanged(QString)),SLOT(currentIndexChanged(QString)));
}

EmoticonsSelector::~EmoticonsSelector()
{
	delete ui;
}

void EmoticonsSelector::loadImpl()
{
	 QStringList theme_list =  Emoticons::themeList();
	 ui->themeSelector->addItems(theme_list);
	 int index = ui->themeSelector->findText(Emoticons::currentThemeName());
	 ui->themeSelector->setCurrentIndex(index);
}

void EmoticonsSelector::cancelImpl()
{

}

void EmoticonsSelector::saveImpl()
{

}

void EmoticonsSelector::currentIndexChanged(const QString& text)
{
	QHash<QString, QStringList> theme_map = Emoticons::theme(text).emoticonsMap();
	QHash<QString, QStringList>::const_iterator it;
	clearEmoticonsPreview();
	int i,j = 0; //HACK
	for (it = theme_map.constBegin();it != theme_map.constEnd();it ++) {
		QMovie *emoticon = new QMovie (it.key());
		QLabel *label = new QLabel();
		label->setMovie(emoticon);
		ui->emoticonsLayout->addWidget(label,i,j);
		m_active_emoticons.append(label);
		emoticon->start();
		j++;
		if (j>8) {
			j = 0;
			i++;
		}
	}
	
}

void EmoticonsSelector::clearEmoticonsPreview()
{
	for (int i = 0;i != m_active_emoticons.count();i ++) {
		m_active_emoticons.at(i)->deleteLater();
	}
	m_active_emoticons.clear();
}
