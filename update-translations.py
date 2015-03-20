#!/usr/bin/python

#****************************************************************************
#**
#** qutIM instant messenger
#**
#** Copyright (c) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

import os
import json
import glob
import re
import subprocess


def ensure_command(*commands):
    for command in commands:
        try:
            subprocess.check_call([command, '--help'])
            return command
        except subprocess.CalledProcessError:
            return command
        except:
            pass
        
    raise RuntimeError('Executables not found: {}'.format(commands))


lupdate = ensure_command('lupdate-qt5', 'lupdate')
lconvert = ensure_command('lconvert-qt5', 'lconvert')
msgmerge = ensure_command('msgmerge')

modules = [('core', 'core')]
modules += map(lambda module: ('plugins/' + module, module), os.listdir('plugins'))
modules += map(lambda module: ('protocols/' + module, module), os.listdir('protocols'))
modules = filter(lambda module: os.path.isdir(module[0]) and os.path.exists(os.path.join(module[0], module[1] + '.qbs')), modules)

files_to_remove = set()


def write_string(path, context, strings):
    with open(path, "w") as f:
        files_to_remove.add(path)
        for x in strings:
            f.write('Qt::translate("{0}", "{1}");\n'.format(context, x))


strings = set()

for resource in glob.glob("core/share/qutim/webkitstyle/*/Contents/Resources/*.json"):
    with open(resource) as f:
        try:
            for item in json.load(f):
                if 'label' in item:
                    strings.add(item['label'])
        except Exception as e:
            print '[ERROR] {0}: {1}'.format(resource, e)
        
write_string('plugins/adiumwebview/__custom_json_from_styles.cpp', 'Style', strings)

strings = set()

for root, dirs, files in os.walk('plugins/weather'):
    for file in filter(lambda x: x.endswith('.html'), files):
        with open(os.path.join(root, file)) as f:
            for line in f:
                strings.update(map(lambda x: x[:-2], re.findall('(?<=%localized{).*?}%', line)))

write_string('plugins/weather/__template_from_weather.cpp', 'Weather', strings)

strings = set()

for root, dirs, files in os.walk('.'):
    for file in filter(lambda x: x.endswith('.h'), files):
        with open(os.path.join(root, file)) as f:
            for line in f:
                strings.update(map(lambda x: x.group(1), re.finditer('Q_CLASSINFO.*"Service".*"(.*?)"\\)', line)))

write_string('core/__generated_from_service_names.cpp', 'Service', strings)

authors = set()
tasks = set()

for file in glob.glob('core/contributers/*.json') + glob.glob('core/devels/*.json'):
    with open(file) as f:
        data = json.load(f)
        
        if 'name' in data:
            authors.add(data['name'])
        
        if 'task' in data:
            tasks.add(data['task'])
                
devels_path = '__from_devels_json.cpp'
with open(devels_path, "w") as f:
    files_to_remove.add(devels_path)

    cpp_strings = []
    cpp_strings += map(lambda x: 'Qt::translate("{0}", "{1}");\n'.format('Author', x), authors)
    cpp_strings += map(lambda x: 'Qt::translate("{0}", "{1}");\n'.format('Task', x), tasks)
    f.writelines(cpp_strings)
    
modules.append((devels_path, 'devels'))

for module in modules:
    strings = set()
    
    for file in glob.glob(os.path.join(module[0], '*.plugin.json')):
        with open(file) as f:
            data = json.load(f)
            
            for label in ('pluginDescription', 'pluginName'):
                if label in data:
                    strings.add(data[label])
                    
    if strings:
        write_string(os.path.join(module[0], '__data_from_plugin_json.cpp'), 'Plugin', strings)
        
    module_path = os.path.join('translations/modules', module[1])
    
    if not os.path.exists(module_path):
        os.makedirs(module_path)
    
    ts_file = os.path.join(module_path, module[1] + '.ts')
    pot_file = os.path.join(module_path, module[1] + '.pot')
    
    subprocess.call([lupdate, '-extensions', 'h,cpp,mm,js,c,ui,qml', '-locations', 'relative', module[0], '-ts', ts_file])
    subprocess.call([lconvert, '-i', ts_file, '-o', pot_file])
    
    files_to_remove.add(ts_file)
    
    for po_file in glob.glob(os.path.join(module_path, '*.po')):
        language = os.path.basename(po_file)[:-3]
        
        subprocess.call([msgmerge, '--update', '--lang', language, '--backup=off', po_file, pot_file])

for file in files_to_remove:
    os.remove(file)
