import argparse
import csv
import os

def get_files_in_folder(folder:str):
    l = []
    for item in os.listdir(folder):
        path = f'{folder}/{item}'
        file_path, ext = os.path.splitext(path)
        if os.path.isfile(path) and ext == ".txt":
            l.append(f'{path}')
    return l

def get_files(folder: str):
    l = {}
    for item in os.listdir(folder):
        item_path = f'{folder}{os.sep}{item}'
        if os.path.isdir(item_path):
            l[item] = get_files_in_folder(item_path)
    return l

def parse_cnf_txt_result(file: str, number: int):

    with open(file,'r') as resobj:
        lines = resobj.readlines()
        if len(lines) <= 2:
            return {
                'Test': str(number),
                'Max RSS': '-1',
                'Algo time': '-1',
                'Result': 'UNKNOWN'
            }
        result = {'Test': str(number), 'Result': lines[1].split(':')[1].strip(),
                  'Max RSS': lines[6].split(':')[1].strip()[:-4], 'Algo time': lines[4].split(':')[1].strip()[:-2]}
        return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="data_sniffer.py",
                                    description="Process benchmark results")
    parser.add_argument('-t', '--test-results-directory',
                        required=True,
                        help="Path to folder containing .cnf.txt files")
    args = parser.parse_args()
    if not os.path.exists(args.test_results_directory):
        print(f'{args.test_directory} does not exist')
        exit(1)

    files = get_files(args.test_results_directory)
    print('asdsad')
    rundir = os.getcwd()
    for folder in files.keys():
        os.chdir(args.test_results_directory)
        with open(f'{os.path.basename(folder)}.csv','w') as ofobj:
            print(f'Saving to {os.path.basename(folder)}.csv')
            c_writer = csv.writer(ofobj)
            c_writer.writerow(['Test','Max RSS','Algo time', 'Result'])
            lst = []
            os.chdir(rundir)
            for file in files[folder]:
                test_number = int(os.path.basename(file).split('.')[0][4:])
                test_result  = parse_cnf_txt_result(file,test_number)
                c_writer.writerow([test_result['Test'],test_result['Max RSS'],test_result['Algo time'], test_result['Result']])
