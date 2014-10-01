#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Definicion Interfaz de Red de HORUS9</title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>IP:</TD><TD><input type="text" name="m_ip" size=12></td></tr>'\
  '<tr><td>MASCARA:</td><td><input type="text" name="m_mask" size=12 value=""></td>'\
  '<tr><td>GATEWAY:</td><td><input type="text" name="m_gw" size=12 value=""></td>'\
  '</tr></table>'


echo '<br><input type="submit" value="Guardar">'\
  '<input type="reset" value="Reset"></form>'

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
  xIP=`echo "$QUERY_STRING" | sed -n 's/^.*m_ip=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`
  xMASK=`echo "$QUERY_STRING" | sed -n 's/^.*m_mask=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`
  xGW=`echo "$QUERY_STRING" | sed -n 's/^.*m_gw=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`
  echo "SE SETEARA LA INTERFAZ DE RED DEL EQUIPO COMO SIGUE:"
  echo "IP: " $xIP
  echo '<br>'
  echo "MASCARA: " $xMASK
  echo '<br>'
  echo "GATEWAY: " $xGW

  setip  -ip $xIP; setip -mask $xMASK;setip -gw $xGW
  echo "MODIFICADA EXITOSAMENTE INTERFAZ DE RED " 
  reset 
fi
echo '</body>'
echo '</html>'



exit 0
