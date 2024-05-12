# MyLanguage

Project of translation system - from my language to NASM

It consists of FrontEnd, MiddleEnd, BackEnd. It translate code on my language to code on NASM:
+ FrontEnd: parses code to tree based recursive descent
+ MiddleEnd: optimizes tree
+ BackEnd: converts tree to code on my assembler

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
git clone https://github.com/khmelnitskiianton/MyLanguage.git
cd ./MyLanguage
cmake .
make
chmod ugo+x translator.sh
chmod ugo+x linker.sh
./translator.sh {path to file begin with this repo}/{file with source code}
./linker.sh {path to file begin with this repo}/{file with NASM code}
```
*Example:* `./translator.sh examples/test_code.sus`

It will be create folder `.tmp`, there will be logs, and intermediate files

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
    var y = (2+x) - 4;
    var a = summ(x, y);
    if (a) {
        print(x);
    }
    while (y != 5){
        print(y);
    }
}

void summ(var a, var b){
    return a+b;
}
``` 

## Standard

Generated tree has a standard, with which you can translate Tree without FrontEnd on your asm(with your BackEnd)!

*Standard of generated tree:*

[```Standard of Tree```](https://github.com/khmelnitskiianton/MyLanguage/blob/main/Standard.md)

## FrontEnd

Based on recursive descent create a file with tree and table of variables with `.tree` extension

## MiddleEnd

Optimizes tree

Now optimizations: convolution of constants, neutral arithmetic operands and convolution of condition `if(0/num){}else{}` and loops `while(0)`

## Logs

When programs work, they create folder `.tmp` where will be all logs of translation

Logs use GraphViz to visualize graphs in pictures

`log.html` - full generated log of changes while working with tree.

*Example:*

<img src="https://github.com/khmelnitskiianton/MyLanguage/assets/142332024/39a89151-e734-4672-aeb3-3e96590c7d8f" width=100% >