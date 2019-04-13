#!/bin/bash


sedpattern="s/\w+/\w*/(g|p|\d+|)" #ustawiamy pattern sed do którego będziemy przyrównywać poprawność pattern podanego przez użytkownika
name=`basename $0` #przechowujemy nazwe wywołanego skryptu bez ścieżki

error_msg()  #funkcja error message wyświetla wiadomość podaną w argumnecie jako error
{ 
        echo "$name: error: $1" 1>&2  #przeadrsowanie strumienia stdout do stderr
}
#funkcja do wysyłania helpa
helpmsg(){ #cat wyświetla załadowanego do niego stringa, między parametrami pisanymi z wielkie litery może wpisywać każdą wartość która będzie załadowana 
cat<<EOT 1>&2       

Write a script modify with the following syntax:

  modify [-r] [-l|-u] <dir/file names...>
  modify [-r] <sed pattern> <dir/file names...>
  modify [-h]

  Proper sed pattern is "s/\w+/\w+/(g|p|\d+|)"
  
  modify_examples

EOT
}

#główna funkcja zmieniająca nazwę pliku przyjmuje 4 argumenty
modify(){
    local rec="$1" #rekurencja pusty string albo maxdepth 1
    local flag="$2" #flaga ustawiona lub sed pattern
    local dir="$3" #ścieżka do folderu z plikami
    local filesinput="$4" #nazwa pliku

    local messeges=`find $dir $rec -type f -name "$filesinput.*" -print 2>&1 ` #szukamy pliku z podaną nazwą ładuje do parametru messeges
    if [[ -z "$messeges" ]]; #sprawdzamy messege pusty jeśli tak nie ma takiego pliku 
    then
            error_msg "no such a file in directory"
    else     #żeby działało na foldera trzeba usunąć type -f 
        find $dir $rec -type f -name "$filesinput.*" -print0 | while IFS= read -r -d '' file; #wywołujemy find i ładujemy przez pipe'a output do pętli IFS= ustawiam seperator na null 
        do                                                                                    #flaga -r pomija backslash jako special char, flaga -d '' ustawiamy żeby rozpoznać nową linie który zaczyna input 
            dir=`dirname "$file"`           #łapiemy ściężkę do pliku                       
            filename=`basename "$file"`     #łapiemy plik bez ścieżki
            name="${filename%.*}"
            extension=$([[ "$filename" = *.* ]] && echo ".${filename##*.}" || echo '') #alternarytwa ifa jeśli jest ucianamy wszystko do kropki albo jeśli nie ma do pusty string 
    
            #case przyrównuje flage 
            case "$flag" in
            upper) 
            name="${name^^}"         #parametr do uppercase'a
            mv -v $file "$dir/$name$extension"
            ;;
            lower)
            name="${name,,}"        #parametr do lowercase'a
            mv -v "$file" "$dir/$name$extension"
            ;;
            *) 
            sed "$flag" "$file"     #sed z patternem fladze na file'u
            ;;
        
            esac
            
        done
    fi
}


#flaga rekurencyjna ustawiamy z defauflu na mexdepth 1 czyli find szuka tylko na poziomie folederu
rec="-maxdepth 1"
flag=""
dir=""
#pętla do zczytywania parametrów opcji na inputu 
#używamy funkcji getopts która automatycznie wykrywa prametry z opcją podane po getopts czyli w tym szukamy hrul inaczej error nie ma takiej opcji
#i ładuje do opt, dwukropek po literze oznacza że musimy być parameter po litrze inaczej error
while getopts hru:l: opt  
do
    case "$opt" in  #case w którym łapiemy flagi i ustawiamy
        h) echo "Found the -h option" 
        helpmsg
        exit 1;;
        r)
        echo "Found the -r option" 
        rec="";;
        u) 
        echo "Found the -u option $OPTARG" #OPTARGu przechowujemy kolejny argument 
        dir=$OPTARG
        flag="upper"
        break;;
        l)
        option="-l" 
        echo "Found the -l option $OPTARG"
        dir=$OPTARG
        flag="lower"
        break;;
        *)
        #if wrong option is chosen exit
        echo "Unknown option"
        exit 1;;
    esac
done
#jeśli nie wybierzemy żadnej flagi sprawdzamy pattern

#generalnie $OPTIND $OPTARG to wewnętrzne parametry ustawiane przez funkce getopts
#shift do parametrów 
shift $[ $OPTIND - 1 ]

#jeśli flaga ustawiona na zero to sprawdzamy co mogło pójśc nie tak żeby wyświetlić poprawny error msg
if [ -z "$flag" ]; then     
    if [ -z "$1" ]; then            #jeśli parametr równa się null to znaczy że nic nie zostało podane      
    error_msg "no arguments inserted"
    exit 1
    fi
    if [[ ! "$1" =~ $sedpattern ]]; then    #jeśli paramatr nie spełnia sed pattern informujemy użytkownika o tym 
    error_msg "no such a sed pattern"
    exit 1
    fi
fi

#sprawdzamy czy flaga ustawiona na zero i czy sed pattern się zgadza
if [ -z "$flag" ] && [[ "$1" =~ $sedpattern ]]; then
    flag="$1"       #jeśli tak to shiftujemy do argumentu który wyznacza folder do pliku
    shift
    dir="$1"            
    if [ -z "$dir" ]; then          #jeśli null to znaczy że nie podaliśmy folderu w którym chcemy uruchomić funkcje
    error_msg "no directory to search in"
    exit 1
    else
    #shift do parametrów które są nazwami plików do zamiany
    shift
    fi
fi


#sprawdzamy czy użytkownik podał parametry po tym jak sprawdziliśmy czy podał ścieżką do plików jeśli nie informujemy o tym

if [ -z "$1" ];then
    error_msg "no file names were given"
    exit 1
fi

#jeśli po sprawdzeniu wszystkich przypadków kod działa wywołujemy dla każdego parametru funkcję modify w pętli for która w $@ przechowuje wszystkie argumenty
count=1
for param in "$@"       
do
echo "Parameter $count: $param"
count=$[ $count + 1 ]
modify "$rec" "$flag" "$dir" "$param"
done

