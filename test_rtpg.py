# Do NOT change this file.

import os

TEST_DIR = "auto-tests-phase3/"
CMD_DIR = "./auto-tests-phase3/cmds/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"

OUTPUT_DIR = f"{TEST_DIR}outputs/"
RTPG_CMD_TESTS_DIR  = f"{CMD_DIR}rtpg/" 
RTPG_OUTPUT_DIR  = f"{OUTPUT_DIR}rtpg/" 

TP_COUNT = 10

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def check_rtpg(answer_fname):
    answer_result = open(answer_fname,'r').readlines()
    written_nodes = answer_result[0].split(',')
    #TODO: how to retrieve input nodes from ckt files.
    circuit_nodes = written_nodes.copy()

    written_nodes.sort()
    circuit_nodes.sort()
    
    if written_nodes!=circuit_nodes:
        return False

    for line in answer_result[1:]:
        tp = line.split(',')

        if len(tp) != len(circuit_nodes):
            return False
        
        if '_t_' in answer_fname:
            for t in tp:
                if t.strip() not in ['0','1','x']:
                    return False
        
        elif '_b_' in answer_fname:
            for t in tp:
                if t.strip() not in ['0','1']:
                    return False
    
    return True

def generate_tests():
    """
    each cmd file:
        READ ...
                
        RTPG b
        RTPG t

        QUIT
    """

    cmd_tests_files = []

    for ckt in os.listdir(CIRCUIT_DIR):
        ckt =  ckt.replace('.ckt','')
        fout = f'{RTPG_CMD_TESTS_DIR}{ckt}_rtpg.txt'
        
        with open(fout, 'w') as ftest:
            ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')
            # format: rtpg 10 b output.txt
            ftest.write(f'RTPG {TP_COUNT} b ../{RTPG_OUTPUT_DIR}{ckt}_b_{TP_COUNT}.txt\n')
            ftest.write(f'RTPG {TP_COUNT} t ../{RTPG_OUTPUT_DIR}{ckt}_t_{TP_COUNT}.txt\n')

            ftest.write('QUIT')
        cmd_tests_files.append(f'{ckt}_rtpg.txt')
        
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
            ckt = cmd_test.replace('.txt','').replace('_rtpg','')
            print('Testing RTPG on ' + cmd_test.replace('.txt', ''))
            os.system(f"cd build/; ./simulator < ../{RTPG_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            student_answer_b = f'{RTPG_OUTPUT_DIR}{ckt}_b_{TP_COUNT}.txt'
            student_answer_t = f'{RTPG_OUTPUT_DIR}{ckt}_t_{TP_COUNT}.txt'

            if check_rtpg(student_answer_b) and check_rtpg(student_answer_t): # if one of the modes b or t is failed, the test fails.
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