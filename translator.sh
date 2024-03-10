#!/bin/bash
# -d file           Проверяет, существует ли файл, и является ли он директорией.
# -e file           Проверяет, существует ли файл.
# -f file           Проверяет, существует ли файл, и является ли он файлом.
# -r file           Проверяет, существует ли файл, и доступен ли он для чтения.
# -s file           Проверяет, существует ли файл, и не является ли он пустым.
# -w file           Проверяет, существует ли файл, и доступен ли он для записи.
# -x file           Проверяет, существует ли файл, и является ли он исполняемым.
# file1 -nt file2   Проверяет, новее ли file1, чем file2.
# file1 -ot file2   Проверяет, старше ли file1, чем file2.
# -O file           Проверяет, существует ли файл, и является ли его владельцем текущий пользователь.
# -G file           Проверяет, существует ли файл, и соответствует ли его идентификатор группы идентификатору группы текущего пользователя.

color_grey="\033[1;30m"
color_red="\033[1;31m"
color_green="\033[1;32m"
color_yellow="\033[1;33m"
color_blue="\033[1;34m"
color_magenta="\033[1;35m"
color_cyan="\033[1;36m"
color_white="\033[1;37m"
color_reset="\033[1;0m"

fail_run="\nRunning failed."

#Check for zero args
if [ $# -eq 0 ] 
    then 
    echo -e $color_red"No arguments passed in translator.$fail_run"$reset
    exit 0  
fi
#Check for 1 argument
if [ $# -eq 1 ]
then 
    echo -e "Source file: $1"
    if [[ -f $1 ]];
    then
        if [ -f "./FrontEnd/frontend.exe" ]
        then
            ./FrontEnd/frontend.exe ./.tmp $1
        else
            echo -e $color_red"Frontend program not found.$fail_run" $reset
        fi
    else
        echo -e $color_red"File not found or its not a file.$fail_run" $reset
    fi
fi
#Check for 2 arguments
#TODO: options
