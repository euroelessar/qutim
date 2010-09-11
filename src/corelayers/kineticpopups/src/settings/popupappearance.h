/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef POPUPAPPEARANCE_H
#define POPUPAPPEARANCE_H
#include <QWidget>
#include "backend.h"
#include <qutim/settingswidget.h>

namespace Ui
{
    class AppearanceSettings;
}

namespace Core
{
namespace KineticPopups
{

class AbstractPopupWidget;
class PopupAppearance : public SettingsWidget
{
	Q_OBJECT
public:
	PopupAppearance();
	virtual ~PopupAppearance();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void onCurrentIndexChanged(int index);
	void onTestButtonClicked(bool);
private:
	void getThemes();
	void preview();
	QString m_current_theme;
	Ui::AppearanceSettings *ui;
	AbstractPopupWidget *m_popup_widget;
    };

}
}
#endif // POPUPAPPEARANCE_H
