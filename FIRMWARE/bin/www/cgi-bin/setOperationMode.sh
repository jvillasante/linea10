#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para el cambio del MODO DE OPERACION DEL EQUIPO</title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>DEFINIR MODO OPERACION DEL EQUIPO(OFFLINE|ONLINE):</TD></tr>'\
  '<tr><td>Modo Operacion:</TD><TD><input type="text" name="m_operate" size=12></td></tr>'\
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
  xOperate=`echo "$QUERY_STRING" | sed -n 's/^.*m_operate=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  echo "SE CAMBIARA EL MODO DE OPERACION COMO SIGUE:"
  echo '<br>'
  echo "MODO OPERACION: " $xOperate
  echo '<br>'
  echo '<br>'

  LOGFILE=/mnt/jffs2/KAIROS_LOG.log



  sqlite3 /mnt/jffs2/Gen_Config.sql 'update gen_config_equipo set valueconfig='\"$xOperate\"',active=1 where paramconfig="MODO_OPERACION_EQUIPO"'



  echo "MODIFICADO MODO OPERACION :" $xHolding>>$LOGFILE 
  echo '<br>'
  echo '<br>'

  echo "MODIFICADO EXITOSAMENTE MODO OPERACION " 
  echo '<br>'

  echo '<br>'
  echo "Se registra resultado en log :" $LOGFILE
  echo '<br>'
  echo '<br>'



  echo "IMPRIMIENTO PARAMETROS DE CONFIGURACION" 
  echo '<br>'
  echo '<br>'
  echo "idconfig|origenconfig|paramconfig|valueconfig|isconfig|active|"

  for f in ` sqlite3 /mnt/jffs2/Gen_Config.sql 'select idconfig,origenconfig,paramconfig,valueconfig,isconfig,active from gen_config_equipo'`; do

    echo '<br>'
    echo "$f"
    echo '<br>'
  done


fi
echo '</body>'
echo '</html>'
