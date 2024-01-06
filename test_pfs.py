# Do NOT change this file.

import os
import subprocess
import time

TIME_LIMIT=5

TEST_DIR = "auto-tests-phase3/"
TPI_DIR = f"{TEST_DIR}tpi/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
CMD_DIR = "./auto-tests-phase3/cmds/"

OUTPUT_DIR = f"{TEST_DIR}outputs/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"

PFS_INPUT_TP_DIR = f"{TEST_DIR}inputs/pfs/tps/"
PFS_INPUT_F_DIR = f"{TEST_DIR}inputs/pfs/faults/"

PFS_CMD_TESTS_DIR  = f"{CMD_DIR}pfs/" 
PFS_OUTPUT_DIR = f"{OUTPUT_DIR}pfs/"
PFS_GOLDEN_DIR = f"{GOLDEN_DIR}pfs/"

FAULT_COUNT = 10
TP_COUNTS = [1,5,10]

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
        PFS ... _b_
        QUIT
    """

    cmd_tests_files = []
    for count in TP_COUNTS:
        for ckt in os.listdir(CIRCUIT_DIR):
            ckt =  ckt.replace('.ckt','')
            fout = f'{PFS_CMD_TESTS_DIR}{ckt}_{count}_pfs.txt'
            with open(fout, 'w') as ftest:
                ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
                # test pattern file format: pfs tps faults output
                ftest.write(f'PFS ../{PFS_INPUT_TP_DIR}{ckt}_b_{count}.txt ../{PFS_INPUT_F_DIR}{ckt}_b_f{FAULT_COUNT}_tp{count}.txt ../{PFS_OUTPUT_DIR}{ckt}_b_f{FAULT_COUNT}_tp{count}.txt\n')
                ftest.write('QUIT')
            cmd_tests_files.append(f'{ckt}_{count}_pfs.txt')

    return cmd_tests_files

if __name__ == '__main__':
    cmd_test_files = generate_tests()
    pass_all = True
    failed_count = 0
    passed_count = 0

    for cmd_test in cmd_test_files:
        ckt = cmd_test.split('_')[0]
        tp_count = cmd_test.replace('_pfs.txt','').replace(ckt+'_','')
        try:
            gold_fail = False
            print('_'*50)
            print('Testing PFS on ' + cmd_test.replace('_pfs.txt', ''))
            # process = subprocess.Popen(f"cd build/; ./simulator < ../{PFS_CMD_TESTS_DIR}{cmd_test} > /dev/null ", shell=True)
            os.system(f"cd build/; ./simulator < ../{PFS_CMD_TESTS_DIR}{cmd_test} > /dev/null")
            # current_time=time.time()
            # while time.time() < current_time + TIME_LIMIT and process.poll() is None:
                # pass
            # if process.poll() is None:
                # process.kill()
                # print(f"test '{ckt.replace('.txt','')}'","exceeded time limit")
                # raise Exception('Time limit exceeded (probably stuck in loop).')

            golden_result_b = f"{PFS_GOLDEN_DIR}{ckt}_b_f{FAULT_COUNT}_tp{tp_count}.txt"
    
            if (not os.path.exists(golden_result_b)):
                print(f'\n{bcolors.WARNING}Golden result does not exists.{bcolors.ENDC}')
                gold_fail = True
    
            student_answer_b = f"{PFS_OUTPUT_DIR}{ckt}_b_f{FAULT_COUNT}_tp{tp_count}.txt"
    
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