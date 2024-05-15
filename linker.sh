#!/bin/bash
#====================================================
#               To assemble file and link it        #
#====================================================

ext_obj=".o"
code_file=""
name_file=""    #name of file without extension
obj_file=""    #tree file created from source
exec=""
#=================================
langfunc="./Lib/MyLangConfig/stdlib.s"
langobj="./Lib/MyLangConfig/stdlib.o"

mathfunc="./Lib/MyLangConfig/math.s"
mathobj="./Lib/MyLangConfig/math.o"
#=================================
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
#=================================

code_file=$1

str=$code_file
substr="."
#reverse strings
reverse_str=$(echo $str | rev)
reverse_substr=$(echo $substr | rev)
#find index of reversed substring in reversed string
prefix=${reverse_str%%$reverse_substr*}
reverse_index=${#prefix} 
#calculate last index
pos_dot=$(( ${#str} - ${#substr} - $reverse_index ))

#check if no dot
if [ $pos_dot -eq -1 ]
then
name_file=$code_file
obj_file=${name_file}${ext_obj}
else
name_file_arr=()       #else create name_of tree
curr_pos=0
name_file=""
while [ $curr_pos -ne $pos_dot ];
do
    name_file_arr[$curr_pos]="${code_file:curr_pos:1}"
    curr_pos=$((curr_pos+1))
done
printf -v name_file '%s' "${name_file_arr[@]}"
obj_file=${name_file}${ext_obj}
fi

exec=$name_file

#===============================================================

if [ $# -eq 1 ]
then 
    if [[ -f $1 ]];
    then
        nasm -f elf64 $code_file -o $obj_file
        nasm -f elf64 $langfunc -o $langobj
        nasm -f elf64 $mathfunc -o $mathobj
        gcc $obj_file $langobj $mathobj -o $exec -no-pie
        $exec
    else
        echo -e $color_red"File not found or its not a file.$fail_run" $reset
    fi
fi