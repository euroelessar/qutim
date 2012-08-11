#!/bin/bash
# Argument = --help --developer

usage()
{
cat << EOF
usage: $0 options

This script init qutIM repo

OPTIONS:
   -h      Show this message
   -d Init with upstream repos for merge requests
EOF
}

add_upstream_repo()
{
    local repo_path=$1
    local repo_url=$2
    local root=$PDW
    echo "--Add upstream ${repo_url} for ${repo_path}"
    cd $repo_path
    git remote add upstream $repo_url &> /dev/null
    cd $root
}

init_repository()
{
git submodule update --init --recursive
}

init_developer()
{
    echo "--Adding upstream remote repos for easy development"
    local root=$PWD

    add_upstream_repo $root git://github.com/euroelessar/qutim.git
    add_upstream_repo $root/protocols/jabber/jreen git://github.com/euroelessar/jreen.git
    add_upstream_repo $root/protocols/vkontakte/vreen git://github.com/gorthauer/vreen.git
}

dflag=false
while getopts “hd” OPTION
do
    case $OPTION in
    h)
	 usage
	 exit 1
     ;;
    d)
	dflag=true
     ;;
    ?)
	exit
     ;;
    esac
done

init_repository
if $dflag ; then
    init_developer
fi
