#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Informaci&oacute;n del Equipo</title>'
echo '</head>'
echo '<body>'

echo "<strong>NTP Status:</strong>"
if [ -f /usr/local/bin/Resources/ntp_is_running ]; then
  echo " NTP est&aacute corriendo en este sistema."
else
  echo " NTP NO est&aacute corriendo en este sistema."
fi

echo "<br/><br/>"

echo "<strong>Versi&oacute;n del Firmware:</strong>"
fwVersion=$(sed -n 's/.*fwVersion*=*\([^ ]*.*\)/\1/p' < /mnt/jffs2/app.ini)
echo " $fwVersion."
  
echo '</body>'
echo '</html>'
