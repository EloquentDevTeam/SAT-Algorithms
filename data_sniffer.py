import argparse
import csv
import os
from matplotlib import pyplot as plt
import numpy as np
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
        #if(file == 'tests/batch_4/dpll_mcl_results/test1.cnf.result.txt'):
        #    breakpoint()
        max_rss = ''
        algo_time = ''
        result = ''

        if(lines[0].startswith('Start SAT. Result')):
            result = lines[0].split(':')[1].strip()
            max_rss = lines[4].split(':')[1].strip()[:-3]
            algo_time = lines[2].split(':')[1].strip()[:-2]
        elif len(lines) == 7:
            result = lines[1].split(':')[1].strip()
            max_rss = lines[4].split(':')[1].strip()[:-3]
            algo_time = lines[2].split(':')[1].strip()[:-2]
        else:
            result = lines[1].split(':')[1].strip()
            max_rss = lines[6].split(':')[1].strip()[:-3]
            algo_time = lines[4].split(':')[1].strip()[:-2]
        result = {'Test': str(number), 'Result': result,
                  'Max RSS': max_rss, 'Algo time': algo_time}
        if result['Algo time'].startswith('-') :
            breakpoint()
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
        with (open(f'{os.path.basename(folder)}.csv','w') as ofobj):
            with open(f'{os.path.basename(folder)}_processing.txt', 'w') as logobj:
                test_name = os.path.basename(folder)[:-len('_results')]
                logobj.write(f'Saving to {os.path.basename(folder)}.csv\n')
                c_writer = csv.writer(ofobj)
                c_writer.writerow(['Test','Max RSS','Algo time', 'Result'])
                lst = []
                os.chdir(rundir)
                average_algo_time = float(0)
                average_memory_usage = float(0)
                succeeded_test = 0
                max_rss_data = []
                max_algo_time_data = []
                tests = []
                for file in files[folder]:
                    test_number = int(os.path.basename(file).split('.')[0][4:])
                    test_result  = parse_cnf_txt_result(file,test_number)
                    c_writer.writerow([test_result['Test'],test_result['Max RSS'],test_result['Algo time'], test_result['Result']])
                    c_algo_time = float(test_result['Algo time'])
                    tests.append(int(test_result['Test']))
                    result = test_result.items()
                    max_algo_time_data.append(c_algo_time)
                    max_rss_data.append(float(test_result['Max RSS']))
                    if c_algo_time > 0:
                        average_algo_time+=c_algo_time
                        average_memory_usage+=float(test_result['Max RSS'])
                        succeeded_test+=1
                    if average_algo_time < 0:
                        breakpoint()

                    if average_memory_usage < 0:
                        breakpoint()
                # Convert object to a list

                # Convert list to an array
                rss_data = np.array(max_rss_data)
                algo_time_data = np.array(max_algo_time_data)
                test_indices = np.array(tests)
                plt.figure().set_size_inches(8,4)
                plt.scatter(test_indices, rss_data, label = "RSS maxim")
                plt.scatter(test_indices, algo_time_data, label = "Timp de execuție măsurat")
                plt.legend()
                plt.axhline(y=-1, color='red', linestyle='--')
                plt.title(f'{test_name} Utilizare memorie (RSS) și timp maxim de execuție')
                os.chdir(args.test_results_directory)

                plt.savefig(f'{test_name}_graph.png', dpi=300)
                plt.clf()
                os.chdir(rundir)
                if succeeded_test > 0:
                    average_algo_time/=succeeded_test
                    logobj.write(f'Average algo time: {round(average_algo_time,3)}\n')
                    logobj.write(f'Average RSS usage: {round(average_memory_usage,3)}\n')
                else:
                    logobj.write(f'Average algo time: DNF\n')
                    logobj.write(f'Average RSS usage: DNF\n')
