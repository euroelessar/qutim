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

#include "webkitcolorsadditions_p.h"
#include <QVariant>

QStringList initColors()
{
    QStringList list;
	const char *colors[] = {
	    "aqua", "aquamarine", "blue", "blueviolet", "brown", "burlywood",
	    "cadetblue", "chartreuse", "chocolate", "coral", "cornflowerblue",
	    "crimson", "cyan", "darkblue", "darkcyan", "darkgoldenrod",
	    "darkgreen", "darkgrey", "darkkhaki", "darkmagenta", "darkolivegreen",
	    "darkorange", "darkorchid", "darkred", "darksalmon", "darkseagreen",
	    "darkslateblue", "darkslategrey", "darkturquoise", "darkviolet",
	    "deeppink", "deepskyblue", "dimgrey", "dodgerblue", "firebrick",
	    "forestgreen", "fuchsia", "gold", "goldenrod", "green", "greenyellow",
	    "grey", "hotpink", "indianred", "indigo", "lawngreen", "lightblue",
	    "lightcoral", "lightgreen", "lightgrey", "lightpink", "lightsalmon",
	    "lightseagreen", "lightskyblue", "lightslategrey", "lightsteelblue",
	    "lime", "limegreen", "magenta", "maroon", "mediumaquamarine", "mediumblue",
	    "mediumorchid", "mediumpurple", "mediumseagreen", "mediumslateblue",
	    "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue",
	    "navy", "olive", "olivedrab", "orange", "orangered", "orchid", "palegreen",
	    "paleturquoise", "palevioletred", "peru", "pink", "plum", "powderblue",
	    "purple", "red", "rosybrown", "royalblue", "saddlebrown", "salmon",
	    "sandybrown", "seagreen", "sienna", "silver", "skyblue", "slateblue",
	    "slategrey", "springgreen", "steelblue", "tan", "teal", "thistle", "tomato",
	    "turquoise", "violet", "yellowgreen"
	};
	int size = sizeof(colors) / sizeof(colors[0]);
	list.reserve(size);
	for (int i = 0; i < size; ++i)
		list << QLatin1String(colors[i]);

    return list;
}

Q_GLOBAL_STATIC_WITH_ARGS(QStringList, validColors, (initColors()))

WebKitColorsAdditions::WebKitColorsAdditions()
{
}

QString WebKitColorsAdditions::representedColorForObject(const QString &object, const QStringList &colors)
{
	QStringList colorsArray = colors;
	if (colors.isEmpty())
		colorsArray = *validColors();
	return colorsArray.at(qHash(object) % colorsArray.size());
}
