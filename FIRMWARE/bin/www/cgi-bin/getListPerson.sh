#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para listas las Personas Habilitadas </title>'
echo '</head>'
echo '<body>'

echo "LISTADO DE PERSONAS :"
echo '<br>'
echo '<br>'

echo "identificador|nombre|rut"
echo "<br>"
echo "<br>"

sqlite3 /mnt/jffs2/iengine.db "SELECT name, value FROM iengine_tags WHERE 1" | while read ROW; do
  # Parsing data (sqlite3 returns a pipe separated string)
  Name=`echo $ROW | awk '{split($0,a,"|"); print a[1]}'`
  Value=`echo $ROW | awk '{split($0,a,"|"); print a[2]}'`

  if [ "$Name" = 'IDENTIFIER' ]; then
    echo "$Value"
  fi
  if [ "$Name" = 'NAME' ]; then
    echo " | $Value"
  fi
  if [ "$Name" = 'RUT' ]; then
    echo " | $Value <br>"
  fi
done

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"

echo '</body>'
echo '</html>'
