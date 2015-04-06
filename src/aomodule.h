#ifndef __AO_MODULE_H__
#define __AO_MODULE_H__

#include <Python.h>
#include <ao/ao.h>

#define OPTSIZE 1024

typedef struct {
  PyObject_HEAD
  ao_device *dev;
} ao_Object;

static PyObject *Py_aoError;

static ao_option * dict_to_options(PyObject *);

static PyObject *py_ao_new(PyObject *, PyObject *, PyObject *);

static void py_ao_dealloc(ao_Object *);
static PyObject* py_ao_getattr(PyObject *, char *);

static char py_ao_play_doc[] = 
"Play the contents of a given audio buffer.\n\
\n\
Arguments:\n\
buff : Buffer or string containing audio data\n\
n : Number of bytes to play (defaults to len(buff))";

static PyObject *py_ao_play(PyObject *, PyObject *);

static char py_ao_driver_id_doc[] = 
"Return the integer identifier for the driver with the given name (or object).";
static PyObject *py_ao_driver_id(PyObject *, PyObject *);

static char py_ao_driver_info_doc[] =
"Return a dictionary of information about a driver.\n\
\n\
It can either be called as a member function of an AudioDevice object:\n\
   x.driver_info()\n\
or as a standalone function which takes the integer id of the driver:\n\
   driver_info(1)";
static PyObject *py_ao_driver_info(PyObject *, PyObject *);

static char py_ao_is_big_endian_doc[] =
"Returns the endianness of the current host.";
static PyObject *py_ao_is_big_endian(PyObject *, PyObject *);

static char py_ao_doc[] = 
"AudioDevice(driverid, bits=16, rate=44100, channels=2, byte_format=1, options=[], filename='', overwrite=0)\n\
OR\
AudioDevice(drivername, bits=16, rate=44100, channels=2, byte_format=1, options=[], filename='', overwrite=0)\n\
\n\
An AudioDevice object is an interface to a sound device. You can either pass\n\
an id of a specific type of device or the name of that device type.\n\
If filename is passed, the module will try to open an output file as the\n\
audio device. In this case, overwrite indicates whether to overwrite an\n\
existing file\n";

static PyTypeObject ao_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "AudioDevice",
  sizeof(ao_Object),
  0,

  /* Standard Methods */
  (destructor) py_ao_dealloc,
  (printfunc) 0,
  (getattrfunc) py_ao_getattr,
  (setattrfunc) 0,
  (cmpfunc) 0,
  (reprfunc) 0,

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
  py_ao_doc,
};


struct PyMethodDef ao_Object_methods[] = {
  {"driver_info", py_ao_driver_info, 
   METH_VARARGS, py_ao_driver_info_doc},
  {"play", py_ao_play, 
   METH_VARARGS, py_ao_play_doc},
  {NULL, NULL},
};

struct PyMethodDef ao_methods[] = {
  {"AudioDevice", (PyCFunction) py_ao_new, 
   METH_VARARGS|METH_KEYWORDS, py_ao_doc},
  {"driver_id", py_ao_driver_id, 
   METH_VARARGS, py_ao_driver_id_doc},
  {"driver_info", py_ao_driver_info, 
   METH_VARARGS, py_ao_driver_info_doc},
  {"is_big_endian", py_ao_is_big_endian, 
   METH_VARARGS, py_ao_is_big_endian_doc},
  {NULL, NULL}
};

static char docstring[] = 
"A Python wrapper for the ao library using in the ogg project.";


#endif /* __AO_MODULE_H__ */







