#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio de la fecha y hora del dispositivo </title>'
echo '</head>'
echo '<body>'

echo "FECHA Y HORA: $(date) <br><br>"
echo "NTP actualiza la fecha y hora autom&aacute;ticamente.<br>"
if [ -f /usr/local/bin/Resources/ntp_is_running ]; then
  echo "<strong>NTP est&aacute corriendo en este sistema.</strong>"
else
  echo "<strong>NTP NO est&aacute corriendo en este sistema.</strong>"
fi
  
echo '</body>'
echo '</html>'
