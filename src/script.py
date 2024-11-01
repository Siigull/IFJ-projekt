import os
import re
import glob

def generate_hh_file(h_file_path):
    with open(h_file_path, 'r') as file:
        h_content = file.readlines()
    
    hh_file_path = h_file_path.replace('.h', '.hh')
    
    found = 0

    includes = ""

    for line in h_content:
        if re.search(r'#include "\w+\.h"', line):
            line = line[:-1]
            line = line.rstrip()
            includes += line[:-1] + "h\"\n"

    with open(hh_file_path, 'w') as hh_file:
        for line in h_content:
            if re.search("#*_H", line):
                hh_file.write(line[:-1] + "H\n")
                found += 1
            else:
                if found == 2:
                    hh_file.write(includes)
                    found += 1
                    hh_file.write('extern "C" {\n')

                if "#endif" in line:
                    hh_file.write("\n}\n")
                
                if re.search(r'#include "\w+\.h"', line):
                    pass
                else: 
                    hh_file.write(line)

header_files = glob.glob(os.path.join(".", '*.h'))

for file_path in header_files:
    generate_hh_file(file_path)
