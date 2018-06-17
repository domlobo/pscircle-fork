import platform
import os
import ycm_core

flags = [
    '-Wall',
    '-Wextra',
    '-x', 'c++',
    '-std=c++11',
    '-I', 'include',
    '-I', 'build',
    '-I', '/usr/src/gtest',
    '-I', '/usr/src/gtest/include',
    '-I', '/usr/include/glib-2.0', 
    '-I', '/usr/lib/x86_64-linux-gnu/glib-2.0/include'
]

SOURCE_EXTENSIONS = [ '.cc', ]

def FlagsForFile( filename, **kwargs ):
    return {
        'flags': flags,
        'do_cache': True
    }
