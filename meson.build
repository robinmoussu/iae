project('iae', 'cpp')

cxxflags = ['-g', '-O3', '-std=c++17', '-Wall', '-Wunused-command-line-argument']

incdir = include_directories(['include', 'tests'])
sources = run_command('find_sources.sh').stdout().strip().split('\n')

executable('iae', sources, include_directories: incdir, cpp_args: cxxflags)

