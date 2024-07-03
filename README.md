# Arctic Fox Lang
A forth inspired lang :3
![image](https://github.com/LazyBev/arctic-fox-lang/assets/157259616/f273a926-66f9-41ad-b7a3-a27c021bb132)
# Interpreting an afl project
e.g.

  gcc -o afl afl.c
  
  ./afl test.afl
# Compiling and running an afl project
e.g.

  make compile
  make run

# How to code in afl
```(int)```: Pushes number to stack

```prn```: Pops and prints top of stack

```+```: Adds top two numbers on the stack

```-```: Subtracts top two numbers on the stack

```*```: Multiplies top two numbers on the stack

```/```: Divides top two numbers on the stack

```%```: Divides top two numbers on the stack but returns remainer instead

```=```: Compares top two numbers on stack and checks if they are equal

```!=```: Compares top two numbers on stack and checks if they are not equal

```>```: Compares top two numbers on stack and checks if first val is greater than second val

```>=```: Compares top two numbers on stack and checks if first val is greater than or equal to second val

```!>```: Compares top two numbers on stack and checks if first val is not greater than second val

```<```: Compares top two numbers on stack and checks if first val is less than second val

```<=```: Compares top two numbers on stack and checks if first val is less than or equal to second val

```!<```: Compares top two numbers on stack and checks if first val is not less than second val

```dup```: Duplicates top of stack

```2dup```: Duplicates top two number on stack

```or```: Performs logical and on the top 2 numebrs on stack

```xor```: Performs logical xor on the top 2 numebrs on stack

```and```: Performs logical or on the top 2 numebrs on stack

```swap```: Swaps top two numbers on the stack

```over```: Pushes a copy of 2nd to top number to top

```pop```: Pops top of the stack

``````mem`````````:``` ```Memory-r```elated operation

```.```: Stores a value to memory

```,```: Loads a value to memory

```syscallN (N = 1 to 6)```: Performs a linux syscall

Examples: 

```60``` ```prn``` -> ```60```

```20``` ```10``` ```-``` ```prn``` -> ```10```

```20``` ```10``` ```+``` ```prn``` -> ```30```

```20``` ```10``` ```*``` ```prn``` -> ```200```

```20``` ```10``` ```/``` ```prn``` -> ```2```

```20``` ```2``` ```%``` ```prn``` -> ```0```

```2``` ```2``` ```=``` ```prn``` -> ```1```

```2``` ```2``` ```!``` ```=``` ```prn``` -> ```0```

```1``` ```2``` ```>``` ```prn``` -> ```0```

```2``` ```2``` ```>=``` ```prn``` -> ```1```

```1``` ```2``` ```!>``` ```prn``` -> ```1```

```1``` ```2``` ```<``` ```prn``` -> ```1```

```3``` ```2``` ```<=``` ```prn``` -> ```0```

```1``` ```2``` ```!<``` ```prn``` -> ```0```

```2``` ```dup``` ```+``` ```prn``` -> ```4```

```2``` ```4``` ```2dup``` ```+``` ```prn``` -> ```6```

```1``` ```2``` ```or``` ```prn``` -> ```3```

```1``` ```3``` ```xor``` ```prn``` -> ```2```

```3``` ```3``` ```and``` ```prn``` -> ```3```

```1``` ```2``` ```swap``` ```prn``` -> ```1```

```4``` ```5``` ```over``` ```-``` ```prn``` -> ```1```

```5``` ```4``` ```pop``` ```prn``` -> ```5```

```mem``` ```0``` ```+``` ```97``` ```.``` -> (pushes ASCII) 'a'

```1``` ```mem``` ```1``` ```1``` ```syscall3``` -> (prints) 'a'

---------------------------------------------------------------------------------------------------------------------------

```(condition) if (code to exec)```: Checks to see if comparison is true and executes some code

```else (code to exec)```: If conditon is false executes some code

```end```:

Example:

    20 20 + 40 = if

        20 20 + 41 = if
    
           50 ```prn```
        
        else
    
           70 ```prn```
        
        end
    
    else

        60 ```prn```
    
    end

```Result``` -> ```70```

---------------------------------------------------------------------------------------------------------------------------

```while (condition)```: Checks the conditon

```do (code to loop)```: Executes looped code

```done```: Ends while statement (done)

Example:

    1 
    while dup 4 > do
    
        dup ```prn```
      
        1 +
      
    done

```Result``` -> ```1 2 3```
