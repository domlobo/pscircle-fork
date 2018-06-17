import platform
import os
import ycm_core

flags = [
    '-Wall',
    '-Wextra',
    '-Wpedantic',
    '-x', 'c',
    '-I', 'include',
    '-I', 'build',
    '-I', '/usr/include/cairo',
]

SOURCE_EXTENSIONS = [ '.c', ]

def FlagsForFile( filename, **kwargs ):
    return {
        'flags': flags,
        'do_cache': True
    }
