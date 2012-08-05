#****************************************************************************
#**
#** qutIM instant messenger
#**
#** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
#**
#*****************************************************************************
#**
#** $QUTIM_BEGIN_LICENSE$
#** This program is free software: you can redistribute it and/or modify
#** it under the terms of the GNU General Public License as published by
#** the Free Software Foundation, either version 3 of the License, or
#** (at your option) any later version.
#**
#** This program is distributed in the hope that it will be useful,
#** but WITHOUT ANY WARRANTY; without even the implied warranty of
#** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#** See the GNU General Public License for more details.
#**
#** You should have received a copy of the GNU General Public License
#** along with this program.  If not, see http://www.gnu.org/licenses/.
#** $QUTIM_END_LICENSE$
#**
#****************************************************************************/

#!/bin/sh

function die() {
	echo $1
	exit 1
}

function deploy_binary() {
	local file="$1"
	local path="$2"
	install_name_tool -id $(basename "$file") "$file"
	otool -L "$file" | grep -Po '^\s+/(?!usr/lib|System).+?(?= \()'| \
	while read target; do
		lib=$(basename "$target")
		if [ -r "$path/Contents/PlugIns/$lib" ]; then
			install_name_tool -change "$target" @executable_path/../PlugIns/$lib "$file"
		else
			install_name_tool -change "$target" @executable_path/../Frameworks/$lib "$file"
			if [ ! -r "$path/Contents/Frameworks/$lib" ]; then
				mkdir -p "$path/Contents/Frameworks/"
				cp "$target" "$path/Contents/Frameworks/$lib"
				chmod 755 "$path/Contents/Frameworks/$lib"
				deploy_binary "$path/Contents/Frameworks/$lib" "$path"
			fi
		fi
	done
}

BUNDLE=${1%/}
[ ! -x "$(which otool)" ] && die "otool doesn't exist"
[ ! -x "$(which install_name_tool)" ] && die "install_name_tool doesn't exist"
find "$BUNDLE/Contents/MacOS" "$BUNDLE/Contents/PlugIns" -type f | \
while read file; do
	deploy_binary "$file" "$BUNDLE"
done
