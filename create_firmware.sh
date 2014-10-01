#!/bin/bash -ex

fwVersion="FW-$(date +%d%m%Y)-00"

cd FIRMWARE/
rm -f firmware.tar.gz
rm -f firmware.tar.gz.md5
rm -f app.ini
rm -f FIRMWARE-*.tar.gz

cp -f ../BUILD/tempo/tempo bin/tempo
cp -f ../BUILD/updater/updater bin/updater
cp -f ../BUILD/synchronizer/synchronizer bin/synchronizer
cp -fr ../SRC/tempo/Resources/ bin
rm -rf bin/Resources/images/

echo "" >> bin/Resources/settings/app.ini
echo "; version del firmware (FW-ddMMyyyy-00) [valor autogenerado]" >> bin/Resources/settings/app.ini
echo "fwVersion = $fwVersion" >> bin/Resources/settings/app.ini
cp -f bin/Resources/settings/app.ini .
# rm -rf bin/Resources/settings/

tar -czf /usr/local/firmware.tar.gz bin
md5sum /usr/local/firmware.tar.gz > /usr/local/firmware.tar.gz.md5

mv /usr/local/firmware.tar.gz .
mv /usr/local/firmware.tar.gz.md5 .

chmod 664 firmware.tar.gz
chmod 664 firmware.tar.gz.md5

tar -cvzf FIRMWARE-$(date +%d.%m.%Y).tar.gz app.ini firmware.tar.gz firmware.tar.gz.md5
