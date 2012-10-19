#!/usr/bin/python
# vim: set fileencoding=utf-8

#   This file is part of "kdpee", kd-partitioning entropy estimator.
#   (c) 2009 Dan Stowell and Queen Mary University of London
#   All rights reserved.
#
#   kdpee is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   kdpee is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with kdpee.  If not, see <http://www.gnu.org/licenses/>.

# The command to build, and the command to build+install:
'''
python setup.py build
python setup.py install
# or for debug:
sudo python setup.py build --debug && sudo python setup.py install
'''

from distutils.core import setup, Extension

kdpee_mod = Extension('kdpee',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '1')],
                    include_dirs = ['/opt/local/lib/python2.5/site-packages/numpy/core/include'],
#                    libraries = ['tcl83'],
#                    library_dirs = ['/usr/local/lib'],
                    sources = ['kdpee_py.c', '../src/kdpee.c'])

setup (name = 'kdpee',
       version = '1.1',
       description = "Estimate the entropy of a multidimensional data distribution, using 'kdpee' the k-dimensional partitioning estimator",
       author = 'Dan Stowell',
       author_email = 'dan.stowell@elec.qmul.ac.uk',
       url = 'http://www.elec.qmul.ac.uk/digitalmusic/downloads/#kdpee',
       long_description = '''
Estimate the entropy of a multidimensional data distribution, using 'kdpee' the k-dimensional partitioning estimator.
Supply an array where each ROW is a dimension and each COLUMN is a datapoint.

  # Analysing a set of 10 2D datapoints:
  kdpee(array( [[1,2,3,4,5,1,2,3,4,5], [6,7,8,9,0,6,7,8,9,0]]))

''',
       ext_modules = [kdpee_mod])
