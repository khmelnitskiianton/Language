# MyLanguage

Project of translation system - from my language to NASM

It support only Linux x86-64.

It consists of FrontEnd, MiddleEnd, BackEnd. It translate code on my language to code on NASM:
+ FrontEnd: parses code to tree based recursive descent
+ MiddleEnd: optimizes tree
+ BackEnd: translates tree to code on NASM

## Table of Contents

- [MyLanguage](#mylanguage)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
  - [Dependent Objects](#dependent-objects)
  - [Language](#language)
  - [Standard](#standard)
  - [FrontEnd](#frontend)
  - [MiddleEnd](#middleend)
  - [Logs](#logs)

## Installation

```bash
git clone https://github.com/khmelnitskiianton/MyLanguage.git   #clone repo
cd ./MyLanguage
cmake .                                                         #compile language
make
chmod +x translator.sh                                          #permissions for scripts
chmod +x linker.sh
./translator.sh {path, begin with this repo}/{source code}      #translate lang code to nasm
./linker.sh {path, begin with this repo}/{NASM .s code}         #link nasm code with language's standart libs
#run program
```

It will be create folder `build`, there will be logs, and intermediate files

## Dependent Objects

Compiler - gcc. Uses - Cmake. Logs using GraphViz to visualize binary tree!

```bash
sudo apt update && sudo apt upgrade     #update
sudo apt install build-essential        #gcc
sudo apt install nasm                   #nasm
sudo apt install make                   #makefile
sudo apt install cmake                  #cmake
sudo apt install graphviz               #graphviz
```

## Language

*Rules and Grammar of my Language:*

[```Grammar+Language```](https://github.com/khmelnitskiianton/MyLanguage/blob/main/Language.md)

*Example of code:*

```cpp
void main(){
    var x;
    var x = 0;
    var y = (2+x) - 4.2;
    var a = summ(x, y);
    if (a) {
        print(x);
    }
    else
    {
        puts(10, 0);
    }
    while (y < 5){
        print(y);
        var y = y + 1;
    }
}

var summ(var a, var b){
    return a+b;
}
``` 
*Some examples of programs on my language*:

1. [factorial.lang](https://github.com/khmelnitskiianton/MyLanguage/blob/main/examples/factorial.lang)
2. [quadratka.lang](https://github.com/khmelnitskiianton/MyLanguage/tree/main/examples/quadratka.lang)

*Description of current functional*:

1. Language has pseudo double numbers, that have fixed point(In assembler: 3.14 &#8801; 314 with inaccuracy .00). It changes in settings in [Lib/MyLangConfig](https://github.com/khmelnitskiianton/MyLanguage/tree/main/Lib/MyLangConfig) constants INACCURACY in `.h` and `.s` files.
2. Now language has unary minus and plus operations(-(x+y)).
3. Language initializes all variables with 0. It understand unassigned variables. 
4. Language has limits for length of variable names and amount of them. Now it set to 100, but you can change it in [Lib/tree.h](https://github.com/khmelnitskiianton/MyLanguage/blob/main/Lib/tree.h)
5. Language has function with many arguments and local variables. Now: no global vars, because I has exams(20.05.2024) and its bad practice in programming.
6. Language has checks for most common problems like dividing by zero, overflowing signed number(not all cases, but some common like multiplication) or square of negative number. This errors will stop program with syscall.

*Standard functions*:

- `input()`             - function reads one pseudo double number(0.0, -12.3, 100.2133) from stdin. Use only in assignment. Remember about accuracy, in case of .00: 13.2134 &#8594; 13.21. Return: rax - one pseudo double number
- `print(var x)`        - function writes one pseudo double number(0.0, -12.3, 100.2133) to stdout. Return: rax - 0 - code of ending function. Return: rax = 0
- `puts(..., 0)`        - function prints string to stdout. Args: ascii decimal codes of letters, last arg must be 0 - terminated symbol. Return: rax = 0. **WARNING**: if you forget 0 at the end it will be UB!!!. 
  
> Advice: you can use `man ascii` in terminal for looking to ascii codes of letters.

*Math functions*:

- `sqrt(var x)`         - function calculates root from not negative number. sqrt(2) = 1.41, sqrt(-1) = Error.It uses FPU. Return: rax - one pseudo double number. Accuracy depends on settings. **WARNING**: if number is negative, it will be error with stopping program. 
- `pow(var x, var y)`   - function raises x to y. pow(-3, 3) = -27, pow(2, 3) = 8, pow(2, -1) = 2. If y < 0 it returns x, if y not integer, it convert it to integer. Return: rax - one pseudo double number.

> Advise: don't use unassigned variables, language doesn't like them. It will be surprise for you

## Standard

Generated tree has a standard, with which you can translate Tree without FrontEnd on your asm(with your BackEnd)!

*Standard of generated tree:*

[```Standard of Tree```](https://github.com/khmelnitskiianton/MyLanguage/blob/main/Standard.md)

## FrontEnd

FrontEnd takes text code on my language and translates it to binary tree with agreement with my standard. Output of FrontEnd is text file `.tree` that contains printed pre-order tree and list of variables.

*Stages:*

1. **Tokenization**:        Move to buffer and use tokenizer that processes words and creates array of objects that contains info about word(type like operation,number or variable and value). With this array of processed objects I can simpler work at the next stages.
2. **Recursive Descent**:   I use parser to make from array of objects binary tree, it based on recursive descent, where I use huge recursion and analyzing expressions. It saves names of variables and numbers in nodes.
3. **Printing**:            After creating binary tree I print it to output file, information in file is enough to restore tree.

## MiddleEnd

MiddleEnd takes tree and optimizes it. It is intermediary program, it helps to avoid complication in final NASM code and increase speed. Output: new file with tree `.tree` with changes.

1. **Scanning Tree**:   I take file `.tree` with standard, scan it and recover binary tree of code.
2. **Optimization**:    After creating tree, I traversal tree and find for redundant branches, then destroy them. 
Now optimizations: convolution of constants, neutral arithmetic operands and convolution of condition `if(0/num) { } else { }` and loops `while(0)`.

3. **Printing**:        After creating binary tree I print it to output file, information in file is enough to restore tree.

## BackEnd

BackEnd takes tree and translates it to NASM file, that next you need to link. It add many standard functions from my asm library. Output: file `.s` with NASM code and comments.

1. **Scanning Tree**:   I take file `.tree` with standard, scan it and recover binary tree of code.
2. **Translation**:     After creating tree, I traversal tree and translate each node to equivalent on asm with adding comments by printing it to `.s` file. 

*Details*:

NASM code work on one system stack.Functions has stack frames, before writing I count all unique local variables in it and initialize them. Calls support CDECL, but functions pretend to have all args in stack. Variables I save in global buffers, not ideal solution(doesn't have overengineering), but I always can rewrite it simpler. I use stack to support nested loops and if constructions to save counters of labels. When I translate expressions, I use system stack to push and pop results, for intermediate operations I use temporary registers `rax`, `rbx`, `rdx`, `rcx`. After expressions result will be in stack - one value. Assignment moves it to local var. I extern standard function. I have some checks for errors, but not all.

## Logs

When programs work, they create folder `build` where will be all logs of translation

Logs use GraphViz to visualize graphs in pictures

`log.html` - full generated log of changes while working with tree.

*Example:*

<img src="https://github.com/khmelnitskiianton/MyLanguage/assets/142332024/39a89151-e734-4672-aeb3-3e96590c7d8f" width=100% >
