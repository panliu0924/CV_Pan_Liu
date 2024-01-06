# Do NOT change this file.

import os

TEST_DIR = "auto-tests-phase3/"
TPI_DIR = f"{TEST_DIR}tpi/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
OUTPUT_DIR = f"{TEST_DIR}outputs/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"
CMD_DIR = "./auto-tests-phase3/cmds/"


LOGICSIM_INPUT_DIR = f"{TEST_DIR}inputs/logicsim/"
LOGICSIM_CMD_TESTS_DIR  = f"{CMD_DIR}logicsim/" 
LOGICSIM_OUTPUT_DIR = f"{OUTPUT_DIR}logicsim/"
LOGICSIM_GOLDEN_DIR = f"{GOLDEN_DIR}logicsim/"

TP_COUNT = 10

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def correct_answer(answer, golden):
    passed=True
    golden_result = open(golden,'r').readlines()
    answer_result = open(answer,'r').readlines()

    for i in range(len(golden_result)):
        if golden_result[i].strip()!=answer_result[i].strip():
            passed = False
            print(f'{bcolors.FAIL}line {i} not equal:{bcolors.ENDC} golden={golden_result[i]}, answered={answer_result[i]}'.replace('\n',''))
  
    return passed

def generate_tests():
    """
    each cmd file:
        READ ...
        LOGICSIM ... _t_
        LOGICSIM ... _b_
        QUIT
    """

    cmd_tests_files = []
    for ckt in os.listdir(CIRCUIT_DIR):
        ckt =  ckt.replace('.ckt','')
        fout = f'{LOGICSIM_CMD_TESTS_DIR}{ckt}_logicsim.txt'
        with open(fout, 'w') as ftest:
            ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
            # test pattern file format: logicsim input output
            ftest.write(f'LOGICSIM ../{LOGICSIM_INPUT_DIR}{ckt}_t_{TP_COUNT}.txt ../{LOGICSIM_OUTPUT_DIR}{ckt}_t_{TP_COUNT}.txt\n')
            ftest.write(f'LOGICSIM ../{LOGICSIM_INPUT_DIR}{ckt}_b_{TP_COUNT}.txt ../{LOGICSIM_OUTPUT_DIR}{ckt}_b_{TP_COUNT}.txt\n')
            ftest.write('QUIT')
        cmd_tests_files.append(f'{ckt}_logicsim.txt')

        
    return cmd_tests_files

if __name__ == '__main__':
    cmd_test_files = generate_tests()
    pass_all = True
    failed_count = 0
    passed_count = 0

    for cmd_test in cmd_test_files:
        try:
            gold_fail = False
            print('_'*50)
            print('Testing logicsim on ' + cmd_test.replace('_logicsim.txt', ''))
            os.system(f"cd build/; ./simulator < ../{LOGICSIM_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            golden_result_b = f"{LOGICSIM_GOLDEN_DIR}golden_b_{cmd_test}"
            golden_result_t = f"{LOGICSIM_GOLDEN_DIR}golden_t_{cmd_test}"
    
            if (not os.path.exists(golden_result_t)) or (not os.path.exists(golden_result_b)):
                print(f'\n{bcolors.WARNING}Golden result does not exists.{bcolors.ENDC}')
                gold_fail = True
    
            ckt = cmd_test.replace('_logicsim.txt','')
            student_answer_b = f"{LOGICSIM_OUTPUT_DIR}{ckt}_b_{TP_COUNT}.txt"
            student_answer_t = f"{LOGICSIM_OUTPUT_DIR}{ckt}_t_{TP_COUNT}.txt"
    
            # if any of b or t is wrong, the whole test is failed.
            if correct_answer(student_answer_b, golden_result_b) and correct_answer(student_answer_t, golden_result_t):
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