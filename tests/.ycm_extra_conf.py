import platform
import os
import ycm_core

flags = [
    '-Wall',
    '-Wextra',
    '-x', 'c++',
    '-std=c++11',
    '-I', 'include',
    '-I', '/usr/src/gtest/include',
]

SOURCE_EXTENSIONS = [ '.cc', ]

def FlagsForFile( filename, **kwargs ):
    return {
        'flags': flags,
        'do_cache': True
    }
