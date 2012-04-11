#!/usr/bin/perl

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

use strict;
use warnings;

FILE: while (my $file = <STDIN>){
    chomp $file;
    open (my $CURFILE, "<", $file) or warn "Cannot open $file: $!", next FILE;
    my $found_header = 0;
    my $found_header_end = 0;
    my @list;
    while (my $str = <$CURFILE>){
        push @list, $str;
        if (!$found_header){
            $found_header = 1 if ($str =~ /msgid\s+["]{2}/);
        }
        elsif (!$found_header_end) {
            next FILE if ($str =~ /"X-Qt-Contexts:\s*true\\n"/);
            if ($str =~ /^\n$/ && !$found_header_end){
                pop @list;
                push @list, qq/"X-Qt-Contexts: true\\n"\n\n/;
                $found_header_end = 1;
            }
        }
    }
    close ($CURFILE);
    open ($CURFILE, ">>", $file) or warn "Cannot modify file: $!"; # Maybe there should be created *.bak?
    truncate $CURFILE, 0; # well, it possible to use ">", but I don't like it because of autotruncate.
    print $CURFILE $_ foreach (@list);
}