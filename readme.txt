Compilacion:
  Compilar proyecto especificando las librerias a usar (HOST | TARGET)
  Compilar los lenguages:
    lupdate tempo.pro
    lrelease tempo.pro

Create FIRMWARE:
===============
  * use create_firmware.sh to create the firmware upgrade files (tar.gz and md5 signature)
  * upload the files to the unit under /usr/local (you can use scp)
  * after reboot the unit will replace the content of /usr/local/bin with the content of the firmware
  * when the upgrade process finishes it will remove the firmware files from /usr/local
  * a new directory called bin.old (under /usr/local) will hold the content of the old /usr/local/bin directory
