# Do NOT change this file.

import os

TEST_DIR = "auto-tests-phase3/"
TPI_DIR = f"{TEST_DIR}tpi/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
CMD_DIR = "./auto-tests-phase3/cmds/"

OUTPUT_DIR = f"{TEST_DIR}outputs/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"

RFL_CMD_TESTS_DIR  = f"{CMD_DIR}rfl/" 
RFL_OUTPUT_DIR = f"{OUTPUT_DIR}rfl/"
RFL_GOLDEN_DIR = f"{GOLDEN_DIR}rfl/"

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
        RFL ...
        QUIT
    """

    cmd_tests_files = []
    for ckt in os.listdir(CIRCUIT_DIR):
        ckt =  ckt.replace('.ckt','')
        fout = f'{RFL_CMD_TESTS_DIR}{ckt}_rfl.txt'
        with open(fout, 'w') as ftest:
            ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
            # test pattern file format: rfl output
            ftest.write(f'RFL ../{RFL_OUTPUT_DIR}{ckt}_rfl.txt\n')
            ftest.write('QUIT')
        cmd_tests_files.append(f'{ckt}_rfl.txt')

        
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
            print('Testing RFL on ' + cmd_test.replace('.txt', ''))
            os.system(f"cd build/; ./simulator < ../{RFL_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            golden_result = f"{RFL_GOLDEN_DIR}{cmd_test}"
    
            if (not os.path.exists(golden_result)):
                print(f'\n{bcolors.WARNING}Golden result does not exists.{bcolors.ENDC}')
                gold_fail = True
    
            student_answer_b = f"{RFL_OUTPUT_DIR}{cmd_test}"
    
            # if any of b or t is wrong, the whole test is failed.
            if correct_answer(student_answer_b, golden_result):
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