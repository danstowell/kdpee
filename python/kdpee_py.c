/**
	Code to expose the "kdpee" entropy estimation C code to python (NumPy)

    This file is part of "kdpee", kd-partitioning entropy estimator.
    (c) 2009 Dan Stowell and Queen Mary University of London
    All rights reserved.

    kdpee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    kdpee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with kdpee.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Python.h"
#include "numpy/arrayobject.h"   // TODO can this path be specified better?
#include "../kdpee/kdpee.h"

////////////////////////////////////////////////////////////////////////////////
// Declarations
static PyObject * kdpee_pycall(PyObject *self, PyObject *args);

static PyMethodDef mymethods[] = {
	{"kdpee", kdpee_pycall, METH_VARARGS, 
			// TODO is METH_VARARGS correct? 
		"Estimate the entropy of a multidimensional data distribution, using 'kdpee' the k-dimensional partitioning estimator.\nSupply an array where each ROW is a dimension and each COLUMN is a datapoint.\n\n  # Analysing a set of 10 2D datapoints:\n  from numpy import *\n  from kdpee import *\n  kdpee(array( [[1,2,3,4,5,1,2,3,4,5], [6,7,8,9,0,6,7,8,9,0]]))"},
	{NULL, NULL, 0, NULL} /* end marker */
};

static struct PyModuleDef cModKdpee =
{
    PyModuleDef_HEAD_INIT,
    "kdpee", /* name of module */
    "",          /* module documentation, may be NULL */
    0,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    mymethods
};


////////////////////////////////////////////////////////////////////////////////
// Methods, including the init method

PyMODINIT_FUNC PyInit_kdpee(void){
	// extension must call import_array() in its initialization 
	// function, after the call to Py_InitModule().
    PyObject *m = PyModule_Create(&cModKdpee);
	import_array();
    return m;
}

static PyObject * kdpee_pycall(PyObject *self /* <- unused */, PyObject *args) 
{
	PyObject *input;
	PyArrayObject *array;
	int n, numdims;
	
	if (!PyArg_ParseTuple(args, "O", &input)) 
		return NULL;

	// Ensure we have contiguous, 2D, floating-point data:
	array = (PyArrayObject*) PyArray_ContiguousFromObject(input, 
#ifdef KDPEE_PREC_SINGLE
		PyArray_FLOAT
#else
		PyArray_DOUBLE
#endif
		, 2, 2); // force 2D

/*
// Not sure, but I think this way of doing it may be too new for my version of numpy
	array = (PyArrayObject*) PyArray_FROM_OTF(input, 
#ifdef KDPEE_PREC_SINGLE
		NPY_CFLOAT
#else
		NPY_DOUBLE
#endif
		, NPY_IN_ARRAY);
*/
	if(array==NULL){
		printf("kdpee_py: nullness!\n");
		return NULL;
	}
	// From this point on: remember to do Py_DECREF(array) if terminating early
	n       = array->dimensions[1];
	numdims = array->dimensions[0];
	
	floatval *data = (floatval *)array->data;
	
	////////////////////////////////////////////////////////////////////////////
	// OK, NOW we can prepare stuff for calling the main kdpee() function.
	// See header file of kdpee() for explanation of these:
	int dimi, datumi;
	const floatval** dimrefs = (const floatval**) malloc(numdims * sizeof(floatval*));
	floatval*        mins    = (      floatval* ) malloc(numdims * sizeof(floatval ));
	floatval*        maxs    = (      floatval* ) malloc(numdims * sizeof(floatval ));
	int*             keys    = (int     * ) malloc(n       * sizeof(int      ));
	floatval         zcut    = 1.96; // maybe at some point make this a vararg

	for(dimi=0; dimi<numdims; ++dimi){
		dimrefs[dimi] = data + (dimi * n);
		mins   [dimi] = INFINITY;
		maxs   [dimi] = -INFINITY;
	}
	for(datumi=0; datumi<n; ++datumi){
		keys[datumi] = datumi; // NOT NEEDED? CHECK what happens in the kdpee code
		for(dimi=0; dimi<numdims; ++dimi){
			if(mins[dimi] > dimrefs[dimi][datumi])
				mins[dimi] = dimrefs[dimi][datumi];
			if(maxs[dimi] < dimrefs[dimi][datumi])
				maxs[dimi] = dimrefs[dimi][datumi];
		}
	}
	
	// call kdpee
	floatval result = kdpee(dimrefs, n, numdims, mins, maxs, zcut, keys);
	
	// Tidying up:
	free(dimrefs);
	free(mins);
	free(maxs);
	free(keys);
	Py_DECREF(array); // destroy the contig array
#ifdef KDPEE_PREC_SINGLE
	return PyFloat_FromFloat((float)result);
#else
	return PyFloat_FromDouble((double)result);
#endif
}
