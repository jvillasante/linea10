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

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"
  
echo '</body>'
echo '</html>'
