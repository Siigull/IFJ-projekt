import os
import re

def generate_hh_file(h_file_path):
    with open(h_file_path, 'r') as file:
        h_content = file.readlines()
    
    hh_file_path = h_file_path.replace('.h', '.hhh')
    
    found = 0

    with open(hh_file_path, 'w') as hh_file:
        for line in h_content:
            if re.search("#*_H", line):
                hh_file.write(line[:-1] + "H\n")
                found += 1
            else:
                if found == 2:
                    found += 1
                    hh_file.write('extern "C" {\n')

                if "#endif" in line:
                    hh_file.write("\n}\n")
                
                if re.search(r'#include "\w+\.h"', line):
                    line = line[:-1]
                    line = line.rstrip()
                    hh_file.write(line[:-1] + "h\"\n")
                else: 
                    hh_file.write(line)

generate_hh_file("expressionparser.h")