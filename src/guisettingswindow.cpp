/*
    GuiSetttingsWindow

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QApplication>
#include <QDesktopWidget>
#include "guisettingswindow.h"
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QStringList>
#include "qutim.h"
//#include "abstractchatlayer.h"
#include "abstractlayer.h"
#include "abstractcontextlayer.h"


class Resource: public QResource
{
	public:
	using QResource::children;
	using QResource::isFile;
	using QResource::isDir;
	Resource() : QResource(){}
};

GuiSetttingsWindow::GuiSetttingsWindow(const QString &profile_name, QWidget *parent)
	: QDialog(parent)
    , m_profile_name(profile_name)
{
	ui.setupUi(this);
	setMinimumSize(size());
	setAttribute(Qt::WA_QuitOnClose, false); //don't close app on "Settings" exit
	setAttribute(Qt::WA_DeleteOnClose, true);
	PluginSystem::instance().centerizeWidget(this);
	addUiContent();
	loadSettings();
	connect(ui.emoticonsComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.chatComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.webkitComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.popupComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.systemIconComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.statusIconComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.listThemeComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.languageBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.styleBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));
	connect(ui.borderComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(enableApplyButton()));


}

GuiSetttingsWindow::~GuiSetttingsWindow()
{
	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
		{
			layer_interface->removeGuiLayerSettings();
		}
	}
}

QPoint GuiSetttingsWindow::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void GuiSetttingsWindow::enableApplyButton()
{
	ui.applyButton->setEnabled(true);
}

void GuiSetttingsWindow::addUiContent()
{
	ui.emoticonsComboBox->addItem("Tango",":/style/emoticons/emoticons.xml");
	ui.listThemeComboBox->addItem("qutim",":/style/cl/default.ListQutim");
	ui.borderComboBox->addItem("qutim",":/style/border/");
	ui.popupComboBox->addItem("qutim0.2",":/style/traytheme");
	ui.styleBox->addItem("qutim0.2",":/style/style/qutim.qss");

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");

	QStringList paths = PluginSystem::instance().getSharePaths();
	qDebug() << paths;

	addWebkitThemes("");
	foreach(QString path, paths)
	{
		QDir dir( path );
		QString emoticons_path = dir.filePath( "emoticons" );
		QString chat_path = dir.filePath( "chatstyle" );
		QString webkit_path = dir.filePath( "webkitstyle" );
		QString popup_path = dir.filePath( "traythemes" );
		QString status_path = dir.filePath( "statusicons" );
		QString system_path = dir.filePath( "systemicons" );
		QString contact_list_theme_path = dir.filePath( "clstyles" );
		QString languages_path = dir.filePath( "languages" );
		QString application_styles_path = dir.filePath( "styles" );
		QString border_themes_path =dir.filePath( "borders" );
		addEmoticonThemes(emoticons_path);
		addListThemes(contact_list_theme_path);
		addChatThemes(chat_path);
		addWebkitThemes(webkit_path);
		addPopupThemes(popup_path);
		addStatusThemes(status_path);
		addSystemThemes(system_path);
		addLanguages(languages_path);
		addApplicationStyles(application_styles_path);
		addBorderThemes(border_themes_path);
	}
}

void GuiSetttingsWindow::addLanguages(const QString &path)
{
	QDir lang_dir(path);
	QStringList themes(lang_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, themes)
	{
		QString name = dirName;
		QLocale locale( name );
		QLocale::Country c = locale.country();
		QLocale::Language l = locale.language();
		if(l != QLocale::C)
		{
			name = QLocale::languageToString(l);
//			if(c != QLocale::AnyCountry)
//				name.append(" (").append(QLocale::countryToString(c)).append(")");
		}
		ui.languageBox->addItem(name, path + "/" + dirName);
	}
}

void GuiSetttingsWindow::addApplicationStyles(const QString &path)
{
	QDir style_path(path);
	QStringList themes(style_path.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));
	foreach(QString dirName, themes)
	{
		QDir dir(path + "/" + dirName);
		QStringList filter;
		filter << "*.qss";
		QStringList file_qss = dir.entryList(filter,QDir::Files);
		if (file_qss.count())
		{
			ui.styleBox->addItem(dirName, dir.path() + "/" + file_qss.at(0));
		}
	}
}

void GuiSetttingsWindow::addEmoticonThemes(const QString &path)
{
	QDir emoticonPath(path);
	QStringList themes(emoticonPath.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));
	foreach(QString dirName, themes)
	{
		QDir dir(path + "/" + dirName);
		QStringList filter;
		filter << "*.xml";
		QStringList fileXML = dir.entryList(filter,QDir::Files);
		if (fileXML.count())
		{
			ui.emoticonsComboBox->addItem(dirName, dir.path() + "/" + fileXML.at(0));
		}
	}
}

void GuiSetttingsWindow::addListThemes(const QString &path)
{
	QDir list_dir(path);
	QStringList filters;
	filters << "*.ListTheme" << "*.ListQutim";
	
	QStringList dirs_with_themes(list_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));//.entryList(filters, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files));
	foreach(QString dir_with_themes, dirs_with_themes)
	{
		QDir dir(path+"/"+dir_with_themes);
		QStringList themes(dir.entryList(filters, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files));
		foreach(QString dir_name, themes)
		{
			ui.listThemeComboBox->addItem(dir_name.left(dir_name.lastIndexOf('.'))+(dir_name.endsWith(".ListTheme")?" (Adium Style)":" (qutIM Style)"), path+"/"+dir_with_themes+"/"+dir_name);
		}
	}
}

void GuiSetttingsWindow::addChatThemes(const QString &path)
{
	QDir chat_dir(path);
	QStringList themes(chat_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, themes)
	{
		ui.chatComboBox->addItem(dirName, path + "/" + dirName);
	}
}

void GuiSetttingsWindow::addWebkitThemes(const QString &path)
{
	if(path.isEmpty())
	{
		Resource variantDir;

                variantDir.setFileName("style/webkitstyle/Variants/");

		if(!variantDir.isDir())
			return;
		ui.webkitComboBox->clear();
                QStringList variantList = variantDir.children();
		foreach(QString variantName, variantList)
		{
			if(variantName.endsWith(".css"))
			{
				QString variantPath = "Variants/"+variantName;
				variantName = variantName.left(variantName.lastIndexOf("."));
				QStringList params;
				params<<""<<variantName;
				ui.webkitComboBox->addItem("Default (" + variantName + ")", params);
			}
		}
		return;
	}
	QDir chat_dir(path);
	QStringList themes(chat_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, themes)
	{
		QString theme_path = path + "/" + dirName + "/Contents/Resources/";
		QDir variant_dir(theme_path + "/Variants");
		QStringList variants(variant_dir.entryList(QDir::Files | QDir::NoDotAndDotDot));
		QSettings settings(path + "/" + dirName + "/Contents/Info.plist",PluginSystem::instance().plistFormat());
		QString style_name = settings.value("CFBundleName",dirName).toString();
		if(settings.value("MessageViewVersion").toInt()<3 && settings.contains("DisplayNameForNoVariant"))
		{
			QStringList params;
			params<<theme_path<<"";
			ui.webkitComboBox->addItem(style_name + " (" + settings.value("DisplayNameForNoVariant").toString() + ")", params);
		}
		if ( variants.count() )
		{
			foreach(QString variant, variants)
			{
				QStringList params;
				QFileInfo file_info(variant);
				params<<theme_path<<file_info.completeBaseName();
				ui.webkitComboBox->addItem(style_name + " (" + file_info.completeBaseName() + ")", params);
			}
		}
		else if(!settings.contains("DisplayNameForNoVariant"))
		{
			QStringList params;
			params<<theme_path<<"";
			ui.webkitComboBox->addItem(style_name, params);
		}
	}
}

void GuiSetttingsWindow::addStatusThemes(const QString &path)
{
	QDir chat_dir(path);
	QStringList themes(chat_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));
	
	foreach(QString dir_name, themes)
	{
		if(dir_name.indexOf(".")>-1)
			ui.statusIconComboBox->addItem(dir_name.left(dir_name.lastIndexOf('.')),path+"/"+dir_name);
		else
			ui.statusIconComboBox->addItem(dir_name, path+"/"+dir_name);
	
	}
}

void GuiSetttingsWindow::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("gui");
	int emoticon_index = ui.emoticonsComboBox->findData(settings.value("emoticons",":/style/emoticons/emoticons.xml").toString());
	ui.emoticonsComboBox->setCurrentIndex(emoticon_index > -1 ?emoticon_index:0);
	QString list_theme_path = settings.value("listtheme",":/style/cl/default.ListQutim").toString();
	int list_theme_index = ui.listThemeComboBox->findData(list_theme_path);
	ui.listThemeComboBox->setCurrentIndex(list_theme_index > -1 ? list_theme_index : list_theme_path.isEmpty() ? 0 : 1);
	int chat_index = ui.chatComboBox->findData(settings.value("chat","").toString());
	ui.chatComboBox->setCurrentIndex(chat_index > -1 ?chat_index:0);
	int popup_index = ui.popupComboBox->findData(settings.value("popup",":/style/traytheme").toString());
	ui.popupComboBox->setCurrentIndex(popup_index > -1 ?popup_index:0);
	int system_index = ui.systemIconComboBox->findData(settings.value("systemicon","").toString());
	ui.systemIconComboBox->setCurrentIndex(system_index > -1 ?system_index:0);
	int status_index = ui.statusIconComboBox->findData(settings.value("statusicon","").toString());
	ui.statusIconComboBox->setCurrentIndex(status_index > -1 ?status_index:0);
	QStringList webkit_param;
	if(settings.contains("wstyle") && settings.contains("wvariant"))
		webkit_param << settings.value("wstyle","").toString() << settings.value("wvariant","").toString();
	else
		webkit_param << "" << "red";
	int webkit_index = ui.webkitComboBox->findData(webkit_param); 
	ui.webkitComboBox->setCurrentIndex(webkit_index > -1 ?webkit_index:0);
	int lang_index = ui.languageBox->findData(AbstractLayer::instance().getParent()->translationPath());
	ui.languageBox->setCurrentIndex(lang_index > -1 ?lang_index:0);

#if defined (Q_OS_WIN32)
        int border_index = ui.borderComboBox->findData(settings.value("borders",":/style/border/").toString());
#else
        int border_index = ui.borderComboBox->findData(settings.value("borders","").toString());
#endif


#if defined(Q_OS_WIN32)
int style_index = ui.styleBox->findData(settings.value("style",":/style/style/qutim.qss").toString());
#else
int style_index = ui.styleBox->findData(settings.value("style","").toString());
#endif

        ui.styleBox->setCurrentIndex(style_index > -1 ?style_index:0);
	ui.borderComboBox->setCurrentIndex(border_index > -1 ?border_index:0);
	settings.endGroup();
	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
		{
			layer_interface->getGuiSettingsList();
		}
	}
}

void GuiSetttingsWindow::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("gui");
	settings.setValue("emoticons", ui.emoticonsComboBox->itemData(ui.emoticonsComboBox->currentIndex()).toString());
	settings.setValue("listtheme", ui.listThemeComboBox->itemData(ui.listThemeComboBox->currentIndex()).toString());
	settings.setValue("chat", ui.chatComboBox->itemData(ui.chatComboBox->currentIndex()).toString());
	settings.setValue("popup", ui.popupComboBox->itemData(ui.popupComboBox->currentIndex()).toString());
	settings.setValue("systemicon", ui.systemIconComboBox->itemData(ui.systemIconComboBox->currentIndex()).toString());
	settings.setValue("statusicon", ui.statusIconComboBox->itemData(ui.statusIconComboBox->currentIndex()).toString());
	QStringList webkit_param = ui.webkitComboBox->itemData(ui.webkitComboBox->currentIndex()).toStringList();
	if ( webkit_param.count() )
	{
		settings.setValue("wstyle", webkit_param.at(0));
		settings.setValue("wvariant", webkit_param.at(1));
	}
	else
	{
		settings.setValue("wstyle", "");
		settings.setValue("wvariant", "");
	}
	settings.setValue("language", ui.languageBox->itemData(ui.languageBox->currentIndex()).toString());
	settings.setValue("style", ui.styleBox->itemData(ui.styleBox->currentIndex()).toString());
	settings.setValue("borders", ui.borderComboBox->itemData(ui.borderComboBox->currentIndex()).toString());
	settings.endGroup();
	reloadContent();
	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *layer_interface = PluginSystem::instance().getLayerInterface(static_cast<LayerType>(type));
		if(layer_interface)
		{
			layer_interface->saveGuiSettingsPressed();
		}
	}
}

void GuiSetttingsWindow::on_applyButton_clicked()
{
	saveSettings();
	ui.applyButton->setEnabled(false);
}

void GuiSetttingsWindow::on_okButton_clicked()
{
	if ( ui.applyButton->isEnabled() )
	{
		saveSettings();
	}
	close();
}

void GuiSetttingsWindow::reloadContent()
{
//	AbstractChatLayer &acl = AbstractChatLayer::instance();
//        acl.reloadContent();
//	AbstractContactList::instance().loadGuiSettings();
	AbstractLayer::instance().reloadStyleLanguage();
        AbstractContextLayer::instance().createActions();
}

void GuiSetttingsWindow::addPopupThemes(const QString &path)
{
	QDir popup_dir(path);
	QStringList themes(popup_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, themes)
	{
		ui.popupComboBox->addItem(dirName, path + "/" + dirName);
	}
}
void GuiSetttingsWindow::addSystemThemes(const QString &path)
{
	QDir popup_dir(path);
	QStringList themes(popup_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, themes)
	{
		ui.systemIconComboBox->addItem(dirName, path + "/" + dirName);
	}	
}

void GuiSetttingsWindow::addBorderThemes(const QString &path)
{
	QDir border_dir(path);
	QStringList borders(border_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	foreach(QString dirName, borders)
	{
		ui.borderComboBox->addItem(dirName, path + "/" + dirName);
	}
}
