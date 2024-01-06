[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/qRoFnGHU)
# Phase-3: ATPG

## Description

Available on *blackboard*.
In addition to new functions, your previous functions also must work properly.

## Build & Run
```
mkdir build && cd build
cmake ..
make

./simulator
```

## Commands Sample
```
HELP
READ ckts/c17.ckt
PC
LEV lev_output.txt

SCOAP output_file.txt
DALG 12 1 output_file.txt
PODEM 9 1 output_file.txt
ATPG DALG output_file.txt report_file.txt
TPG output_file.txt report_file.txt

RTPG 10 b rtpg_output.txt
RFL rfl_output.txt
LOGICSIM tp_input_file.txt logicsim_output.txt
DFS tp_input_file.txt dfs_output.txt
PFS tp_input_file.txt fault_input_file.txt pfs_output.txt
TPFC 200 4 tpfc_output.txt tpfc_report

QUIT
```

## Testing
```
python3 test_main.py --func scoap
python3 test_main.py --func dalg
python3 test_main.py --func podem

python3 test_main.py --func rtpg
python3 test_main.py --func rfl
python3 test_main.py --func pfs
python3 test_main.py --func dfs
python3 test_main.py --func logicsim

python3 test_main.py --func all
```

## Checking Memory Leak Using Valgrind
As you allocate memory dynamically and do not deallocate it correctly, it results in memory leak. This means when the program is finished, there is still some allocated memory in the ram while it is inaccessible and you cannot free it. To detect this, here's a tool you can use.


First, install [Valgrind](https://valgrind.org/). On Ubuntu it is install using this script:
```
sudo apt-get install valgrind
```

To use it, when running program, use this command:
```
valgrind --leak-check=yes ./simulator
```

Here is a quick guide to how to read leak logs:

* if you see something like below in the log, it means that no memory leak has occured:

    ```
    ==96617== HEAP SUMMARY:
    ==96617==     in use at exit: 0 bytes in 0 blocks
    ==96617==   total heap usage: 552 allocs, 552 frees, 123,266 bytes allocated
    ==96617== 
    ==96617== All heap blocks were freed -- no leaks are possible
    ```
* Otherwise, you will see a summary like this at the end of the log:
    ```
    ==92649== HEAP SUMMARY:
    ==92649==     in use at exit: 2,820 bytes in 70 blocks
    ==92649==   total heap usage: 181 allocs, 111 frees, 108,768 bytes allocated
    ==92649== 
    ==92649== 92 bytes in 1 blocks are definitely lost in loss record 4 of 8
    ==92649==    at 0x4841888: malloc (vg_replace_malloc.c:393)
    ==92649==    by 0x12F042: cread() (readckt.cpp:350)
    ==92649==    by 0x136501: main (main.cpp:21)
    ==92649== 
    ==92649== LEAK SUMMARY:
    ==92649==    definitely lost: 92 bytes in 1 blocks
    ==92649==    indirectly lost: 0 bytes in 0 blocks
    ==92649==      possibly lost: 0 bytes in 0 blocks
    ==92649==    still reachable: 2,728 bytes in 69 blocks
    ==92649==         suppressed: 0 bytes in 0 blocks
    ```
**SUMMARY**: 


`definitely lost` means: The allocated memory has no longer a pointer.

`indirectly lost` means: The pointer to the place that the variable's pointer was saved, is lost.

`possible lost` means: The memory is not freed and Valgrind cannot be sure whether there is a pointer or not.

`still reachable` means: Possibly a global variable is not freed but still has a pointer.

`suppressed lost` means: You can safely ignore this one.

<br>

[Read more here.](https://web.stanford.edu/class/archive/cs/cs107/cs107.1174/guide_valgrind.html#:~:text=That%20block%20was%20allocated%20by,since%20lost%20track%20of%20it.)

### New Mini-Task!
As you already knew that memory leak is such a disadvantage to your code, utilize Valgrind to reduce memory leak as much as you can.
