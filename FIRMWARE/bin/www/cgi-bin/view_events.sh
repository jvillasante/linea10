#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Eventos</title>'
echo '</head>'
echo '<body>'

echo "Eventos:"
echo '<br>'
echo '<br>ntp manual

# This is the 2nd commit message:

ntp esta listo
'

echo "id|sentido|identificador|fecha|sincronizado"
echo "<br>"
echo "<br>"

sqlite3 /mnt/jffs2/genera.db "SELECT substr('0000000000' || id, -10, 10), sense, ident, datetime(date, 'unixepoch', 'localtime'), synchronized FROM events" | while read ROW; do
  echo "$ROW <br>"
done

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"

echo '</body>'
echo '</html>'
