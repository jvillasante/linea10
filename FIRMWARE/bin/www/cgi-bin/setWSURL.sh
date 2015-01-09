#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio de URL de los Web Services </title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>INTERFAZ PARA MODIFICAR IP Y PUERTO DE LOS WEB SERVICES</td></tr>'\
  '<tr><td><strong>NOTA: EL SISTEMA SE REINICIARA LUEGO DE REALIZAR ESTA ACCION</strong></td></tr>'\
  '<tr><td>WS IP:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <input type="text" name="m_wsip" size=24></td></tr>'\
  '<tr><td>WS PUERTO:&nbsp;&nbsp; <input type="text" name="m_wspuerto" size=24></td></tr>'\
  '<tr><td>WS CLIENTE:&nbsp; <input type="text" name="m_wscliente" size=80></td></tr>'\
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
  xWSIP=`echo "$QUERY_STRING" | sed -n 's/^.*m_wsip=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`
  xWSPuerto=`echo "$QUERY_STRING" | sed -n 's/^.*m_wspuerto=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`
  xWSCliente=`echo "$QUERY_STRING" | sed -n 's/^.*m_wscliente=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  echo "SE CAMBIARA EL IP Y PUERTO DE CADA WEB SERVICES COMO SIGUE:"
  echo '<br>'
  echo "WS_IP: " $xWSIP
  echo '<br>'
  echo "WS_PUERTO: " $xWSPuerto
  echo '<br>'
  echo "WS_CLIENTE: " $xWSCliente
  echo '<br>'
  echo '<br>'
  
  sed -i "s/wsIP=.*/wsIP=\"$xWSIP\"/" /mnt/jffs2/app.ini
  sed -i "s/wsPort=.*/wsPort=\"$xWSPuerto\"/" /mnt/jffs2/app.ini
  
  if [ $EQUIPO = "TEMPO" ]; then
    xWSClienteUpdate="${xWSCliente//HORUS9/Update}"
    sed -i "s/wsFirmwareUpdateURL=.*/wsFirmwareUpdateURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSClienteUpdate\/UpdateLinea9.asmx\"/" /mnt/jffs2/app.ini
    sed -i "s/wsCargaMasivaURL=.*/wsCargaMasivaURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/WS_HORUS9.asmx?op\=getCargaMasivaM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsSincronizacionURL=.*/wsSincronizacionURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/WS_HORUS9.asmx?op\=SincronizacionmarcasM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsVerificaPersonaURL=.*/wsVerificaPersonaURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/WS_HORUS9.asmx?op\=VerificaPersonaM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsEnrollURL=.*/wsEnrollURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/WS_HORUS9.asmx?op\=enrollM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsAlarmasURL=.*/wsAlarmasURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/WS_HORUS9.asmx?op\=alarma\"/" /mnt/jffs2/app.ini
  elif [ $EQUIPO = "SNACK" ]; then
    xWSClienteUpdate="${xWSCliente//SNACK9/Update}"
    sed -i "s/wsFirmwareUpdateURL=.*/wsFirmwareUpdateURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSClienteUpdate\/UpdateLinea9.asmx\"/" /mnt/jffs2/app.ini
    sed -i "s/wsCargaMasivaURL=.*/wsCargaMasivaURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/SNACK9.asmx?op\=getCargaMasivaM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsSincronizacionURL=.*/wsSincronizacionURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/SNACK9.asmx?op\=SincronizacionMarcasM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsVerificaPersonaURL=.*/wsVerificaPersonaURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/SNACK9.asmx?op\=VerificaPersonaM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsEnrollURL=.*/wsEnrollURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/SNACK9.asmx?op\=enrollM\"/" /mnt/jffs2/app.ini
    sed -i "s/wsAlarmasURL=.*/wsAlarmasURL=\"http:\/\/$xWSIP:$xWSPuerto\/$xWSCliente\/SNACK9.asmx?op\=alarma\"/" /mnt/jffs2/app.ini
  fi
  
  echo "MODIFICADO EXITOSAMENTE WS IP:" $xWSIP
  echo '<br>'
  echo "MODIFICADO EXITOSAMENTE WS PUERTO:" $xWSPuerto
  echo '<br>'
  echo "MODIFICADO EXITOSAMENTE WS CLIENTE:" $xWSCliente
  echo '<br>'
  echo '<br>'
  
  echo "La aplicacion se reiniciara en aproximadamente 5 segundos..."
  sleep 5
  reboot
fi

echo '</body>'
echo '</html>'
