# Arctic Fox Lang

A forth inspired lang :3
![image](https://github.com/LazyBev/arctic-fox-lang/assets/157259616/f273a926-66f9-41ad-b7a3-a27c021bb132)

# Compiling and running an afl project
e.g.

    

# How to Code in AFL

Basic Operations

    (int): Pushes number to stack

    prn: Pops and prints top of stack

Arithmetic Operations

    +: Adds top two numbers on the stack

    -: Subtracts top two numbers on the stack

    *: Multiplies top two numbers on the stack

    /: Divides top two numbers on the stack

    %: Divides top two numbers on the stack but returns remainder instead

Comparison Operations

    =: Compares top two numbers on stack and checks if they are equal

    !=: Compares top two numbers on stack and checks if they are not equal

    >: Compares top two numbers on stack and checks if first value is greater than second value

    >=: Compares top two numbers on stack and checks if first value is greater than or equal to second value

    !>: Compares top two numbers on stack and checks if first value is not greater than second value

    <: Compares top two numbers on stack and checks if first value is less than second value

    <=: Compares top two numbers on stack and checks if first value is less than or equal to second value

    !<: Compares top two numbers on stack and checks if first value is not less than second value

Stack Operations

    dup: Duplicates top of stack

    2dup: Duplicates top two numbers on stack

    swap: Swaps top two numbers on the stack

    over: Pushes a copy of 2nd to top number to top

    drop: Pops top of the stack

Logical Operations

    or: Performs logical OR on the top 2 numbers on stack

    xor: Performs logical XOR on the top 2 numbers on stack
    
    and: Performs logical AND on the top 2 numbers on stack

Memory Operations

    mem: Memory-related operation

    .: Stores a value to memory

    ,: Loads a value from memory

System Calls

    syscallN (N = 1 to 6): Performs a Linux syscall

Extra:
    //: Comment line

Examples: 

    60 prn // prints 60

    20 10 - prn // prints 10

    20 10 + prn // 30

    20 10 * prn -> 200

    20 10 / prn -> 2

    20 2 % prn -> 0

    2 2 = prn -> 1

    2 2 != prn -> 0

    1 2 > prn -> 0

    2 2 >= prn -> 1

    1 2 !> prn -> 1

    1 2 < prn -> 1

    3 2 <= prn -> 0

    1 2 !< prn -> 0

    2 dup + prn -> 4

    2 4 2dup + prn -> 6

    1 2 or prn -> 3

    1 3 xor prn -> 2

    3 3 and prn -> 3

    1 2 swap prn -> 1

    4 5 over - prn -> 1

    5 4 pop prn -> 5

    mem 0 + 97 . -> (pushes ASCII) 'a'

    1 mem 1 1 syscall3 -> (prints) 'a'


---------------------------------------------------------------------------------------------------------------------------

If-Else Statements:

    if: Checks to see if condition is true

    else: If conditon is false

    end: Ends an if statement

Example:

    20 20 + 40 = if

        20 20 + 41 = if
    
           50 prn
        
        else
    
           70 prn
        
        end
    
    else

        60 prn
    
    end

Result -> `70`

---------------------------------------------------------------------------------------------------------------------------

While Loops

    while: Starts a while loop

    do: Executes loop body

    done: Ends a while loop

Example:

    1 
    while dup 4 > do
    
        dup prn
      
        1 +
      
    done

Result -> `1 2 3`

---------------------------------------------------------------------------------------------------------------------------

Macros

    macro: Starts a macro definition

    close: Closes the macro definition

Example:

    macro lol
        60 60 + prn
    close

    lol

Result -> `120`
