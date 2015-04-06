#!/usr/bin/env python

"""Setup script for the Ao module distribution.
Configuration in particular could use some work."""

import os, sys, re, string
from distutils.core import setup
from distutils.extension import Extension
from distutils.command.config import config
from distutils.command.build import build

def get_setup():
    data = {}
    r = re.compile(r'(\S+)\s*?=\s*(.+)')
    
    if not os.path.isfile('Setup'):
        print "No 'Setup' file. Perhaps you need to run the configure script."
        sys.exit(1)

    f = open('Setup', 'r')
    
    for line in f.readlines():
        m = r.search(line)
        if not m:
            print "Error in setup file:", line
            sys.exit(1)
        key = m.group(1)
        val = m.group(2)
        data[key] = val
        
    return data

data = get_setup()
ao_include_dir = data['ao_include_dir']
ao_lib_dir = data['ao_lib_dir']
ao_libs = string.split(data['ao_libs'])


setup (# Distribution meta-data
        name = "pyao",
        version = "0.82",
        description = "A wrapper for the ao library",
        author = "Andrew Chatham",
        author_email = "andrew.chatham@duke.edu",
        url = "http://dulug.duke.edu/~andrew/pyvorbis.html",
        license = 'GPL',

        # Description of the modules and packages in the distribution

        ext_modules = [Extension(
                name = 'aomodule',
                sources = ['src/aomodule.c'],
                include_dirs = [ao_include_dir],
		library_dirs = [ao_lib_dir],
                libraries = ao_libs)]
)


