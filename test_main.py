# Do NOT change this file.

import os
import argparse

WEB_ADDR = "https://sportlab.usc.edu/~msabrishami/files"
WEB_DIR = "ee658-spring-2023"
TEST_DIR = "auto-tests-phase3"
ZIP_NAME = f"{TEST_DIR}.zip" 

TP_COUNT = 10

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'

def prepare_env():
    """download tests/create folders"""

    os.system(f"rm -rf {TEST_DIR}")
    os.system(f"mkdir {TEST_DIR}")
    os.system(f"rm -rf {ZIP_NAME}")

    if os.path.exists("/etc/hostname") and \
            ("viterbi-scf" in open("/etc/hostname").readline().strip()):
        os.system(f"cp /home/{WEB_DIR}/{ZIP_NAME} ./")
    else: 
        os.system(f"wget {WEB_ADDR}/{WEB_DIR}/{ZIP_NAME}")
    
    os.system(f"unzip {ZIP_NAME} -d ./{TEST_DIR} > /dev/null") 
    os.system(f"rm -rf {ZIP_NAME}")


if __name__ == '__main__':

    prepare_env()

    os.system("rm -rf build; mkdir build && cd build; cmake ..; make")
    
    if not os.path.exists('./build/simulator'):
        print('cpp file not compiled.')
        exit(1)

    parser = argparse.ArgumentParser(description ='Which function to test')
    parser.add_argument('-f', '--func', metavar='function', 
            choices={'rtpg', 'rfl', 'logicsim', 'dfs', 'pfs', 'podem',
                'scoap', 'dalg', 'podem', 'all'}, required=True)

    exit_val = None
    try:
        args = parser.parse_args()
        if args.func == 'all':
            for t in ['rtpg','rfl','logicsim','dfs','pfs','podem','scoap','dalg','podem']:
                if os.system(f'python3 test_{t}.py') != 0:
                    print(f'{t} failed.')
                    exit_val = 0
                else:
                    print(f'{t} passed.')

        elif os.system(f'python3 test_{args.func}.py') != 0:
            print(f'{args.func} failed.')
            exit_val = 1
        else:
            print(f'{args.func} passed.')
            exit_val = 0
    except:
        exit_val = 1

    os.system(f'rm -rf {TEST_DIR}')
    exit(exit_val)
