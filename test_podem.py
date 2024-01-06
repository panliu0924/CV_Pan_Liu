# Do NOT change this file.

import os
import pandas as pd

TEST_DIR = "auto-tests-phase3/"
CMD_DIR = "./auto-tests-phase3/cmds/"
CIRCUIT_DIR = f"{TEST_DIR}ckts/"
GOLDEN_DIR = f"{TEST_DIR}golden_results/"

OUTPUT_DIR = f"{TEST_DIR}outputs/"
PODEM_CMD_TESTS_DIR  = f"{CMD_DIR}podem/" 
PODEM_OUTPUT_DIR  = f"{OUTPUT_DIR}podem/" 
PODEM_GOLDEN_DIR  = f"{GOLDEN_DIR}podem/" 

X_VALUE = 'x'

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def gen_all_tps(tp):
    from collections import deque

    all_tps = deque()
    all_tps.append(tp)

    while True:
        front_tp = all_tps.popleft()
        if not X_VALUE in front_tp:
            all_tps.append(front_tp)
            break
        
        first_x = None
        for t in range(len(front_tp)):
            if front_tp[t] == X_VALUE:
                first_x = t
                break
        
        #substitute zero and one
        if first_x is not None:
            tp_copy = front_tp.copy()
            tp_copy[first_x] = 1
            all_tps.append(tp_copy)

            front_tp[first_x] = 0
            all_tps.append(front_tp)
    
    return list(all_tps)

def tp_to_int(tp):
    return int(tp.replace(',','').replace('\n',''), 2)

def check_podem(answer, ckt, fault):
    """Using fd_csv"""
    passed=True
    answer_result = open(answer,'r').readlines()
    if len(answer_result)>1:
        answer_result = answer_result[1].strip()
        stu_tp = list(answer_result.split(','))
        
        all_stu_tps = gen_all_tps(stu_tp)
        golden_df = pd.read_csv(f'./auto-tests-phase3/golden_results/fd_csv/{ckt}_fd.csv', index_col=0)
    else:
        all_stu_tps = []

    for st_tp in all_stu_tps:
        tp_int = tp_to_int(",".join([str(s) for s in st_tp]))
        if not golden_df[fault].loc[tp_int] == 1.0:
            passed=False
            print(f'{bcolors.FAIL}tp {st_tp} does not detect {fault}.{bcolors.ENDC}')

    return passed

def generate_tests():
    """
    each cmd file:
        READ ...
                
        PODEM fault_num stuck_val output

        QUIT
    """

    cmd_tests_files = []
    output_files = []
    for ckt in os.listdir(CIRCUIT_DIR):
        ckt =  ckt.replace('.ckt','')
        
        fault_list = open(f'./auto-tests-phase3/inputs/_podem/_{ckt}_podem_faults.txt','r').readlines()
        idx = 0
        for fault in fault_list:
            fout = f'{PODEM_CMD_TESTS_DIR}{ckt}_podem_{idx}.txt'
            with open(fout, 'w') as ftest:
                f_num, s_val = list(fault.strip().split(' '))
                ftest.write(f'READ ../{CIRCUIT_DIR}{ckt}.ckt\n')

                ftest.write(f'PODEM {f_num} {s_val} ../{PODEM_OUTPUT_DIR}{ckt}-PODEM-{f_num}-{s_val}.txt\n')

                ftest.write('QUIT')
            output_files.append(f'./{PODEM_OUTPUT_DIR}{ckt}-PODEM-{f_num}-{s_val}.txt')
            cmd_tests_files.append(f'{ckt}_podem_{idx}.txt')

            idx+=1
        
    return cmd_tests_files, output_files

if __name__ == '__main__':

    cmd_test_files, output_files = generate_tests()
    pass_all = True
    failed_count = 0
    passed_count = 0

    for i in range(len(cmd_test_files)):
        cmd_test = cmd_test_files[i]
        student_answer = output_files[i]
        try:
            gold_fail = False
            print('_'*50)
            ckt = cmd_test.replace('.txt','').replace('_podem','').split('_')[0]
            print('Testing PODEM on ' + cmd_test.replace('.txt', ''))
            os.system(f"cd build/; ./simulator < ../{PODEM_CMD_TESTS_DIR}{cmd_test} > /dev/null ")
            
            fault_str=student_answer.replace('.txt','').split('PODEM-')[-1].replace('-','@')

            if check_podem(student_answer, ckt, fault_str):
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