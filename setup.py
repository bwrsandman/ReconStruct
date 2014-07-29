#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys


try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

if sys.argv[-1] == 'publish':
    os.system('python setup.py sdist upload')
    sys.exit()

readme = open('README.md').read()
history = open('HISTORY.rst').read().replace('.. :changelog:', '')
requirements = open('requirements.txt').read().strip().split('\n')
requirements = [i for i in requirements if not i.startswith('#')]

setup(
    name='ReconStruct',
    version='0.1.0',
    description='Tool to help reverse engineer binary file formats, such as game files.',
    long_description=readme + '\n\n' + history,
    author='Sandy Carter',
    author_email='bwrsandman@gmail.com',
    url='https://github.com/bwrsandman/ReconStruct',
    packages=[
        'ReconStruct',
    ],
    package_dir={'ReconStruct': 'ReconStruct'},
    include_package_data=True,
    install_requires=requirements,
    license="BSD",
    zip_safe=False,
    keywords='ReconStruct',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Natural Language :: English',
        "Programming Language :: Python :: 2",
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
    ],
    test_suite='tests',
)
