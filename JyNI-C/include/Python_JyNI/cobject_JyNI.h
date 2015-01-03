/* This File is based on cobject.h from CPython 2.7.5 release.
 * It has been modified to suit JyNI needs.
 *
 * Copyright of the original file:
 * Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 * 2011, 2012, 2013, 2014, 2015 Python Software Foundation.  All rights reserved.
 *
 * Copyright of JyNI:
 * Copyright (c) 2013, 2014, 2015 Stefan Richthofer.  All rights reserved.
 *
 *
 * This file is part of JyNI.
 *
 * JyNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JyNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with JyNI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */


/*
   CObjects are marked Pending Deprecation as of Python 2.7.
   The full schedule for 2.x is as follows:
     - CObjects are marked Pending Deprecation in Python 2.7.
     - CObjects will be marked Deprecated in Python 2.8
       (if there is one).
     - CObjects will be removed in Python 2.9 (if there is one).

   Additionally, for the Python 3.x series:
     - CObjects were marked Deprecated in Python 3.1.
     - CObjects will be removed in Python 3.2.

   You should switch all use of CObjects to capsules.  Capsules
   have a safer and more consistent API.  For more information,
   see Include/pycapsule.h, or read the "Capsules" topic in
   the "Python/C API Reference Manual".

   Python 2.7 no longer uses CObjects itself; all objects which
   were formerly CObjects are now capsules.  Note that this change
   does not by itself break binary compatibility with extensions
   built for previous versions of Python--PyCObject_AsVoidPtr()
   has been changed to also understand capsules.

*/

/* original file header comment follows: */

/* C objects to be exported from one extension module to another.
 
   C objects are used for communication between extension modules.
   They provide a way for an extension module to export a C interface
   to other extension modules, so that extension modules can use the
   Python import mechanism to link to one another.

*/

#ifndef Py_COBJECT_H
#define Py_COBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

PyAPI_DATA(PyTypeObject) PyCObject_Type;

#define PyCObject_Check(op) (Py_TYPE(op) == &PyCObject_Type)

/* Create a PyCObject from a pointer to a C object and an optional
   destructor function.  If the second argument is non-null, then it
   will be called with the first argument if and when the PyCObject is
   destroyed.

*/
PyAPI_FUNC(PyObject *) PyCObject_FromVoidPtr(
	void *cobj, void (*destruct)(void*));


/* Create a PyCObject from a pointer to a C object, a description object,
   and an optional destructor function.  If the third argument is non-null,
   then it will be called with the first and second arguments if and when 
   the PyCObject is destroyed.
*/
PyAPI_FUNC(PyObject *) PyCObject_FromVoidPtrAndDesc(
	void *cobj, void *desc, void (*destruct)(void*,void*));

/* Retrieve a pointer to a C object from a PyCObject. */
PyAPI_FUNC(void *) PyCObject_AsVoidPtr(PyObject *);

/* Retrieve a pointer to a description object from a PyCObject. */
PyAPI_FUNC(void *) PyCObject_GetDesc(PyObject *);

/* Import a pointer to a C object from a module using a PyCObject. */
PyAPI_FUNC(void *) PyCObject_Import(char *module_name, char *cobject_name);

/* Modify a C object. Fails (==0) if object has a destructor. */
PyAPI_FUNC(int) PyCObject_SetVoidPtr(PyObject *self, void *cobj);


typedef struct {
    PyObject_HEAD
    void *cobject;
    void *desc;
    void (*destructor)(void *);
} PyCObject;


#ifdef __cplusplus
}
#endif
#endif /* !Py_COBJECT_H */
