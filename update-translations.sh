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

# Try to find lconvert
lconvert=`which lconvert-qt4`
if [ -nf $lconvert ]
then
	lconvert=`which lconvert`
fi

# Try to find lupdate
lupdate=`which lupdate-qt4`
if [ -nf $lupdate ]
then
	lupdate=`which lupdate`
fi

customJsonFile=$PWD/plugins/adiumwebview/__custom_json_from_styles.cpp
weatherFile=$PWD/plugins/weather/__template_from_weather.cpp

grep \"label\" $PWD/core/share/qutim/webkitstyle/*/Contents/Resources/*.json | sed 's/.*://g;s/, *$/);/g;s/^/Qt::translate("Style",/' | sort -u > $customJsonFile
find ./plugins/weather/ -type f -name \*html -exec cat {} \; | sed 's/%localized{/\n%localized{/g;s/}%/}%\n/g' | grep %localized{ | sed 's/%localized{/Qt::translate("Weather", "/;s/}%$/");/' | sort -u > $weatherFile

for file in $PWD/plugins/* $PWD/protocols/* $PWD/core
do
	if [ -f $file/CMakeLists.txt ]
	then
		module=`basename $file`
		modulePath=$PWD/translations/modules/$module
		customCMakeFile=$file/__data_from_cmakelists_txt.cpp
		mkdir -p $modulePath
		find "$file" -name CMakeLists.txt -exec cat {} \; | grep -P '(DISPLAY_NAME|DESCRIPTION)' | sed 's/\r//;s/.*\(DISPLAY_NAME\|DESCRIPTION\)/Qt::translate("Plugin",/;s/$/);/' > $customCMakeFile
		$lupdate -extensions "h,cpp,mm,js,c,ui,qml" -locations relative $file -ts "$modulePath/$module.ts"
		$lconvert -i "$modulePath/$module.ts" -o "$modulePath/$module.pot"
		rm "$modulePath/$module.ts"
		rm "$customCMakeFile"
		for poFile in `ls $modulePath/*.po`
		do
			msgmerge --update --backup=off $poFile "$modulePath/$module.pot"
		done
	fi
done

rm $customJsonFile $weatherFile

module=devels
modulePath=$PWD/translations/modules/$module
mkdir -p $modulePath
cat $PWD/core/devels/*.json $PWD/core/contributers/*.json | grep -P '(name|task)' | sed 's/[ \t]*"/"/g;s/^/pgettext(/;s/name/Author/;s/task/Task/;s/,/);/;s/:/,/' | xgettext -C --from-code=utf-8 --force-po --no-location - -o - | sed 's/CHARSET/UTF-8/' > $modulePath/$module.pot
for poFile in `ls $modulePath/*.po`
do
	msgmerge --update --backup=off $poFile "$modulePath/$module.pot"
done

