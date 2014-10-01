#!/bin/bash -ex

./clean.sh

rm /home/jvillasante/Dropbox/Work/linea10*.tar.gz
tar -cvzf /home/jvillasante/Dropbox/Work/linea10-$(date +%d.%m.%Y).tar.gz $PWD/../linea10/
