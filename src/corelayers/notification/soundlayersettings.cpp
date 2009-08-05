/*
    SoundSettings

    Copyright (c) 2008-2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
                       2008 by Denis Novikov <Denisss>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QtGui>
#include <QSound>
#include <QtXml>

#include "soundlayersettings.h"
#include "src/abstractlayer.h"

SoundLayerSettings::SoundLayerSettings(const QString &profile_name, QWidget *parent)
: QWidget(parent), changed(false)
{
	m_profile_name = profile_name;
	ui.setupUi(this);

	// Filling events table (tree widget)
        appendEvent(tr("Startup"), NotifyStartup);
        appendEvent(tr("System event"), NotifySystem);
        appendEvent(tr("Outgoing message"), NotifyMessageSend);
        appendEvent(tr("Incoming message"), NotifyMessageGet);
        appendEvent(tr("Contact is online"), NotifyOnline);
        appendEvent(tr("Contact changed status"), NotifyStatusChange);
        appendEvent(tr("Contact went offline"), NotifyOffline);
        appendEvent(tr("Contact's birthday is comming"), NotifyBirthday);

	//ui.playOnlyTree->setEnabled(false);

	// Filling systemCombo
//	ui.systemCombo->setItemData(0, qutim_sdk_0_2::NoSound);
//#ifdef HAVE_PHONON
//	ui.systemCombo->addItem(tr("Phonon"), qutim_sdk_0_2::LibPhonon);
//#endif
//#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
//        ui.systemCombo->addItem(tr("QSound"), qutim_sdk_0_2::LibSound);
//#endif
//	ui.systemCombo->addItem(tr("Command"), qutim_sdk_0_2::UserCommand);
//	ui.systemCombo->addItem(tr("Plugin"), qutim_sdk_0_2::PluginEngine);
//
//	// disabling events tab
//	//ui.eventsTab->setEnabled(false);
//
//	// connections
//	connect(ui.commandEdit, SIGNAL(textChanged(const QString &)),
//		this, SLOT(widgetStateChanged()));
	loadSettings();
}

inline
void SoundLayerSettings::appendEvent(const QString &eventName,
        const NotificationType event)
{
	static const Qt::ItemFlags itemFlags =
		  Qt::ItemIsSelectable
		| Qt::ItemIsUserCheckable
		| Qt::ItemIsEnabled;

	QTreeWidgetItem *newItem = new QTreeWidgetItem(ui.eventsTree);
	newItem->setFlags(itemFlags);
	newItem->setText(0, eventName);
	newItem->setCheckState(0, Qt::Unchecked);
	newItem->setData(0, Qt::UserRole, static_cast<int>(event));
	eventList << newItem;
}

void SoundLayerSettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		
	settings.beginGroup("sounds");
//	ui.systemCombo->setCurrentIndex(ui.systemCombo->findData(static_cast<qutim_sdk_0_2::SoundEngineSystem>(settings.value("soundengine",
//		0).toInt())));

	QString fileName;
	for (int i = 0; i < eventList.size(); i++)
	{
		fileName = settings.value("event"
			+ QString::number(eventList.at(i)->data(0, Qt::UserRole).toInt()),
			"-").toString();
		eventList.at(i)->setCheckState(0,
			(fileName.at(0) == '+') ? Qt::Checked : Qt::Unchecked);
		eventList.at(i)->setData(0, Qt::UserRole+1, fileName.mid(1));
	}

//#ifndef Q_OS_WIN32
//	ui.commandEdit->setText(settings.value("command", "play \"%1\"").toString());
//#else
//	ui.commandEdit->setText(settings.value("command", "").toString());
//#endif

	settings.endGroup();

	changed = false;
}

void SoundLayerSettings::saveSettings()
{
	// we do not save anything until something has been changed
	if (!changed) return;

	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
		
	settings.beginGroup("sounds");

//	QString command = ui.commandEdit->text().trimmed();
//	if (getCurrentSoundSystem() == qutim_sdk_0_2::UserCommand)
//	{
//		if (command.isEmpty())
//			ui.systemCombo->setCurrentIndex(0);
//		else if (!command.contains("%1")) command += " \"%1\"";
//		ui.commandEdit->setText(command);
//	}
//
//	settings.setValue("command", command);
//
//	settings.setValue("soundengine",
//		ui.systemCombo->itemData(ui.systemCombo->currentIndex()).toInt());

	int i;

	QString fileName;
	for (i = 0; i < eventList.size(); i++)
	{
		fileName = (eventList.at(i)->checkState(0) == Qt::Checked) ? "+" : "-";
		fileName += eventList.at(i)->data(0, Qt::UserRole+1).toString();
		settings.setValue("event"
			+ QString::number(eventList.at(i)->data(0,
			Qt::UserRole).toInt()), fileName);
	}

	settings.endGroup();

	changed = false;
	emit settingsSaved();
}

//void SoundLayerSettings::on_systemCombo_currentIndexChanged(int /* index */)
//{
//	qutim_sdk_0_2::SoundEngineSystem sys = getCurrentSoundSystem();
//
//	ui.commandEdit->setEnabled(false);
//	ui.commandButton->setEnabled(false);
//	if (sys != qutim_sdk_0_2::NoSound)
//	{
//		//ui.playOnlyTree->setEnabled(true);
//		//ui.eventsTab->setEnabled(true);
//		if (sys == qutim_sdk_0_2::UserCommand)
//		{
//			ui.commandEdit->setEnabled(true);
//			ui.commandButton->setEnabled(true);
//		}
//	}
//	else
//	{
//		//ui.playOnlyTree->setEnabled(false);
//		//ui.eventsTab->setEnabled(false);
//	}
//
//	widgetStateChanged();
//}
//
//void SoundLayerSettings::on_commandButton_clicked()
//{
//#ifdef Q_OS_WIN32
//	QString binName = QFileDialog::getOpenFileName(this,
//		tr("Select command path"), QDir::currentPath(),
//		tr("Executables (*.exe *.com *.cmd *.bat)\nAll files (*.*)"));
//#else
//	QString binName = QFileDialog::getOpenFileName(this,
//		tr("Select command path"), "/bin",
//		tr("Executables"));
//#endif // WIN32
//	if (binName.isEmpty()) return;
//
//#ifndef Q_OS_WIN32
//	ui.commandEdit->setText(binName + " \"%1\"");
//#else
//	ui.commandEdit->setText(QString("\"%1\" \"%2\"").arg(binName).arg("%1"));
//#endif // WIN32
//}

void SoundLayerSettings::on_eventsTree_currentItemChanged(QTreeWidgetItem *current,
	QTreeWidgetItem * /* previous */)
{
	if (!current) return;

	if (current->checkState(0) == Qt::Checked)
	{
		ui.fileEdit->setEnabled(true);
		ui.openButton->setEnabled(true);
	}
	else
	{
		ui.fileEdit->setEnabled(false);
		ui.openButton->setEnabled(false);
	}
	if (ui.fileEdit->text() == getCurrentFile())
		on_fileEdit_textChanged(getCurrentFile());
	else
		ui.fileEdit->setText(getCurrentFile());
}

void SoundLayerSettings::on_eventsTree_itemChanged(QTreeWidgetItem *item, int column)
{
	if (column) return;
	ui.fileEdit->setEnabled(item->checkState(0) == Qt::Checked);
	on_fileEdit_textChanged(ui.fileEdit->text());
	ui.openButton->setEnabled(ui.fileEdit->isEnabled());

	widgetStateChanged();
}

void SoundLayerSettings::on_fileEdit_textChanged(const QString &text)
{
	if (ui.fileEdit->isEnabled())
	{
		if (text.isEmpty())
		{
			ui.applyButton->setEnabled(true);
			ui.playButton->setEnabled(false);
		}
		else if (QFile::exists(text))
		{
			QTreeWidgetItem *item = ui.eventsTree->currentItem();
			if (item)
				ui.applyButton->setEnabled(getCurrentFile() != text);
			else
				ui.applyButton->setEnabled(false);

			ui.playButton->setEnabled(true);
		}
		else
		{
			ui.applyButton->setEnabled(false);
			ui.playButton->setEnabled(false);
		}
	}
	else
	{
		ui.applyButton->setEnabled(false);
		ui.playButton->setEnabled(false);
	}
}

void SoundLayerSettings::on_applyButton_clicked()
{
	ui.eventsTree->currentItem()->setData(0, Qt::UserRole+1, ui.fileEdit->text());
	ui.applyButton->setEnabled(false);

	widgetStateChanged();
}

void SoundLayerSettings::on_openButton_clicked()
{
	QString filter = tr("Sound files (*.wav);;All files (*.*)", "Don't remove ';' chars!");

	if (lastDir.isEmpty())
	{
		if (!ui.fileEdit->text().isEmpty() && QFile::exists(ui.fileEdit->text()))
			lastDir = ui.fileEdit->text().section('/', 0, -2);
		else
			lastDir = QDir::currentPath();
	}

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open sound file"), lastDir, filter);
	if (fileName.isEmpty()) return;

	ui.fileEdit->setText(fileName);
	if (getCurrentFile().isEmpty())
	{
		ui.applyButton->setEnabled(true);
		ui.applyButton->animateClick(500);
	}
	lastDir = fileName.section('/', 0, -2);
}

void SoundLayerSettings::on_playButton_clicked()
{
//	qutim_sdk_0_2::SoundEngineSystem sys = getCurrentSoundSystem();
//
//	if (sys == qutim_sdk_0_2::UserCommand)
//	{
//		QString command = ui.commandEdit->text().trimmed();
//		if (command.isEmpty())
//		{
//			ui.systemCombo->setCurrentIndex(0);
//			return;
//		}
//		if (!command.contains("%1"))
//		{
//			command += " \"%1\"";
//			ui.commandButton->setText(command);
//		}
//		AbstractSoundLayer::instance().testPlaySound(sys, command.arg(ui.fileEdit->text()));
//	}
//	else
//                AbstractSoundLayer::instance().testPlaySound(sys, ui.fileEdit->text());
	AbstractLayer::SoundEngine()->playSound(ui.fileEdit->text());
}

void SoundLayerSettings::on_exportButton_clicked()
{
	QString fileName(QFileDialog::getSaveFileName(this, tr("Export sound style"),
		lastDir, tr("XML files (*.xml)")));
	if (fileName.isEmpty()) return;

	if (!fileName.endsWith(".xml")) fileName += ".xml";

	QString exportDir = fileName.section('/', 0, -2);
	lastDir = exportDir;

	if (QMessageBox::question(this, tr("Export..."),
		tr("All sound files will be copied into \"%1/\" directory. " \
			"Existing files will be replaced without any prompts.\n" \
			"Do You want to continue?").arg(exportDir),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::No) != QMessageBox::Yes)
		return;

	exportDir += "/%1";

	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::critical(this, tr("Error"),
			tr("Could not open file \"%1\" for writing.").arg(fileName));
		return;
	}

	QDomDocument doc("qutimsounds");
	QDomProcessingInstruction process =
		doc.createProcessingInstruction("xml",
		"version=\"1.0\" encoding=\"utf-8\"");
	doc.appendChild(process);

	QDomElement rootElement = doc.createElement("qutimsounds");
	doc.appendChild(rootElement);

	QDomElement soundsElement = doc.createElement("sounds");

	int i;
	QString soundFile;
	QString newFile;
	for (i = 0; i < eventList.size(); i++)
	{
		soundFile = eventList.at(i)->data(0, Qt::UserRole+1).toString();
		if (soundFile.isEmpty()) continue;
		if (!QFile::exists(soundFile)) continue;
		QDomElement soundElement = doc.createElement("sound");
		QDomAttr eventAttr = doc.createAttribute("event");
		eventAttr.setValue(qutim_sdk_0_2::XmlEventNames[eventList.at(i)->data(0,
			Qt::UserRole).toInt()]);
		soundElement.setAttributeNode(eventAttr);

		QDomElement fileElement = doc.createElement("file");
		QDomText fileText =
			doc.createTextNode(soundFile.mid(soundFile.lastIndexOf('/')+1));

		newFile = exportDir.arg(fileText.data());

		// check if directories are different
		if (newFile.section('/', 0, -2) != soundFile.section('/', 0, -2))
		{
			if (QFile::exists(newFile))
				QFile::remove(newFile);

			if (!QFile::copy(soundFile, newFile))
			{
				QMessageBox::critical(this, tr("Error"),
					tr("Could not copy file \"%1\" to \"%2\".").arg(soundFile).arg(newFile));
				file.close();
				return;
			}
		}

		fileElement.appendChild(fileText);
		soundElement.appendChild(fileElement);

		soundsElement.appendChild(soundElement);
	}

	rootElement.appendChild(soundsElement);

	QTextStream fileStream(&file);
	fileStream.setCodec("UTF-8");
	fileStream << doc.toString();
	file.close();

	QMessageBox::information(this, tr("Export..."),
		tr("Sound events successfully exported to file \"%1\".").arg(fileName));
}

void SoundLayerSettings::on_importButton_clicked()
{
	int i, eventPos;
	bool bChanged = false;
	QString fileName(QFileDialog::getOpenFileName(this, tr("Import sound style"),
		lastDir, tr("XML files (*.xml)")));
	if (fileName.isEmpty()) return;

	QString importDir = fileName.section('/', 0, -2);
	lastDir = importDir;

	importDir += "/%1";

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(this, tr("Error"),
			tr("Could not open file \"%1\" for reading.").arg(fileName));
		return;
	}

	QDomDocument doc("qutimsounds");
	if (!doc.setContent(&file))
	{
		file.close();
		QMessageBox::critical(this, tr("Error"),
			tr("An error occured while reading file \"%1\".").arg(fileName));
		return;
	}
	file.close();

	QDomElement rootElement = doc.documentElement();

	QDomNodeList soundsNodeList = rootElement.elementsByTagName("sounds");
	if (soundsNodeList.count() != 1)
	{
		QMessageBox::critical(this, tr("Error"),
			tr("File \"%1\" has wrong format.").arg(fileName));
		return;
	}

	// Preparing QHash for faster search;
	QHash<QString, int> eventHash;
	int event;
	for (i = 0; i < eventList.size(); i++)
	{
		event = eventList.at(i)->data(0, Qt::UserRole).toInt();
		if (!qutim_sdk_0_2::XmlEventNames[event]) continue;
		eventHash.insert(qutim_sdk_0_2::XmlEventNames[event], i);

		// clear old files
		eventList.at(i)->setData(0, Qt::UserRole+1, QString());
		eventList.at(i)->setCheckState(0, Qt::Unchecked);
	}

	QDomElement soundsElement = soundsNodeList.at(0).toElement();
	soundsNodeList = soundsElement.elementsByTagName("sound");

	QDomElement soundElement;
	QString eventName, soundFileName;

	for (i = 0; i < soundsNodeList.count(); i++)
	{
		soundElement = soundsNodeList.at(i).toElement();
		eventName = soundElement.attribute("event");
		if (eventName.isEmpty()) continue;

		if (!eventHash.contains(eventName)) continue;

		if (!soundElement.elementsByTagName("file").count()) continue;

		soundFileName = importDir.arg(soundElement.elementsByTagName("file").at(0).toElement().text());
		if (!QFile::exists(soundFileName)) continue;

		eventPos = eventHash.value(eventName);
		eventList.at(eventPos)->setData(0,
			Qt::UserRole+1, soundFileName);

		eventList.at(eventPos)->setCheckState(0, Qt::Checked);

		bChanged = true;
	}

	if (bChanged) widgetStateChanged();

	on_eventsTree_currentItemChanged(ui.eventsTree->currentItem(), NULL);
	}


inline QString SoundLayerSettings::getCurrentFile() const
{
	QTreeWidgetItem *item = ui.eventsTree->currentItem();
	if (item)
		return item->data(0, Qt::UserRole+1).toString();
	else
		return QString();
}

//inline qutim_sdk_0_2::SoundEngineSystem SoundLayerSettings::getCurrentSoundSystem() const
//{
//	return static_cast<qutim_sdk_0_2::SoundEngineSystem>(ui.systemCombo->itemData(ui.systemCombo->currentIndex()).toInt());
//}
