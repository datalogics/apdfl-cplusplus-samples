import os
import shutil
from shutil import ignore_patterns
from git import Repo

PATHSEP = os.path.sep
print('Cloning apdfl-samples....')
Repo.clone_from('git@github.com:datalogics/apdfl-samples.git', 'apdfl-samples', branch='develop-18')
print('Cloning DLE....')
Repo.clone_from('git@github.com:datalogics/dle.git', 'dle', branch='develop-18')

print('Copying APDFL samples....')
source = os.path.join('apdfl-samples')
for root, dirs, files in os.walk(source):
    # Avoid copying files in this folder
    # Some stray README.md files
    dirs[:] = [d for d in dirs if not d.startswith('_Input')]
    for file in files:
        if file.endswith(".cpp") or file.endswith(".h") or file.endswith(".md"):
            source_dir = root.split(f'apdfl-samples{PATHSEP}')
            if source_dir.__len__() > 1:
                dest_dir = os.path.join('CPlusPlus', 'Sample_Source', source_dir[1])
            else:
                dest_dir = os.path.join('CPlusPlus', 'Sample_Source')
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            dest = os.path.join(dest_dir, file)
            print('Copying', os.path.join(root, file), 'to', dest)
            shutil.copy2(os.path.join(root, file), dest)

print('Copying APDFL samples input....')
source = os.path.join('apdfl-samples', '_Input')
dest_dir = os.path.join('Sample_Input')
shutil.rmtree(dest_dir)
shutil.copytree(source, dest_dir, ignore=ignore_patterns('*.md'))

print('Copying DLE DotNetFramework samples....')
source = os.path.join('dle', 'Samples', 'DotNetFramework')
for root, dirs, files in os.walk(source):
    for file in files:
        if file.endswith(".cs") or file.endswith(".md"):
            source_dir = root.split(f'dle{PATHSEP}Samples{PATHSEP}DotNetFramework{PATHSEP}')
            if source_dir.__len__() > 1:
                dest_dir = os.path.join('DotNETFramework', 'Sample_Source', source_dir[1])
            else:
                dest_dir = os.path.join('DotNETFramework', 'Sample_Source')
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            dest = os.path.join(dest_dir, file)
            print('Copying', os.path.join(root, file), 'to', dest)
            shutil.copy2(os.path.join(root, file), dest)

print('Copying DLE DotNet samples....')
source = os.path.join('dle', 'Samples', 'DotNet')
for root, dirs, files in os.walk(source):
    for file in files:
        if file.endswith(".cs") or file.endswith(".md"):
            source_dir = root.split(f'dle{PATHSEP}Samples{PATHSEP}DotNet{PATHSEP}')
            if source_dir.__len__() > 1:
                dest_dir = os.path.join('DotNET', 'Sample_Source', source_dir[1])
            else:
                dest_dir = os.path.join('DotNET', 'Sample_Source')
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            dest = os.path.join(dest_dir, file)
            print('Copying', os.path.join(root, file), 'to', dest)
            shutil.copy2(os.path.join(root, file), dest)

print('Copying DLE Java samples....')
source = os.path.join('dle', 'Samples', 'Java')
for root, dirs, files in os.walk(source):
    for file in files:
        if file.endswith(".java") or file.endswith(".md"):
            source_dir = root.split(f'dle{PATHSEP}Samples{PATHSEP}Java{PATHSEP}src{PATHSEP}main{PATHSEP}java{PATHSEP}com{PATHSEP}datalogics{PATHSEP}pdfl{PATHSEP}samples{PATHSEP}')
            if file.endswith(".md"):
                if source_dir.__len__() > 1:
                    dest_dir = os.path.join('Java', 'Sample_Source', source_dir[1])
                else:
                    dest_dir = os.path.join('Java', 'Sample_Source')
            else:
                if source_dir.__len__() > 1:
                    dest_dir = os.path.join('Java', 'Sample_Source', source_dir[1], 'src', 'com', 'datalogics', 'PDFL', 'Samples')
                else:
                    # skip this javaviewer folder
                    dest_dir = os.path.join('Java', 'Sample_Source', source_dir[0], 'src', 'com', 'datalogics', 'PDFL', 'Samples')
                    continue
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            dest = os.path.join(dest_dir, file)
            print('Copying', os.path.join(root, file), 'to', dest)
            shutil.copy2(os.path.join(root, file), dest)

print('Copying DLE samples input....')
source = os.path.join('dle', 'Samples', 'Data')
dest_dir = os.path.join('Sample_Input')
for root, dirs, files in os.walk(source):
    # Avoid copying files in this folder into Sample_Input
    dirs[:] = [d for d in dirs if not d.startswith('ExtractTextFromMultiRegions')]
    # These are files only used in tests or not associated with samples
    for file in files:
        if ((file == 'DuplicateOCGs.pdf') or
            (file == 'LocallyBuiltSample.pdf') or
            (file == 'pages_as_images.pdf') or
            (file == 'SConscript') or
            (file == 'README.md')):
            continue
        print('Copying', os.path.join(root, file), 'to', dest_dir)
        shutil.copy2(os.path.join(root, file), dest_dir)
