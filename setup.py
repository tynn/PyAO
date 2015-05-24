#!/usr/bin/env python

"""Setup script for the ao module distribution."""

from sys import version_info
try:
 from setuptools import setup, Extension
except:
 from distutils.core import setup, Extension

setup (# Distribution meta-data
        name = "pyao",
        version = "1.2",
        description = "A wrapper for the ao library",
        author = "Andrew Chatham",
        author_email = "andrew.chatham@duke.edu",
        maintainer = "Christian Schmitz",
        maintainer_email = "tynn.dev@gmail.com",
        url = "https://github.com/tynn/PyAO",
        license = 'GPLv3+',

        # Description of the modules and packages in the distribution

        ext_modules = [Extension(
                name = 'ao' if version_info.major >= 3 else 'aomodule',
                sources = ['src/aomodule.c'],
                depends = ['src/aomodule.h'],
                libraries = ["ao"]
			)]
)

