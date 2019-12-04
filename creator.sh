#!/bin/bash

#KONFIGURACJA WYSWIETLANIA
if [ ! -e "screen.ini" ]
then
  cp  "fasada-core/lib/fasada/_config_template/screen.ini" ./
fi
source "screen.ini"

$ECHO
$ECHO $COLOR1 "KREATOR INSTALACJI SYSTEMU FASADA DLA Linux-UBUNTU (polskojęzyczny)"
$ECHO $COLOR2 "(For english version use creator_en.sh)" $COLOR1
$ECHO
$ECHO "Informacje o systemie:" $COLOR2
uname -a
lsb_release -a
$ECHO $NORMCO

#NARZĘDZIA
TIMESTAMP=`date --utc +%Y%m%d_%H%M%SZ`

function pause(){
   $ECHO "$*"
   read -p "?"
}

#KONFIGURACJA FASADY
if [ ! -e "fasada.ini" ]
then
      $ECHO $COLOR1 "Brak pliku fasada.ini. Kopiowanie z zasobów..." $NORMCO
      cp  "fasada-core/lib/fasada/_config_template/fasada.ini" ./
      echo
      cat "fasada.ini"
      echo
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać i edytować plik)" $NORMCO
else
      $ECHO $COLOR2 "Wczytanie$COLOR1 fasada.ini" $NORMCO
fi
source "fasada.ini"

#WERSJA SYSTEMU
#https://unix.stackexchange.com/questions/88644/how-to-check-os-and-version-using-a-linux-command
source "/etc/lsb-release"
if [ -e "/etc/debian_version" ]
then
DEBIAN=`cat /etc/debian_version`
fi
#pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO

#POSZUKIWANIE SKŁADNIKÓW
$ECHO $COLOR2 "Szukam niezbędnych składników..." $NORMCO
CMAKE=`whereis cmake | cut --delimiter=' '   -f 2`
MAKE=`whereis make | cut --delimiter=' '   -f 2`
GPP=`whereis g++ | cut --delimiter=' '   -f 2`

$ECHO $COLOR2 "Przeszukano." $NORMCO

if [ ! -e "$CMAKE" ]
then
      $ECHO $COLOR1 "cmake jest konieczny. Zainstaluj ten program." $NORMCO
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO
else
      $ECHO $COLOR2 "cmake znaleziony:$COLOR1 $CMAKE" $NORMCO
fi

if [ ! -e "$MAKE" ]
then
      $ECHO $COLOR1 "make jest konieczny.  Zainstaluj ten program." $NORMCO
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO
else
      $ECHO $COLOR2 "make znaleziony:$COLOR1 $MAKE" $NORMCO
fi

if [ ! -e "$GPP" ]
then
      $ECHO $COLOR1 "g++  jest konieczny.  Zainstaluj ten program." $NORMCO
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO
else
      $ECHO $COLOR2 "g++ znaleziony:$COLOR1 $GPP" $NORMCO
fi
$ECHO $NORMCO

$ECHO $COLOR2 "AKTUALIZACJA ŹRÓDEŁ..." $NORMCO
git pull | tee git.log
$ECHO $COLOR2 "WYKONANO" 
$ECHO $NORMCO

$ECHO $COLOR2 "BUDOWANIE AKTUALNEJ WERSJI W KATALOGU $COLOR1 '${SOURCEDIR}'" $NORMCO
#pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1

pushd $SOURCEDIR
rm Makefile
$ECHO $COLOR1
CMAKEDEFS="-DDISTRIB_ID:STRING=$DISTRIB_ID -DDISTRIB_RELEASE:INTERNAL=$DISTRIB_RELEASE"
$CMAKE . $CMAKEDEFS #| tee cmake.log
$ECHO $NORMCO

if [ ! -e Makefile ]
then
      $ECHO $COLOR1 "Makefile nie został wygenerowany" $NORMCO
      exit
else
      $ECHO $COLOR2 "BUDOWANIE PROJEKTU ZA POMOCĄ $MAKE" $NORMCO
      $MAKE #| tee make.log
fi

if [ ! -e treeserver ]
then
      $ECHO $COLOR1 "Brak koniecznego pliku wykonywalnego treeserver" $NORMCO
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO
fi

if [ ! -e wwwserver ]
then
      $ECHO $COLOR1 "Brak koniecznego pliku wykonywalnego treeserver" $NORMCO
      pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $NORMCO
fi

popd
$ECHO $COLOR2 "WYKONANO"
$ECHO $NORMCO

if [ -e "stop.sh" ]
then
     $ECHO $COLOR1 "WYŁĄCZENIE SERWISU FASADA!!!"
     pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1
     ./stop.sh
fi

$ECHO $COLOR2 "POSZUKIWANIE I AKTUALIZACJA DANYCH Z PORTALI SPOŁECZNOŚCIOWYCH" $NORMCO
#pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1

pushd $PRIVATEDIR
$ECHO
$ECHO $COLOR2 "LISTA ARCHIWÓW W KATALOGU" $PRIVATEDIR $NORMCO
ls -lt *.zip
ZIPS=`ls -t *.zip`
$ECHO $COLOR2 "DO SPRAWDZENIA:" $COLOR1 $ZIPS
$ECHO $NORMCO

for file in $ZIPS
do 
      DIRNAME=`echo "$file" | cut -d'.' -f1`
      $ECHO $COLOR2 "\t" "ARCHIWUM" $COLOR1 $file 
      $ECHO $COLOR2 "\t" "ROZPAKOWUJE DO KATALOGU" $COLOR1 $DIRNAME $COLOR2

      if [ ! -e "$DIRNAME" ]
      then
	$ECHO $COLOR2 "\t" "TWORZĘ KATALOG" $COLOR1 $DIRNAME $NORMCO
	mkdir $DIRNAME
      fi

      $ECHO "\t" "ROZPAKOWYWANIE: $COLOR1 unzip -u $file -d $DIRNAME $NORMCO"
      unzip -u $file -d $DIRNAME
      $ECHO $COLOR2 "\t" "ROZPAKOWYWANIE ZAKOŃCZONE"
      $ECHO $NORMCO
done


BACKUP=".BACKUP"
if [ ! -e "$BACKUP" ]
then
  $ECHO $COLOR1 "TWORZĘ KATALOG" "$BACKUP" $NORMCO
  mkdir "$BACKUP"
fi 

if [ -e "index.json" ]
then
  $ECHO $COLOR2 "ZAPAMIĘTUJE POPRZEDNI PLIK $COLOR1 index.json" $NORMCO
  mv index.json "${BACKUP}/index$TIMESTAMP.json" 
  $ECHO $NORMCO
fi
$ECHO $NORMCO

$ECHO $COLOR2 "INDEKSOWANIE ZAWARTOŚCI KATALOGU" $PRIVATEDIR $NORMCO
#pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1
pwd
$ECHO $COLOR2
$BINDIR/indexer "." $INDEXPAR > indexer.log #| grep -v "#"  #$INDEXPAR may be --all, means ALL FILES!!!
$ECHO $COLOR1 "WYKONANO." $COLOR2 "Dokładna lista normalnych działań w pliku" $COLOR1 "indexer.log"
$ECHO $NORMCO

popd


$ECHO $COLOR2 "POSZUKIWANIE I INDEKSOWANIE WYKONANE" 
pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1
$ECHO $NORMCO

SKIN="$PRIVATEDIR/_skin/"
if [ ! -e "$SKIN" ]
then
  $ECHO $COLOR2 "TWORZĘ KATALOG $COLOR1 $SKIN $COLOR2 i JEGO OBOWIĄZKOWĄ ZAWARTOŚĆ" $NORMCO
  mkdir "$SKIN"
  cp fasada-core/lib/fasada/_skin_template/* "$SKIN"
fi
$ECHO $NORMCO

$ECHO $COLOR2 "TESTOWE ODPALENIE SERWISU" $COLOR1
pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1

if [ -e "output.fac" ]
then
   $ECHO $COLOR2 "ZABEZPIECZENIE OSTATNICH WYNIKÓW:$COLOR1  cp output.fac output$TIMESTAMP.fac $NORMCO"
   mv output.fac output$TIMESTAMP.fac
fi

$ECHO $COLOR2 "URUCHOMIENIE SERWISU..." $NORMCO
#pause $COLOR2 "KONTYNUOWAĆ? (ENTER lub Ctrl-C żeby przerwać)" $COLOR1
$ECHO
indexpath=`realpath "$PRIVATEDIR/index.json"`

$ECHO $COLOR1 "treeserver" "$COLOR2$indexpath" "--force" $NORMCO
$BINDIR/treeserver $indexpath --force &
sleep 5
pause $COLOR2 "Naciśnij ENTER" $NORMCO

$ECHO
$ECHO $COLOR1 "wwwserver localhost $COLOR2 $PRIVATEPORT $PRIVATEDIR" $NORMCO
$BINDIR/wwwserver localhost $PRIVATEPORT $PRIVATEDIR > wwwserver.log &
sleep 2
pause $COLOR2 "Naciśnij ENTER" $NORMCO

$ECHO $COLOR1 $browser "http:localhost:$PRIVATEPORT/?ls&html&long" $NORMCO
$browser "http:localhost:$PRIVATEPORT/?ls&html&long"
sleep 1
$ECHO
pause $COLOR2 "Nacisniej ENTER gdy skończysz testowanie" $NORMCO

$browser "http:localhost:$PRIVATEPORT/!!!!"
$ECHO $COLOR2 "ZAKOŃCZONE"
$ECHO $NORMCO

cat > start.sh <<EOF 
#!/bin/bash
#STARTING FASADA 
if [ -e "output.fac" ]
then
   echo "Kontynuuje ze znalezionego pliku output.fac" 
   cp output.fac $PRIVATEDIR/input.fac
   ${BINDIR}treeserver $PRIVATEDIR/input.fac -  > treeserver.log 2>&1 &
else
   echo "Rozpoczęcie z pliku indeksu: $indexpath"
   ${BINDIR}treeserver $indexpath -  > treeserver.log 2>&1 &
fi
sleep 5
${BINDIR}wwwserver localhost $PRIVATEPORT $PRIVATEDIR  > wwwserver.log 2>&1 &
sleep 2
$browser "http:localhost:$PRIVATEPORT/?ls&html&long"
EOF

cat > stop.sh <<EOF 
#!/bin/bash
#FINISHING FASADA - WHEN WWWSERVER STOPS, ALL ATHERS PARTS ALSO EXIT
$browser "http:localhost:$PRIVATEPORT/!!!!"
EOF

chmod +x *.sh

wait
$ECHO $COLOR1 "NIE MAM JUŻ NIC WIĘCEJ DO ZROBIENIA" $NORMCO

