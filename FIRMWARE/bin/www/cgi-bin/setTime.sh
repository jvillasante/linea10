#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio de la fecha y hora del dispositivo </title>'
echo '</head>'
echo '<body>'

if [ -f /usr/local/bin/Resources/ntp_is_running ]; then
  echo "NTP actualiza la fecha y hora autom&aacute;ticamente.<br>"
  echo "<strong>NTP est&aacute corriendo en este sistema.</strong><br>"
  echo "No es necesario actualizar la fecha y hora."
else
  echo "<form method=GET action=\"${SCRIPT}\">"\
    '<table nowrap>'\
    '<tr><td>Formato de la fecha a especificar (MMddhhmmccyy) </TD></tr>'\
    '<tr><td>MM: Mes (num&eacute;rico de dos d&iacute;gitos) </TD></tr>'\
    '<tr><td>dd: D&iacute;a (num&eacute;rico de dos d&iacute;gitos) </TD></tr>'\
    '<tr><td>hh: Hora (num&eacute;rico de dos d&iacute;gitos) </TD></tr>'\
    '<tr><td>mm: Minutos (num&eacute;rico de dos d&iacute;gitos) </TD></tr>'\
    '<tr><td>cc: Primeros dos d&iacute;gitos del a&ntilde;o </TD></tr>'\
    '<tr><td>yy: Ultimos dos d&iacute;gitos del a&ntilde;o </TD></tr>'\
    '<tr><td>Fecha y Hora Nueva:</TD><TD><input type="text" name="m_date" size=14></td></tr>'\
    '</table>'
  echo '<br><input type="submit" value="Guardar"></form>'
  
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
    # No looping this time, just extract the data you are looking for with sed:
    xDate=`echo "$QUERY_STRING" | sed -n 's/^.*m_date=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

    echo "SE CAMBIARA LA FECHA Y HORA DEL EQUIPO COMO SIGUE<br>"
    echo "Fecha y Hora (MMddhhmmccyy): " $xDate
    echo '<br>'
    echo '<br>'

    # date $xDate
    # hwclock --systohc
    ./SetTimeKairosLocal $xDate
  fi
fi

echo '</body>'
echo '</html>'
