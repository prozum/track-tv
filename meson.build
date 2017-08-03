project('track-tv', 'cpp', 'c')
opencv_dep = dependency('opencv')

subdir('firmata')
incdir = include_directories('firmata')

tracktv_exe = executable('track-tv', 'main.cpp',
                         link_with: firmata_lib,
                         dependencies: opencv_dep,
                         include_directories: incdir)
