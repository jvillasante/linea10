#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio del IP del NTP </title>'
echo '</head>'
echo '<body>'

NTP_IP=$(sed -n 's/^ntpIP*=*\([^ ]*.*\)/\1/p' < /mnt/jffs2/app.ini)
echo "<strong>IP DEL NTP ACTUAL:</strong> " $NTP_IP

echo "<br><hr /><br>"

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>MODIFICAR EL IP PARA ACCESO A NTP</td></tr>'\
  '<tr><td><strong>NOTA: EL SISTEMA SE REINICIARA LUEGO DE REALIZAR ESTA ACCION</strong></td></tr>'\
  '<tr><td>NTP IP: <input type="text" name="m_ntpIP" size=24> <input type="submit" value="Guardar" /></td></tr>'\
  '</table></form>'

echo "<br><hr /><br>"

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<input type="hidden" name="syncNTP" value="1">'\
  '<table nowrap>'\
  '<tr><td>SINCRONIZAR NTP</td></tr>'\
  '<tr><td><strong>NOTA: EL SISTEMA TRATARA DE SINCRONIZAR LA FECHA CON EL SERVIDOR NTP</strong></td></tr>'\
  '<tr><td><input type="submit" value="Sincronizar Ahora" /></td></tr>'\
  '</table></form>'

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
  
  xNTPIP=`echo "$QUERY_STRING" | sed -n 's/^.*m_ntpIP=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  if [ -z $xNTPIP ]; then
    NTP_IP=$(sed -n 's/^ntpIP*=*\([^ ]*.*\)/\1/p' < /mnt/jffs2/app.ini)
    echo "Invocando NTP con IP: " $NTP_IP " para sincronizaci&oacute; de la fecha.<br>"
    NTP_RESPONSE=$(ntpd -d -n -q -p $NTP_IP 2>&1)
    echo "<pre>$NTP_RESPONSE</pre>"
  else 
    echo "SE CAMBIARA EL IP DEL NTP COMO SIGUE:"
    echo '<br>'
    echo "NTP IP: " $xNTPIP
    echo '<br>'

    sed -i "s/ntpIP=.*/ntpIP=$xNTPIP/" /mnt/jffs2/app.ini

    echo "MODIFICADO EXITOSAMENTE EL IP DEL NTP"
    echo '<br>'

    echo "La aplicacion se reiniciar&aacute; en aproximadamente 5 segundos..."
    sleep 5
    reboot
  fi
fi

echo '</body>'
echo '</html>'
