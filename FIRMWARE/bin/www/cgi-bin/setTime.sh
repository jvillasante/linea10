#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio de la fecha y hora del dispositivo </title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>Formato de la fecha a especificar (MMddhhmmccyy) </td></tr>'\
  '<tr><td>MM: Mes (num&eacute;rico de dos d&iacute;gitos) </td></tr>'\
  '<tr><td>dd: D&iacute;a (num&eacute;rico de dos d&iacute;gitos) </td></tr>'\
  '<tr><td>hh: Hora (num&eacute;rico de dos d&iacute;gitos) </td></tr>'\
  '<tr><td>mm: Minutos (num&eacute;rico de dos d&iacute;gitos) </td></tr>'\
  '<tr><td>cc: Primeros dos d&iacute;gitos del a&ntilde;o </td></tr>'\
  '<tr><td>yy: Ultimos dos d&iacute;gitos del a&ntilde;o </td></tr>'\
  '<tr><td><strong>Fecha y Hora Nueva:</strong> <input type="text" name="m_date" size=24></td></tr>'\
  '</table>'
echo '<br><input type="submit" value="Guardar"></form>'

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
else
  echo "<br><hr /><br>"
  
  # No looping this time, just extract the data you are looking for with sed:
  xDate=`echo "$QUERY_STRING" | sed -n 's/^.*m_date=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  echo "SE CAMBIARA LA FECHA Y HORA DEL EQUIPO COMO SIGUE<br>"
  echo "Fecha y Hora (MMddhhmmccyy): " $xDate
  echo '<br>'
  echo '<br>'

  ./SetTimeKairosLocal $xDate
  echo "<br><br>"
  sleep .5 

  NTP_IP=$(sed -n 's/^ntpIP*=*\([^ ]*.*\)/\1/p' < /mnt/jffs2/app.ini)
  echo "Invocando NTP con IP: " $NTP_IP " para sincronizaci&oacute; de la fecha.<br>"
  NTP_RESPONSE=$(ntpd -d -n -q -p $NTP_IP 2>&1)
  echo "<pre>$NTP_RESPONSE</pre>"
fi

echo '</body>'
echo '</html>'
