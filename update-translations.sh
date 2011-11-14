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

for file in $PWD/plugins/* $PWD/protocols/* $PWD/core
do
	if [ -f $file/CMakeLists.txt ]
	then
		module=`basename $file`
		modulePath=$PWD/translations/modules/$module
		for poFile in `ls $modulePath/*.po`
		do
			lang=`basename $poFile | sed 's/\..*//'`
			$lconvert -i "$modulePath/$lang.po" -o "$modulePath/$lang.ts"
		done
		for langFile in `ls $modulePath/*.ts`
		do
			lang=`basename $langFile | sed 's/\..*//'`
			$lupdate -extensions "h,cpp,mm,js,c,ui,qml" -locations relative -target-language $lang "$file" -ts "$modulePath/$lang.ts"
			$lconvert -i "$modulePath/$lang.ts" -o "$modulePath/$lang.po"
			rm "$modulePath/$lang.ts"
		done
		$lupdate -extensions "h,cpp,mm,js,c,ui,qml" -locations relative $file -ts "$modulePath/$module.ts"
		$lconvert -i "$modulePath/$module.ts" -o "$modulePath/$module.pot"
		rm "$modulePath/$module.ts"
	fi
done

