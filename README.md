# PyAO
###### a Python wrapper module for the ao library

This is a wrapper for [libao](https://xiph.org/ao/), an audio device
abstraction library.
libao is available with ogg/vorbis at http://www.xiph.org.

This version wraps the libao version 1.2.0 and supports all of the latest
Python versions.

### Build
Build the extension modules with:

	python setup build_ext

#### Using version 1.1.0 of libao
In version 1.1.0 of libao the implementation of `ao_file_extension()` is
missing. To bypass calling this function, build with:

	python setup build_ext -DAO_FE_MISSING

### A note
Because of the way the AO API works, if you are opening a device that outputs
to a file (like raw or wav), then you HAVE to pass the filename as a keyword
parameter to the constructor (like above). It can't just be an option (you
used to be able to do that). Opening a "live" device (e.g. oss or alsa), you
obviously don't have to worry about the filename.

### Historical information

PyAO was developed as part of the
[PyOgg](http://ekyo.nerim.net/software/pyogg/) project.  
This is a continuation with approval of the last maintainer
Eric Faurot <eric.faurot@gmail.com>.

#### Original author
Andrew Chatham <andrew.chatham@duke.edu>

