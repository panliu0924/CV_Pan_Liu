# Do NOT change this file.

import os

TEST_DIR = "auto-tests-phase3/"
TPI_DIR = f"{TEST_DIR}tpi/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
OUTPUT_DIR = f"{TEST_DIR}outputs/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"
CMD_DIR = "./auto-tests-phase3/cmds/"


DFS_INPUT_DIR = f"{TEST_DIR}inputs/dfs/"
DFS_CMD_TESTS_DIR  = f"{CMD_DIR}dfs/" 
DFS_OUTPUT_DIR = f"{OUTPUT_DIR}dfs/"
DFS_GOLDEN_DIR = f"{GOLDEN_DIR}dfs/"

TP_COUNT = [1, 5, 10]

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def correct_answer(answer, golden):
    passed=True
    golden_result = open(golden,'r').readlines()
    answer_result = open(answer,'r').readlines()

    for line in answer_result:
        if line and line not in golden_result:
            print(f"{bcolors.FAIL}\n")
            print(f"{bcolors.FAIL}\nWrong line in output:")
            print(f"Error on\n\t{bcolors.FAIL}"+line)
            passed = False

    for line in golden_result:
        if line and line not in answer_result:
            print(f"{bcolors.FAIL}\nMissing line in output:")
            print(f"{bcolors.FAIL}\t"+line)
            passed = False

    return passed

def generate_tests():
    """
    each cmd file:
        READ ...
        DFS ... _b_
        QUIT
    """

    cmd_tests_files = []
    counts = TP_COUNT
    for count in counts:
        for ckt in os.listdir(CIRCUIT_DIR):
            ckt =  ckt.replace('.ckt','')
            fout = f'{DFS_CMD_TESTS_DIR}{ckt}_{count}_dfs.txt'
            with open(fout, 'w') as ftest:
                ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
                # test pattern file format: dfs input output
                ftest.write(f'DFS ../{DFS_INPUT_DIR}{ckt}_b_{count}.txt ../{DFS_OUTPUT_DIR}{ckt}_b_{count}.txt\n')
                ftest.write('QUIT')
            cmd_tests_files.append(f'{ckt}_{count}_dfs.txt')

    return cmd_tests_files

if __name__ == '__main__':
    cmd_test_files = generate_tests()
    pass_all = True
    failed_count = 0
    passed_count = 0

    for cmd_test in cmd_test_files:
        ckt = cmd_test.split('_')[0]
        tp_count = cmd_test.replace('_dfs.txt','').replace(ckt+'_','')
        try:
            gold_fail = False
            print('_'*50)
            print('Testing DFS on ' + cmd_test.replace('_dfs.txt', ''))
            os.system(f"cd build/; ./simulator < ../{DFS_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            golden_result_b = f"{DFS_GOLDEN_DIR}{ckt}_b_{tp_count}.txt"
    
            if (not os.path.exists(golden_result_b)):
                print(f'\n{bcolors.WARNING}Golden result does not exists.{bcolors.ENDC}')
                gold_fail = True
    
            student_answer_b = f"{DFS_OUTPUT_DIR}{ckt}_b_{tp_count}.txt"
    
            if correct_answer(student_answer_b, golden_result_b):
                print(f"{bcolors.OKGREEN}\nTest '{cmd_test.replace('.txt','')}' PASSED{bcolors.ENDC}")
                passed_count += 1
    
            else:
                print(f"{bcolors.FAIL}\nTest '{cmd_test.replace('.txt','')}' FAILED{bcolors.ENDC}")
                pass_all = False
                failed_count+=1

        except Exception as e:
            print(e)
            print(f"{bcolors.FAIL}\nError{bcolors.ENDC}")
            if not gold_fail:
                pass_all = False
                failed_count+=1
            
    print('-'*50)

    print(f'{bcolors.OKGREEN}{passed_count} tests are passed.{bcolors.ENDC}')
    print(f'{bcolors.FAIL}{failed_count} tests are failed.{bcolors.ENDC}')

    if pass_all:
        exit(0)
    else:
        exit(1)