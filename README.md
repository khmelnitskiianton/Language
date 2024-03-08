# MyLanguage

Project of translation system - from my language to my assembler with processor.

It consists of 
- Translator: FrontEnd, MiddleEnd, BackEnd. It translate code on my language to code on my assembler (from project `Processor`) 
    + FrontEnd: parses code to tree based recursive descent
    + MiddleEnd: optimizes tree
    + BackEnd: converts tree to code on my assembler
- My Processor ([```Github Processor```](https://github.com/khmelnitskiianton/Processor)) that work similar to Assembler, but on C and simpler.

## Table of Contents

- [MyLanguage](#mylanguage)
  - [Table of Contents](#table-of-contents)
  - [Installation](#installation)
  - [Extra programs](#extra-programs)
  - [Language](#language)
  - [Standard](#standard)
  - [Logs](#logs)

## Installation

To start program you need to use CMake and run program

1.  Clone repository
2.  Compile CMake, write command in main directory in repo
3.  Run CMake (compile program), write command in main directory in repo
4.  Run program(before you need to create this file)

```bash
git clone https://github.com/khmelnitskiianton/MyLanguage.git
cd ./MyLanguage
cmake .
make
./translator.sh {path to file begin with this repo}/{file with code}
```
*Example:* `./translator.sh examples/test_code.sus`

It will be create folder `.tmp`, there will be logs, and intermediate files

## Extra programs

Compiler - gcc. Uses - Cmake. Logs using GraphViz to visualize binary tree!

```bash
sudo apt update && sudo apt upgrade     #update
sudo apt install build-essential        #gcc
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
    var y = (2+0)^1 - 4;
    var a = summ(x, y);
    if (a) {
        print(x);
    }
    while (y != 5){
        y++;
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

## Logs

When programs work, they create folder `.tmp` where will be all logs of translation

Logs use GraphViz to visualize graphs in pictures

`log.html` - full generated log of changes while working with tree.

*Example:*

<img src="https://github.com/khmelnitskiianton/MyLanguage/assets/142332024/39a89151-e734-4672-aeb3-3e96590c7d8f" width=100% >