#!/bin/sh

function die() {
	echo $1
	exit 1
}

function deploy_binary() {
	local file=$1
	local path=$2
	install_name_tool -id $(basename $file) $file
	list=$(otool -L "$file" | grep -Po '^\s+/(?!usr/lib|System).*? ')
	if [ "$list" != "" ]; then
		for target in $list; do
			lib=$(basename $target)
			if [ -r $path/Contents/PlugIns/$lib ]; then
				install_name_tool -change $target @executable_path/../PlugIns/$lib $file
			else
				install_name_tool -change $target @executable_path/../Frameworks/$lib $file
				if [ ! -r $path/Contents/Frameworks/$lib ]; then
					mkdir -p "$path/Contents/Frameworks/"
					cp $target $path/Contents/Frameworks/$lib
					deploy_binary $path/Contents/Frameworks/$lib $path
				fi
			fi
		done
	fi
}

BUNDLE=${1%/}
[ ! -x "$(which otool)" ] && die "otool doesn't exist"
[ ! -x "$(which install_name_tool)" ] && die "install_name_tool doesn't exist"
for file in `find $BUNDLE/Contents/{MacOS,PlugIns} -type f`; do
	deploy_binary $file $BUNDLE
done
