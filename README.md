# SCC
Small Compiler Collection(SCC) - my translation system

It consists of 
- Translator: FrontEnd, MiddleEnd, BackEnd. It translate code on my language to code on my assembler (from project [Github Processor](https://github.com/khmelnitskiianton/Processor "my processor")) 
    + FrontEnd: parses code to tree based recursive descent
    + MiddleEnd: optimizes tree (based on project [Github Differentiator](https://github.com/khmelnitskiianton/Differentiator "my differentiator"))
    + BackEnd: converts tree to code on my assembler

## Table of Contents

- [Installation](#installation)
- [Extra programs](#extra-programs)
- [Language](#language)
- [Options of compiling](#options-of-compiling)
- [Myassert](#myassert)
- [Verificator](#verificator)
- [Logs](#logs)

## Installation

To start program you need to use CMake and run program

1.  Clone repository
2.  Compile CMake, write command in main directory in repo
3.  Run CMake (compile program), write command in main directory in repo
4.  Run program(before you need to create this file) 
```c
git clone https://github.com/khmelnitskiianton/SCC.git
cd ./SCC
cmake .
make
./scc.bat <path to file with code>/<file with code>
```

## Extra programs

Logs using Graphviz to vizualize binary tree!(you can turn them off) 

```c
sudo apt install graphviz
```

## Language

[Grammar of my language](https://github.com/khmelnitskiianton/SCC/blob/main/grammar.pdf)

```java
- Initilizing                       var x;
- Assignment                        var y = (1+3^2)*a - sum(b,c)/4;
- Increment_decrement_operators     i++;
- Condition                         if (a && (b || c)) { ... } else { ... }
- Loop                              while ((x >= y) == 1) {... break; ... continue;}
- Define_function                   void foo(var p, var m) { ... return;}
- Call_function                     hello();
```

```java
Example of code:

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

## Options of compiling

DEBUG_LOG 

DEBUG_TEX_LOG

DEBUG_MYASSERT 

DEBUG_VERIFY 

## Myassert

`myassert.h`

Now it relized like MYASSERT() that take all errors with enum and write it to console

## Verificator

`verificator.cpp` `verificator.h` 

Checking tree for loops and other errors. Functions returns EnumOfErrors 

## Logs

`log.cpp` `log.h`

Logs use GraphViz to vizualized graphs in pictures

After generating image it includes to html file `log.html`

[Example of log](https://github.com/khmelnitskiianton/SCC/blob/main/example_log.svg)