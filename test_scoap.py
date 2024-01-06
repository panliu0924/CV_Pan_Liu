# Do NOT change this file.

import os

TEST_DIR = "auto-tests-phase3/"
CMD_DIR = "./auto-tests-phase3/cmds/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"

OUTPUT_DIR = f"{TEST_DIR}outputs/"
SCOAP_CMD_TESTS_DIR  = f"{CMD_DIR}scoap/" 
SCOAP_OUTPUT_DIR  = f"{OUTPUT_DIR}scoap/" 
SCOAP_GOLDEN_DIR  = f"{GOLDEN_DIR}scoap/" 

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def check_scoap(answer, golden):
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
                
        SCOAP output

        QUIT
    """

    cmd_tests_files = []

    for ckt in os.listdir(CIRCUIT_DIR):
        ckt =  ckt.replace('.ckt','')
        fout = f'{SCOAP_CMD_TESTS_DIR}{ckt}_scoap.txt'
        
        with open(fout, 'w') as ftest:
            ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
            # format: scoap output.txt
            ftest.write(f'SCOAP ../{SCOAP_OUTPUT_DIR}output_{ckt}.txt\n')

            ftest.write('QUIT')
        cmd_tests_files.append(f'{ckt}_scoap.txt')
        
    return cmd_tests_files

if __name__ == '__main__':

    cmd_test_files = generate_tests()
    print(cmd_test_files)

    pass_all = True
    failed_count = 0
    passed_count = 0

    for cmd_test in cmd_test_files:
        try:
            gold_fail = False
            print('_'*50)
            ckt = cmd_test.replace('.txt','').replace('_scoap','')
            print('Testing SCOAP on ' + cmd_test.replace('.txt', ''))
            os.system(f"cd build/; ./simulator < ../{SCOAP_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            student_answer = f'{SCOAP_OUTPUT_DIR}output_{ckt}.txt'
            golden_result = f'{SCOAP_GOLDEN_DIR}scoap_{ckt}.txt'

            if check_scoap(student_answer, golden_result):
                print(f"{bcolors.OKGREEN}\nTest '{cmd_test.replace('.txt','')}'","PASSED",f"{bcolors.ENDC}")
                passed_count += 1
            else:
                print(f"{bcolors.FAIL}\nTest '{cmd_test.replace('.txt','')}'","FAILED",f"{bcolors.ENDC}")
                pass_all = False
                failed_count+=1

        except Exception as e:
            print(e)
            print(f"{bcolors.FAIL}\nError{bcolors.ENDC}")
            
            pass_all = False
            failed_count+=1
            
    print('-'*50)

    print(f'{bcolors.OKGREEN}{passed_count} tests are passed.{bcolors.ENDC}')
    print(f'{bcolors.FAIL}{failed_count} tests are failed.{bcolors.ENDC}')

    if pass_all:
        exit(0)
    else:
        exit(1)