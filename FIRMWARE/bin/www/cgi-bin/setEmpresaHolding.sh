#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio de la empresa HOLDING </title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>DEFINIR EL NOMBRE DE LA EMPRESA HOLDING</TD></tr>'\
  '<tr><td><strong>NOTA: EL SISTEMA SE REINICIARA LUEGO DE REALIZAR ESTA ACCION</strong></td></tr>'\
  '<tr><td>EMPRESA:</TD><TD><input type="text" name="m_holding" size=12></td></tr>'\
  '</tr></table>'


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
  xHolding=`echo "$QUERY_STRING" | sed -n 's/^.*m_holding=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  match=+
  repl=' '
  xHolding="${xHolding//$match/$repl}"

  echo "SE CAMBIARA LA EMPRESA HOLDING COMO SIGUE:"
  echo '<br>'
  echo "EMPRESA HOLDING: " $xHolding
  echo '<br>'
  echo '<br>'

  sed -i "s/empresaHolding=.*/empresaHolding=\"$xHolding\"/" /mnt/jffs2/app.ini

  echo "MODIFICADA EXITOSAMENTE EMPRESA HOLDING"
  echo '<br>'
  echo '<br>'

  echo "La aplicacion se reiniciara en aproximadamente 5 segundos..."
  sleep 5
  reboot
fi
echo '</body>'
echo '</html>'
