#ifndef COLORS_H
#define COLORS_H

#define GREY "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[1;0m"
//for((i=0;i<256;i++)); do for((j=0;j<256;j++)); do echo -e "color $i $j: \e[${i};${j}m test  \e[1;0m"; done; done
#endif