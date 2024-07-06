# Tau Lang

A forth inspired lang :3
![image](https://github.com/LazyBev/tau-lang/assets/157259616/97128299-c355-4448-bce9-b16ddeca7b89)

# Compiling and running an tau project

python (compiler) (flag) (source).tau (out)

# How to Code in tau

Standard lib:

    write: Prints a string

    read: Reads a string

    true: Boolean true

    false: Boolean false
    
    2dup: Duplicates top two numbers on stack
    
    2drop: Drops top two numbers on stack
    
    stdin: Return code for standard input
    
    stdout: Return code for standard output
    
    stderr: Return code for standard error
    
    sys_read:  Syscall number for reading
    
    sys_write:  Syscall number for writing
    
    sys_open:  Syscall number for opening
    
    sys_close:  Syscall number for closing
    
    sys_exit:  Syscall number for exit

    exit: Exits with code 0

Misc:

    //: Comment line

    ;: Includes a file

Basic Operations

    (int): Pushes number to stack

    prn: Pops and prints top of stack

    Example:

        ;core/std.afl
        
        60 prn // prints 60

        exit

Strings:

    "": Creates a string

    Example:

        ;core/std.afl

        "Hello" write // prints Hello

        exit

Arithmetic Operations

    +: Adds top two numbers on the stack

    -: Subtracts top two numbers on the stack

    *: Multiplies top two numbers on the stack

    /: Divides top two numbers on the stack

    %: Divides top two numbers on the stack but returns remainder instead

    Example:

        ;core/std.afl
    
        20 10 - prn // prints 10

        20 10 + prn // prints 30

        20 10 * prn // prints 200

        20 10 / prn // prints 2

        20 2 % prn // prints 0

        exit



Comparison Operations

    =: Compares top two numbers on stack and checks if they are equal

    !=: Compares top two numbers on stack and checks if they are not equal

    >: Compares top two numbers on stack and checks if first value is greater than second value

    >=: Compares top two numbers on stack and checks if first value is greater than or equal to second value

    !>: Compares top two numbers on stack and checks if first value is not greater than second value

    <: Compares top two numbers on stack and checks if first value is less than second value

    <=: Compares top two numbers on stack and checks if first value is less than or equal to second value

    !<: Compares top two numbers on stack and checks if first value is not less than second value

    Example:

        ;core/std.afl

        2 2 = prn // prints 1
    
        2 2 != prn // prints 0
        
        1 2 > prn // prints 0
        
        2 2 >= prn // prints 1
        
        1 2 !> prn // prints 1
        
        1 2 < prn // prints 1
        
        3 2 <= prn // prints 0
        
        1 2 !< prn // prints 0

        exit

Stack Operations

    dup: Duplicates top of stack

    swap: Swaps top two numbers on the stack

    drop: Pops top of the stack

    over: Pushes a copy of 2nd to top number to top

    Example:

        ;core/std.afl

        2 dup + prn // prints 4
    
        2 4 2dup + prn // prints 6

        1 2 swap prn // prints 1
    
        5 4 drop prn // prints 5

        4 5 over - drop prn // prints 1

        exit

Logical Operations

    or: Performs logical OR on the top 2 numbers on stack

    xor: Performs logical XOR on the top 2 numbers on stack
    
    and: Performs logical AND on the top 2 numbers on stack

    Example:

        ;core/std.afl

        1 2 or prn // prints 3
    
        1 3 xor prn // prints 2
        
        3 3 and prn // prints 3

        exit

Memory Operations

    bit: Creates a bit in memeory

    store: Stores a value to memory

    load: Loads a value from memory

    Example:

        ;core/std.afl

        bit 0 + 97 store // (pushes ASCII) 'a'
    
        1 mem 1 1 syscall3 // (prints) 'a'

        exit

System Calls

    syscallN (N = 1 to 6): Performs a Linux syscall

    Example:

        0 60 syscall1 // exit with 0


---------------------------------------------------------------------------------------------------------------------------

If-Else Statements:

    if: Checks to see if condition is true

    else: If conditon is false

    end: Ends an if statement

    Example:

        ;core/std.afl

        20 20 + 40 = if

            20 20 + 41 = if
        
            50 prn
            
            else
        
            70 prn
            
            end
        
        else

            60 prn
        
        end

        exit

Result -> `70`

---------------------------------------------------------------------------------------------------------------------------

While Loops

    while: Starts a while loop

    do: Executes loop body

    done: Ends a while loop

    Example:

        ;core/std.afl

        1 while dup 4 > do
        
            dup prn
        
            1 +
        
        done

        exit

Result -> `1 2 3`

---------------------------------------------------------------------------------------------------------------------------

Macros

    macro: Starts a macro definition

    close: Closes the macro definition

    Example:

        ;core/std.afl

        macro lol
            60 60 + prn
        close

        lol

        exit

Result -> `120`

---------------------------------------------------------------------------------------------------------------------------

Includes

    ;: Includes a file

    Example:
        
        In lol.afl:
            
            macro hi 
            
                "hi" 1 1 syscall3 
                
                drop 
                
            close

        In main.afl:

            ;core/std.afl

            ;lol.afl

            hi

            exit

Result -> `hi`

