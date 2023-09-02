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

#include "aomodule.h"
#include <assert.h>

static int
uint_32_obj (PyObject *obj, void *addr) {
  unsigned long value;
  uint_32 *value_32 = (uint_32 *) addr;

  value = PyLong_AsUnsignedLong(obj);

  if (value != (unsigned long)-1 || !PyErr_Occurred()) {
    if (value <= 0xffffffff) {
      *value_32 = (uint_32) value;
      return 1;
    }

    PyErr_SetString(PyExc_OverflowError, "long int too large to convert");
  }

  return 0;
}

static ao_option *
dict_to_options(PyObject *dict)
{
  Py_ssize_t pos = 0;
  PyObject *key = NULL, *val = NULL, *tuple;
  ao_option *head = NULL;
  int ret = 1;
  const char *key_str, *val_str;

  if (!PyDict_Check(dict)) {
    PyErr_SetString(PyExc_TypeError, "Must be a dictionary");
    return NULL;
  }

  tuple = PyTuple_New(2);
  if (!tuple)
    return NULL;

  while (PyDict_Next(dict, &pos, &key, &val)) {
    PyTuple_SET_ITEM(tuple, 0, key);
    PyTuple_SET_ITEM(tuple, 1, val);

    if (!(ret = PyArg_ParseTuple(tuple, "ss", &key_str, &val_str)))
      break;

    if (!(ret = ao_append_option(&head, key_str, val_str))) {
      PyErr_SetString(Py_aoError, "Error appending options");
      break;
    }
  }

  Py_XINCREF(key);
  Py_XINCREF(val);
  Py_DECREF(tuple);

  if (ret)
    return head;

  ao_free_options(head);
  return NULL;
}

/*
   Helper function to parse everything out of the argument list.

   Returns 0 on failure.
*/
static int
parse_args(PyObject *args, PyObject *kwargs,
           ao_sample_format *format, PyObject **py_options,
           const char **filename,
           int *driver_id,
           int *overwrite)
{
  static const char *driver_id_kwlist[] = {"driver_id", "bits", "rate",
                                           "channels", "byte_format",
                                           "matrix", "options", "filename",
                                           "overwrite", NULL};
  static const char *driver_name_kwlist[] = {"driver_name", "bits",
                                             "rate", "channels",
                                             "byte_format", "matrix",
                                             "options", "filename",
                                             "overwrite", NULL};
  const char *driver_name = NULL;

  assert(py_options != NULL);
  assert(format != NULL);
  assert(filename != NULL);
  assert(driver_id != NULL);
  assert(overwrite != NULL);

  /* Set the default values */
  format->bits = 16;
  format->rate = 44100;
  format->channels = 2;
  format->byte_format = 1; /* What should this be by default? Anyone? */
  format->matrix = NULL;

  if(!(PyArg_ParseTupleAndKeywords(args, kwargs, "i|iiiisO!si",
                                   (char **) driver_id_kwlist,
                                   driver_id,
                                   &format->bits,
                                   &format->rate,
                                   &format->channels,
                                   &format->byte_format,
                                   &format->matrix,
                                   &PyDict_Type, py_options,
                                   filename, overwrite))) {
    PyErr_Clear();
    if(!(PyArg_ParseTupleAndKeywords(args, kwargs, "|ziiiisO!si",
                                   (char **) driver_name_kwlist,
                                   &driver_name,
                                   &format->bits,
                                   &format->rate,
                                   &format->channels,
                                   &format->byte_format,
                                   &format->matrix,
                                   &PyDict_Type, py_options,
                                   filename, overwrite)))
      return 0;

    if (driver_name)
      *driver_id = ao_driver_id(driver_name);
    else
      *driver_id = ao_default_driver_id();
  }

  return 1;
}

/*
  Actually create a new AudioDevice object
*/
static PyObject*
py_ao_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
  int driver_id, overwrite = 0;
  const char *filename = NULL;
  PyObject *py_options = NULL;
  ao_option *c_options = NULL;
  ao_device *dev;
  ao_sample_format sample_format;
  ao_Object *retobj;

  if (!parse_args(args, kwargs,
                  &sample_format, &py_options,
                  &filename, &driver_id, &overwrite))
    return NULL;

  if (py_options && PyDict_Size(py_options) > 0) {
    /* dict_to_options returns NULL on error, so you can't pass
       an empty dictionary. We can skip this then anyway. */

    c_options = dict_to_options(py_options);
    if (!c_options) {
      return NULL;
    }
  }

  if (filename == NULL) {
    dev = ao_open_live(driver_id, &sample_format, c_options);
  } else {
    dev = ao_open_file(driver_id, filename, overwrite,
                       &sample_format, c_options);
  }
  ao_free_options(c_options);

  if (dev == NULL) {
    PyErr_SetString(Py_aoError, "Error opening device.");
    return NULL;
  }

  retobj = PyObject_New(ao_Object, type);
  retobj->dev = dev;
  retobj->driver_id = driver_id;
  return (PyObject *) retobj;
}

static void
py_ao_dealloc(ao_Object *self)
{
  ao_close(self->dev);
  PyObject_Del(self);
}

static PyObject *
py_ao_driver_id(PyObject *self, PyObject *args)
{
  int driver_id;
  const char *str;

  if (!PyArg_ParseTuple(args, "s", &str))
    return NULL;

  driver_id = ao_driver_id(str);

  if (driver_id < 0) {
    PyErr_SetString(Py_aoError, "No such driver");
    return NULL;
  }

  return PyInt_FromLong(driver_id);
}

static PyObject *
info_to_obj(ao_info *info) {
  static char *keys[] = {"name", "short_name", "author", "comment"};
  PyObject *retdict;
  PyObject *string;
  char *strings[4];
  int i;

  if (!info) {
    PyErr_SetString(Py_aoError, "Error getting info");
    return NULL;
  }

  retdict = PyDict_New();
  if (retdict) {
    strings[0] = info->name;
    strings[1] = info->short_name;
    strings[2] = info->author;
    strings[3] = info->comment;

    for (i = 0; i < 4; i++) {
      string = PyString_FromString(strings[i]);
      if (!string || PyDict_SetItemString(retdict, keys[i], string) < 0) {
        Py_CLEAR(retdict);
        break;
      }
    }
  }

  return retdict;
}

static PyObject *
py_ao_driver_info(PyObject *self, PyObject *args)
{
  const char *driver_name = NULL;
  int driver_id = 0;
  ao_info **info_list;
  PyObject *retlist;
  PyObject *item;

  if (self != NULL && PyObject_TypeCheck(self, &ao_Type)) {

    /* It's a method */
    if (!(PyArg_ParseTuple(args, "")))
      return NULL;

    driver_id = ((ao_Object *) self)->driver_id;

  } else {

    /* Maybe it's a string, None or none */
    if ((PyArg_ParseTuple(args, "|z", &driver_name))) {
      if (driver_name) {

        driver_id = ao_driver_id(driver_name);

      } else {

        info_list = ao_driver_info_list(&driver_id);

        retlist = PyList_New(driver_id);
        if (retlist)
          while (driver_id--) {
            item = info_to_obj(info_list[driver_id]);
            if (!item || PyList_SetItem(retlist, driver_id, item) < 0) {
              Py_CLEAR(retlist);
              break;
            }
          }

        return retlist;
      }

    } else {

      /* Maybe it's an int */
      PyErr_Clear();
      if (!(PyArg_ParseTuple(args, "i", &driver_id)))
        return NULL;
    }
  }

  if (driver_id < 0) {
    PyErr_SetString(Py_aoError, "No such driver");
    return NULL;
  }

  return info_to_obj(ao_driver_info(driver_id));
}

static PyObject *
py_ao_file_extension(PyObject *self, PyObject *args)
{
  const char *driver_name;
  int driver_id;

  if (self != NULL && PyObject_TypeCheck(self, &ao_Type)) {

    /* It's a method */
    if (!(PyArg_ParseTuple(args, "")))
      return NULL;

    driver_id = ((ao_Object *) self)->driver_id;

  } else {

    /* Maybe it's a string, None or none */
    if ((PyArg_ParseTuple(args, "s", &driver_name))) {

      driver_id = ao_driver_id(driver_name);

    } else {

      /* Maybe it's an int */
      PyErr_Clear();
      if (!(PyArg_ParseTuple(args, "i", &driver_id)))
        return NULL;
    }
  }

  if (driver_id >= 0) {
#ifndef AO_FE_MISSING
    driver_name = ao_file_extension(driver_id);
    if (driver_name)
      return PyString_FromString(driver_name);
#endif

    if (ao_driver_info(driver_id))
      Py_RETURN_NONE;
  }

  PyErr_SetString(Py_aoError, "No such driver");
  return NULL;
}

static PyObject *
py_ao_play(ao_Object *self, PyObject *args)
{
  const char *samples;
  char *output_samples;
  uint_32 num_bytes = 0;

#if PY_MAJOR_VERSION >= 3
  Py_ssize_t len;
  if (!(PyArg_ParseTuple(args, "y#|O&", &samples, &len, uint_32_obj, &num_bytes)))
#else
  int len;
  if (!(PyArg_ParseTuple(args, "s#|O&", &samples, &len, uint_32_obj, &num_bytes)))
#endif
    return NULL;

  if (num_bytes == 0 || num_bytes > (uint_32) len)
    num_bytes = len;

  output_samples = PyMem_New(char, num_bytes);
  if (!samples)
    return PyErr_NoMemory();

  Py_BEGIN_ALLOW_THREADS
  memcpy(output_samples, samples, sizeof(char) * num_bytes);
  ao_play(self->dev, output_samples, num_bytes);
  Py_END_ALLOW_THREADS

  PyMem_Del(output_samples);

  Py_RETURN_NONE;
}

static PyObject*
py_ao_is_big_endian(PyObject *self, PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  if (ao_is_big_endian())
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

static PyObject*
py_ao_default_driver_id(PyObject *self, PyObject *args)
{
  /* Passed with METH_NOARGS */
  return PyInt_FromLong(ao_default_driver_id());
}


#define AddInt(x) PyModule_AddObject(module, #x, PyInt_FromLong(x));

#if PY_MAJOR_VERSION >= 3
#define RET_MOD (PyObject *)
static PyModuleDef aomodule = {
  PyModuleDef_HEAD_INIT, "ao", docstring,
  -1, ao_methods,
  NULL, NULL, NULL, NULL
};
PyMODINIT_FUNC
PyInit_ao(void)
#else
#define RET_MOD (void)
#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initao(void)
#endif
{
  PyObject *module;

  if (PyType_Ready(&ao_Type) < 0)
    return RET_MOD NULL;

  Py_aoError = PyErr_NewException("ao.aoError", NULL, NULL);
  if (!Py_aoError)
    return RET_MOD NULL;

#if PY_MAJOR_VERSION >= 3
  module = PyModule_Create(&aomodule);
#else
  module = Py_InitModule3("ao", ao_methods, docstring);
#endif

  if (!module) {
    Py_DECREF(Py_aoError);
    return RET_MOD NULL;
  }

  Py_INCREF(&ao_Type);
  PyModule_AddObject(module, "AudioDevice", (PyObject *) &ao_Type);

  Py_INCREF(Py_aoError);
  PyModule_AddObject(module, "aoError", Py_aoError);

#ifdef AO_FMT_LITTLE
  AddInt(AO_FMT_LITTLE);
#endif

#ifdef AO_FMT_BIG
  AddInt(AO_FMT_BIG);
#endif

#ifdef AO_FMT_NATIVE
  AddInt(AO_FMT_NATIVE);
#endif

#ifdef AO_NULL
  AddInt(AO_NULL);
#endif

#ifdef AO_OSS
  AddInt(AO_OSS);
#endif

#ifdef AO_IRIX
  AddInt(AO_IRIX);
#endif

#ifdef AO_SOLARIS
  AddInt(AO_SOLARIS);
#endif

#ifdef AO_WIN32
  AddInt(AO_WIN32);
#endif

#ifdef AO_BEOS
  AddInt(AO_BEOS);
#endif

#ifdef AO_ESD
  AddInt(AO_ESD);
#endif

#ifdef AO_ALSA
  AddInt(AO_ALSA);
#endif

#ifdef AO_WAV
  AddInt(AO_WAV);
#endif

#ifdef AO_RAW
  AddInt(AO_RAW);
#endif

#ifdef AO_DRIVERS
  AddInt(AO_DRIVERS);
#endif

  ao_initialize();

  return RET_MOD module;
}

