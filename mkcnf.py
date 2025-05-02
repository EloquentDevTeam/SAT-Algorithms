#!/usr/bin/env python3
import os
import argparse
import random
from io import StringIO

def get_literal(clause_literals: list[int], literal_count: int) -> int:
    while True:
        exp = random.randint(1,11)
        polarity = (-1) ** exp
        literal = random.randint(1,literal_count)
        fin_literal = literal * polarity
        if fin_literal not in clause_literals and (-1) * fin_literal not in clause_literals:
            return fin_literal

def get_clauses(literal_count: int, clause_count: int, clause_min_size: int, clause_max_size: int, seed: int | None) -> str:
    output = StringIO()
    output.write(f'p cnf {literal_count} {clause_count}\n')
    for c_clause in range(clause_count):
        clause_size = random.randint(clause_min_size,clause_max_size)
        literals = list()
        for c_literal in range(clause_size):
            l = get_literal(literals, literal_count)
            output.write(str(l))
            literals.append(l)
            output.write(' ')
        output.write('0')
        output.write('\n')
    result = output.getvalue()
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog="mkcnf.py",
                                     description="Conjunctive Normal Form Generator")

    parser.add_argument('--clause-count', type=int, required=True, help='How many clauses should be generated')
    parser.add_argument('--clause-max-size',type=int, required= True, help = 'Maximum number of literals a clause can have')
    parser.add_argument('--clause-min-size',type=int, required=False, default=1, help = 'Minimum number of literals a clause can have')
    parser.add_argument('--literal-count', type=int, required=True, help='How many distinct literals should be in the final formula, bar for their negations')
    parser.add_argument('--batch-size',type=int, default=1, required = False, help='How many tests to generate')
    parser.add_argument('--name',type=str,default='cnf_tests',required=False, help='Name to use for the final folder')
    parser.add_argument('--seed',type=int,default=None, required=False, help='Seed for the random number generator to use')
    parser.add_argument('--path', help='Path to save tests to. A subdirectory will be created there',default=os.getcwd(),required=False)
    # TODO: parser.add_argument('--bias',help='Hints to the formulae generator to get a particular set of formulae')
    parser.add_argument('-f','--force',help='Overwrite existing directory', action='store_true')

    args = parser.parse_args()
    random.seed(args.seed)
    
    if args.clause_min_size < 1:
        print('Clauses must have at least one literal')
        exit(1)

    final_path = f'{args.path}/{args.name}'
    try:
        os.chdir(args.path)
        os.mkdir(args.name)
    except FileExistsError:
        try:
            if args.force:
                pass
            else:
                res = input(f'Folder {args.name} already exists in {args.path}. Overwrite? (Y/N)')
                if not res.startswith('Y') and not res.startswith('y'):
                    print('Aborted.')
                    exit(0)
        except EOFError:
            print('Aborted.')
            exit(0)
    os.chdir(args.name)
    for i in range(args.batch_size):
        result = get_clauses(args.literal_count,args.clause_count,min(args.clause_min_size,args.literal_count), min(args.clause_max_size,args.literal_count) ,args.seed)
        with open(f'test{i+1}.cnf','w') as fwriter:
            fwriter.write(result)