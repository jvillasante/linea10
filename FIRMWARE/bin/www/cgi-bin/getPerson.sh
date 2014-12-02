#!/bin/ash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>Interfaz para listas las Personas Habilitadas </title>'
echo '</head>'
echo '<body>'

echo "<form method=GET action=\"${SCRIPT}\">"\
  '<table nowrap>'\
  '<tr><td>BUSCAR PERSONA : </TD></tr>'\
  '<tr><td>RUT con Digito Verificador:</TD><TD><input type="text" name="m_rut" size=20></td></tr>'\
  '</table>'

echo '<br><input type="submit" value="Buscar"></form>'

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
  xRUT=`echo "$QUERY_STRING" | sed -n 's/^.*m_rut=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"`

  echo "RUT DE LA PERSONA A BUSCAR :" $xRUT
  echo '<br>'
  echo '<br>'

  echo "IMPRIMIENDO COINCIDENCIAS DE LA BUSQUEDA " 
  echo '<br>'
  echo '<br>'
  echo "identificador|nombre|rut"
  echo '<br>'
  echo '<br>'

  Id=""
  for f in `sqlite3 /mnt/jffs2/iengine.db 'select userid from iengine_tags where value = '\"$xRUT\"' '`; do
    Id=`echo $f | awk '{split($0,a,"|"); print a[1]}'`
  done

  if [ -z "$Id" ]; then
    echo "No existen usuarios con ese RUT: " $xRUT
    exit 0
  fi
  
  for f in `sqlite3 /mnt/jffs2/iengine.db 'select name, value from iengine_tags where userid = '\"$Id\"' '`; do
    # Parsing data (sqlite3 returns a pipe separated string)
    Name=`echo $f | awk '{split($0,a,"|"); print a[1]}'`
    Value=`echo $f | awk '{split($0,a,"|"); print a[2]}'`

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
fi

echo "<hr />"
echo "<a href="../web_kairos.html">Regresar a EpiGenesis</a>"

echo '</body>'
echo '</html>'
