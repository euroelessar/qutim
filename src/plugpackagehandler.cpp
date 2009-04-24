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

#include "plugpackagehandler.h"

plugPackageHandler::plugPackageHandler(plugPackageModel* plug_package_model, QObject* parent)
: m_plug_package_model(0)
{
	setParent(parent);
	m_plug_package_model = plug_package_model;
}

plugPackageHandler::~plugPackageHandler() {

}

void plugPackageHandler::getPackageList() {
	plugXMLHandler plug_handler;
	QHash<QString, packageInfo> package_list= plug_handler.getPackageList();
	updatePlugPackageModel(package_list);
	return;
}



void plugPackageHandler::setPlugPackageModel(plugPackageModel* plug_package_model) {
	m_plug_package_model = plug_package_model;
}

void plugPackageHandler::updatePlugPackageModel(const QHash<QString, packageInfo>& package_list) {
	QHash<QString, packageInfo>::const_iterator it = package_list.begin();
	for (;it != package_list.end();it++) {
		ItemData item(buddy,QIcon(":/icons/hi64-action-package.png"),package_list.value(it.key()),package_list.value(it.key()).id);
		m_plug_package_model->addItem(item,it.key());
	}
	return;
}
