/*
    StatusDialog

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

#include "statusdialog.h"
#include "statuspresetcaption.h"
#include "pluginsystem.h"

StatusDialog::StatusDialog(const QString &profile_name,
		QWidget *parent)
    : QDialog(parent)
    , m_icon_manager(IconManager::instance())
    , m_profile_name(profile_name)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setFixedSize(size());
	PluginSystem::instance().centerizeWidget(this);
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	m_preset_file = settings.fileName().section('/', 0, -2) + "/statuspresets.xml";
	loadPresets();
	setWindowTitle(tr("Write your status message"));
	setWindowIcon(m_icon_manager.getIcon("statuses"));
	ui.delButton->setIcon(m_icon_manager.getIcon("deletetab"));
	ui.okButton->setIcon(m_icon_manager.getIcon("apply"));
	ui.saveButton->setIcon(m_icon_manager.getIcon("save_all"));
	ui.cancelButton->setIcon(m_icon_manager.getIcon("cancel"));
}

StatusDialog::~StatusDialog()
{

}

QPoint StatusDialog::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

void StatusDialog::on_saveButton_clicked()
{
	StatusPresetCaption caption_dialog;
	caption_dialog.setWindowTitle(tr("Preset caption"));
	caption_dialog.setWindowIcon(m_icon_manager.getIcon("statuses"));
	caption_dialog.setButtonIcon(m_icon_manager.getIcon("apply"),
			m_icon_manager.getIcon("cancel"));
	if ( caption_dialog.exec() )
	{
		savePreset(caption_dialog.getCaption());
	}
}

void StatusDialog::savePreset(const QString &preset_caption)
{
	QFile presets_file(m_preset_file);

	if ( presets_file.exists() )
	{
		if (presets_file.open(QIODevice::ReadOnly) )
		{
			QDomDocument doc;
			if ( doc.setContent(&presets_file) )
			{
				QDomElement rootElement = doc.documentElement();
				QDomElement preset_element = doc.createElement("preset");
				QDomElement caption_element = doc.createElement("caption");
				QDomText caption_text = doc.createTextNode(preset_caption);
				caption_element.appendChild(caption_text);
				QDomElement message_element = doc.createElement("message");
				QDomText message_text = doc.createTextNode(ui.statusTextEdit->toPlainText());
				message_element.appendChild(message_text);
				preset_element.appendChild(caption_element);
				preset_element.appendChild(message_element);
				rootElement.insertAfter(preset_element, rootElement.lastChild());

				presets_file.close();
				if ( presets_file.open(QIODevice::WriteOnly) )
				{
					QTextStream preset_stream(&presets_file);
					preset_stream.setCodec("UTF-8");
					preset_stream<<doc.toString();
					presets_file.close();
				}
			}
		}
	} else {
		if ( presets_file.open(QIODevice::WriteOnly) )
		{
			QDomDocument doc("stauspresets");
			QDomProcessingInstruction process = doc.createProcessingInstruction(
			        "xml", "version=\"1.0\" encoding=\"utf-8\"");
			doc.appendChild(process);
			QDomElement rootElement = doc.createElement("stauspresets");
			doc.appendChild(rootElement);
				
			QDomElement preset_element = doc.createElement("preset");
			QDomElement caption_element = doc.createElement("caption");
			QDomText caption_text = doc.createTextNode(preset_caption);
			caption_element.appendChild(caption_text);
			QDomElement message_element = doc.createElement("message");
			QDomText message_text = doc.createTextNode(ui.statusTextEdit->toPlainText());
			message_element.appendChild(message_text);
			
			preset_element.appendChild(caption_element);
			preset_element.appendChild(message_element);
			rootElement.appendChild(preset_element);
			doc.appendChild(rootElement);
			
			QTextStream presets_stream(&presets_file);
			presets_stream.setCodec("UTF-8");
			presets_stream<<doc.toString();
			presets_file.close();
		}
	}
	ui.presetComboBox->addItem(preset_caption,ui.statusTextEdit->toPlainText());
	
}

void StatusDialog::loadPresets()
{
	QFile presets_file(m_preset_file);
	if ( presets_file.open(QIODevice::ReadOnly))
	{
		QDomDocument doc;
		if ( doc.setContent(&presets_file) )
			{
				QDomElement rootElement = doc.documentElement();
				int msgCount = rootElement.elementsByTagName("preset").count();
				
				QDomElement preset = rootElement.firstChildElement("preset");
				
				for( int i = 0; i < msgCount; i++)
				{
					QDomElement preset_caption = preset.firstChildElement("caption");
					QDomElement preset_message = preset.firstChildElement("message");
					ui.presetComboBox->addItem(preset_caption.text(), preset_message.text());
					preset = preset.nextSiblingElement("preset");
				}
					
				presets_file.close();						
			}
	}
}

void StatusDialog::on_presetComboBox_currentIndexChanged(int index)
{
	if( index )
	{
		ui.statusTextEdit->setPlainText(ui.presetComboBox->itemData(index).toString());
	}
}

void StatusDialog::on_delButton_clicked()
{
	int index = ui.presetComboBox->currentIndex();
	if ( index )
	{
		QFile presets_file(m_preset_file);
		if ( presets_file.open(QIODevice::ReadOnly))
		{
			QDomDocument doc;
			if ( doc.setContent(&presets_file) )
				{
					QDomElement rootElement = doc.documentElement();
					int msgCount = rootElement.elementsByTagName("preset").count();
					QDomElement preset = rootElement.firstChildElement("preset");
					for( int i = 0; i < msgCount; i++)
					{
						QDomElement preset_caption = preset.firstChildElement("caption");
						QDomElement preset_message = preset.firstChildElement("message");
						if ( ui.presetComboBox->currentText() == preset_caption.text() &&
								ui.presetComboBox->itemData(index).toString() == preset_message.text() )
						{
							rootElement.removeChild(preset);
						}
						preset = preset.nextSiblingElement("preset");
					}
					presets_file.close();	
					if ( presets_file.open(QIODevice::WriteOnly) )
					{
						QTextStream preset_stream(&presets_file);
						preset_stream.setCodec("UTF-8");
						preset_stream<<doc.toString();
						presets_file.close();
					}
				}
		}
		ui.presetComboBox->removeItem(index);
	}
}
