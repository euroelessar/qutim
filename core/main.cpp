/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include <QApplication>
#include <QTextCodec>
#include <QWidget>

#include <cstdlib>
#include <ctime>

#ifdef QUTIM_DECLARATIVE_UI
#include "src/declarative/modulemanagerimpl.h"
#else
#include "src/widgets/modulemanagerimpl.h"
#endif
#include <QTime>

#if defined(STATIC_IMAGE_PLUGINS)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

#ifdef MEEGO_EDITION
#include <MDeclarativeCache>

Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#ifdef MEEGO_EDITION
	QScopedPointer<QApplication> app(MDeclarativeCache::qApplication(argc, argv));
#else
	QScopedPointer<QApplication> app(new QApplication(argc, argv));
#endif

	Core::ModuleManagerImpl core;
	Q_UNUSED(core);

    qsrand(std::time(nullptr));
    srand(qrand());

	return app->exec();
}

