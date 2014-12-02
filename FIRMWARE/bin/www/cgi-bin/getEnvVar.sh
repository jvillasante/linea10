#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Datos del sistema</title>'
echo '</head>'
echo '<body>'

echo "<h4>Interfaz de Red</h4>"
echo "<p><pre>$(/sbin/ifconfig)</pre></p>"
echo "<p><pre>$(/sbin/route -n)</pre></p>"

echo "<hr />"

echo "<h4>Procesos</h4>"
echo "<p><pre>$(ps aux)</pre></p>"

echo "<hr />"

echo "<h4>Memoria RAM</h4>"
echo "<p><pre>$(free -m)</pre></p>"

echo "<hr />"

echo "<h4>Disco Duro:</h4>"
echo "<p><pre>$(df -h)</pre></p>"

echo "<hr />"

echo "<h4>Variables de entorno</h4>"
echo "<p><pre>$(env)</pre></p>"

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"
  
echo '</body>'
echo '</html>'
