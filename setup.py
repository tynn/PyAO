#!/usr/bin/env python

"""Setup script for the Ao module distribution.
Configuration in particular could use some work."""

import sys
from distutils.core import setup, Extension

setup (# Distribution meta-data
        name = "pyao",
        version = "1.2",
        description = "A wrapper for the ao library",
        author = "Christian Schmitz",
        author_email = "tynn.dev@gmail.com",
        url = "https://github.com/tynn/PyAO",
        license = 'GPLv3+',

        # Description of the modules and packages in the distribution

        ext_modules = [Extension(
                name = 'ao' if sys.version_info.major >= 3 else 'aomodule',
                sources = ['src/aomodule.c'],
                depends = ['src/aomodule.h'],
                libraries = ["ao"]
			)]
)

