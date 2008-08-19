#!/bin/bash
#
# Quand il marchera, ce prog fournira les adresses des symboles des
# librairies. Mais il ne marche pas.
#
# $Id: symbols.sh,v 1.1 2004-09-08 23:16:49 lalet Exp $

# usage
LIBRARY=$1
BINARY=$2

offset=`ldd $BINARY | grep $LIBRARY | sed 's/.*(// ; s/).*//'`

i=0
#for ligne in `objdump -T $LIBRARY | awk '{print $1":"$7}' | grep -v '^:$'`
for ligne in `nm -D $LIBRARY | awk '{print $1":"$3}'`
  do
  address=`echo $ligne | cut -d ':' -f 1`
  symbol=`echo $ligne | cut -d ':' -f 2`
  if [ "X$symbol" != "X" -a "$address" != "00000000" ]; then
      echo "address[$i]=0x$address + $offset;"
      echo "symbol[$((i++))]=\"$symbol\";"
  fi
done
