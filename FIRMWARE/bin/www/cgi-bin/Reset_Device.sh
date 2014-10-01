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
  '<table nowrap>'\
  '<tr><td>Opcion para Resetear el Dispositivo</TD></tr>'\
  '</tr></table>'


echo '<br><input type="submit" value="Reset"></form>'

# Make sure we have been invoked properly.

if [ "$REQUEST_METHOD" != "GET" ]; then
  echo "<hr>Error en Script:"\
    "<br>el metodo de solicitud nos es GET"\
    "<br>Revise la declaracion de su formulario METHOD=\"GET\".<hr>"
  exit 1
fi

# If no search arguments, exit gracefully now.


echo '<br>'
echo "RESETEANDO EQUIPO: "
echo '<br>'
echo '<br>'

LOGFILE=/mnt/jffs2/TEMPO_LOG.log



echo "Reseteando_Equipo:" >>date>>$LOGFILE 
echo '<br>'
echo '<br>'

reboot



echo '</body>'
echo '</html>'
