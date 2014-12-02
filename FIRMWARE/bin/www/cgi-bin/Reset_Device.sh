#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para resetear el dispositivo </title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<input type="hidden" name="reset" value="1">'\
  '<table nowrap>'\
  '<tr><td>Opcion para Resetear el Dispositivo</TD></tr>'\
  '</tr></table>'

echo '<br><input type="submit" value="Resetear Ahora!"></form>'

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"

# Make sure we have been invoked properly.

if [ "$REQUEST_METHOD" != "GET" ]; then
  echo "<hr>Error en Script:"\
    "<br>el metodo de solicitud nos es GET"\
    "<br>Revise la declaracion de su formulario METHOD=\"GET\".<hr>"
  exit 1
fi

# If no search arguments, exit gracefully now.
if [ -z "$QUERY_STRING" ]; then
  exit 0
fi

echo "<br><hr /><br>"
echo "RESETEANDO EQUIPO: $(date)" 
echo '<br>'
echo '<br>'

reboot

echo '</body>'
echo '</html>'
