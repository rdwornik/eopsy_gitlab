#!/bin/bash

echo "Genereating folder with sample files"
./generate.sh boo
echo "ERROR HANDLING"
echo
echo "TEST: empty input"
echo "./modify.sh"
./modify.sh
echo '========================='
echo "TEST: only flags inserted"
echo "./modify.sh -l"
./modify.sh -l
echo '========================='
echo "TEST: only recursive flag inserted"
echo "./modify.sh -r"
./modify.sh -r
echo '========================='
echo "TEST: flag with no wrongly typed dir"
echo "./modify.sh -u /boo boo01"
./modify.sh -u /boo boo01
echo '========================='
echo "TEST: flag with no file as parameter"
echo "./modify.sh -u boo"
./modify.sh -u boo
echo '========================='
echo "TEST: wrong sed pattern"
echo "./modify.sh s/boo/foo boo boo01"
./modify.sh s/boo/foo boo boo01
echo '========================='
echo "TEST: no parameter after setting flag"
echo "./modify.sh -l"
./modify.sh -l
echo '========================='


echo "SUCCESS CASE HANDLING"
echo '========================='
echo "TEST: help message"
echo "./modify.sh -h"
./modify.sh -h 
echo '========================='
echo "TEST: to upprecase NO Recursive"
echo "./modify.sh -u boo boo01 boo02 boo03 boo04
ls -F boo"
./modify.sh -u boo boo01 boo02 boo03 boo04
ls -F boo
echo '========================='
echo "TEST: to lowercase NO Recursive"
echo "./modify.sh -l boo boo01"
./modify.sh -l boo BOO01 BOO02
ls -F boo
echo '========================='
echo "TEST: to upprecase WITH Recursive tree before exec"
tree
echo "./modify.sh -r -u boo boo01 boo02 boo03 boo04
ls -F boo"
./modify.sh -r -u boo boo01 boo02 boo03 boo04
echo "tree after"
tree
echo '========================='
echo "TEST: to to lower case WITH Recursive tree before exec"
tree
echo "./modify.sh -r -u boo BOO01 BOO02
ls -F boo"
./modify.sh -r -l boo BOO01 BOO02
echo "tree after"
tree
echo '========================='
echo "TEST: NO recursive sed pattern case"
echo "./modify.sh s/niemcy/francja/ boo boo01 boo02"
./modify.sh s/niemcy/francja/ boo boo01 boo02
echo '========================='
echo "TEST: NO recursive sed pattern case 2 with flag g"
echo "./modify.sh s/niemcy/francja/g boo boo02"
./modify.sh s/niemcy/francja/g boo boo1 boo02
echo '========================='
echo "TEST: WITH recursive sed pattern case"
echo "./modify.sh s/niemcy/francja/ boo boo01 boo02"
./modify.sh -r s/niemcy/francja/ boo boo01 boo02
echo '========================='
echo "TEST: WITH recursive sed pattern case"
echo "./modify.sh s/niemcy/francja/ boo boo01 boo02"
./modify.sh -r s/niemcy/francja/ boo boo01 boo02
echo "TEST: no parameter after setting flag"
