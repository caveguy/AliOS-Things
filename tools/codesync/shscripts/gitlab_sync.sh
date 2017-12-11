#!/bin/sh

cd ~/
if [ ! -d githubsync ]; then
    echo "error: githubsync directory does not exist"
    exit 1
fi

cd ~/githubsync
if [ ! -d AliOS ]; then
    echo "error: AliOS directory does not exist"
    exit 1
fi
if [ ! -d gitlab ]; then
    git clone git@gitlab.alibaba-inc.com:lc122798/aos_github_sync.git gitlab
fi
githubdir=~/githubsync/AliOS
gitlabdir=~/githubsync/gitlab
cd ${gitlabdir}
git reset --hard 91299eace971576c00eaf0a56fbcd97ff981700f

rm -rf ${gitlabdir}/*
cp -rf ${githubdir}/* ${gitlabdir}/
cp -rf ${githubdir}/.gitignore ${gitlabdir}/
cp -rf ${githubdir}/.vscode ${gitlabdir}/

git add -A
datetime=`date +%F@%H:%M`
git commit -m "github code synchronization at ${datetime}" > /dev/null
git push -f origin master
