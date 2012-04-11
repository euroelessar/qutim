/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef CHATAPPEARANCE_H
#define CHATAPPEARANCE_H

#include "../lib/webkitpreviewloader.h"
#include <qutim/settingswidget.h>
#include <QWidget>
#include <QHash>

namespace Ui
{
	class chatAppearance;
}
class QWebPage;
class QLabel;

namespace Adium
{

class WebViewVariable;
class WebViewController;

class WebViewAppearance : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
public:
	WebViewAppearance();
	virtual ~WebViewAppearance();
	
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
	
private slots:
	void onVariableChanged();
	void on_styleNameBox_currentIndexChanged(const QString &styleName);
	void on_variantBox_currentIndexChanged(const QString &variant);
	void on_showUserIconBox_clicked(bool checked);
	void on_showHeaderBox_clicked(bool checked);
	void on_fontButton_clicked();
	void on_defaultFontButton_clicked();
	void on_customBackgroundBox_clicked(bool checked);
	void on_backgroundTypeBox_currentIndexChanged(int index);
	void on_imageButton_clicked(bool checked);
	void on_colorButton_clicked(bool checked);
	
protected:
	void rebuildChatView();
	void updateCustomScript();
	void setBackgroundPath(const QString &path);
	QString backgroundPath() const;
	void setBackgroundColor(const QColor &color);
	QColor backgroundColor() const;
	void setDefaultFont(const QString &family, int size);
	void makeSettings();
	
private:
	void fillStylesComboBox();
	
	Ui::chatAppearance *ui;
	WebKitPreview::Ptr m_preview;
	QString m_styleName;
	QHash<QString, QString> m_themes;
	QList<WebViewVariable *> m_variables;
	WebKitMessageViewStyle *m_style;
	WebViewController *m_controller;
	bool m_isLoad;
	bool m_isFillingState;
};

}

#endif // CHATAPPEARANCE_H

