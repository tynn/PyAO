/*
 *  Copyright (C) 2001-2004  Andrew Chatham  <andrew.chatham@duke.edu>
 *  Copyright (C) 2015       Christian Schmitz  <tynn.dev@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AO_MODULE_H__
#define __AO_MODULE_H__

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <structmember.h>
#include <ao/ao.h>

#if PY_MAJOR_VERSION >= 3
#define PyInt_FromLong PyLong_FromLong
#define PyString_FromString PyUnicode_FromString
#endif

#define OPTSIZE 1024

typedef struct {
  PyObject_HEAD
  ao_device *dev;
  int driver_id;
} ao_Object;

static PyObject *Py_aoError;

static ao_option * dict_to_options(PyObject *);

static PyObject *py_ao_new(PyTypeObject *, PyObject *, PyObject *);

static void py_ao_dealloc(ao_Object *);

static char py_ao_play_doc[] =
"Play the contents of a given audio buffer.\n\
\n\
Arguments:\n\
buff : Buffer or string containing audio data\n\
n : Number of bytes to play (defaults to len(buff))";

static PyObject *py_ao_play(ao_Object *, PyObject *);

static char py_ao_driver_id_doc[] =
"Return the integer identifier for the driver with the given name (or object).";
static PyObject *py_ao_driver_id(PyObject *, PyObject *);

static char py_ao_driver_info_doc[] =
"Return a dictionary of information about a driver.\n\
\n\
It can either be called as a member function of an AudioDevice object:\n\
   x.driver_info()\n\
or as a standalone function which takes the integer id or the string name of the\n\
driver:\n\
   ao.driver_info(1) or ao.driver_info(\"pulse\")\n\
If None is passed (or the argument omitted) information for all drivers\n\
is returned as a list of dictionaries:\n\
   ao.driver_info() or ao.driver_info(None)";
static PyObject *py_ao_driver_info(PyObject *, PyObject *);

static char py_ao_is_big_endian_doc[] =
"Returns the endianness of the current host.";
static PyObject *py_ao_is_big_endian(PyObject *, PyObject *);

static char py_ao_doc[] =
"AudioDevice([driverid], bits=16, rate=44100, channels=2, byte_format=1, options=[], filename='', overwrite=0)\n\
OR\
AudioDevice(drivername, bits=16, rate=44100, channels=2, byte_format=1, options=[], filename='', overwrite=0)\n\
\n\
An AudioDevice object is an interface to a sound device. You can either pass\n\
an id (defaults to ao_default_driver_id) of a specific type of device or the\n\
name of that device type.\n\
If filename is passed, the module will try to open an output file as the\n\
audio device. In this case, overwrite indicates whether to overwrite an\n\
existing file\n";

static PyObject *py_ao_default_driver_id(PyObject *, PyObject *);
static char py_ao_default_driver_id_doc[] =
"Returns the ID number of the default live output driver.\n\
\n\
If the configuration files specify a default driver, its ID is returned,\n\
otherwise the library tries to pick a live output driver that will work\n\
on the host platform.\n\
\n\
Return values:\n\
    . a non-negative value is the ID number of the default driver\n\
    . -1 indicates failure to find a usable audio output device\n\
\n\
Notes:\n\
    If no default device is available, you may still use the \n\
    null device to test your application.";

static PyObject *py_ao_file_extension(PyObject *, PyObject *);
static char py_ao_file_extension_doc[] =
"Returns the normal file extension associated with a particular driver (like \"wav\" or \"au\").\n\
\n\
This is just an information function to allow library users to guess appropriate\n\
file names. You can safely ignore the recommended extension.\n\
\n\
Return Values:\n\
    . string containing suggested file extension.\n\
    . None if this driver has no file extension associated with it.";


struct PyMethodDef ao_Object_methods[] = {
  {"driver_info", py_ao_driver_info,
   METH_VARARGS, py_ao_driver_info_doc},
  {"file_extension", py_ao_file_extension,
   METH_VARARGS, py_ao_file_extension_doc},
  {"play", (PyCFunction) py_ao_play,
   METH_VARARGS, py_ao_play_doc},
  {NULL, NULL},
};

static PyMemberDef ao_Object_members[] = {
  {"driver_id", T_INT,
   offsetof(ao_Object, driver_id),
   READONLY, py_ao_driver_id_doc},
  {NULL},
};

static PyTypeObject ao_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "ao.AudioDevice",
  sizeof(ao_Object),
  0,
  (destructor) py_ao_dealloc,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  Py_TPFLAGS_DEFAULT,
  py_ao_doc,
  0,
  0,
  0,
  0,
  0,
  0,
  ao_Object_methods,
  ao_Object_members,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  py_ao_new,
};


struct PyMethodDef ao_methods[] = {
  {"driver_id", py_ao_driver_id,
   METH_VARARGS, py_ao_driver_id_doc},
  {"driver_info", py_ao_driver_info,
   METH_VARARGS, py_ao_driver_info_doc},
  {"is_big_endian", py_ao_is_big_endian,
   METH_VARARGS, py_ao_is_big_endian_doc},
  {"default_driver_id", py_ao_default_driver_id,
   METH_NOARGS, py_ao_default_driver_id_doc},
  {"file_extension", py_ao_file_extension,
   METH_VARARGS, py_ao_file_extension_doc},
  {NULL, NULL}
};

static char docstring[] =
"A Python wrapper for the ao library using in the ogg project.";


#endif /* __AO_MODULE_H__ */

