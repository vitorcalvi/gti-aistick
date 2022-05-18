#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************

from setuptools import setup


def readme():
    with open('README.rst') as f:
        return f.read()


setup(name='gtilib',
      version='4.0',
      description='Python wrapper for GTI hardware library',
      long_description=readme(),
      classifiers=[
          'Development Status :: 3 - Alpha',
          'License :: Other/Proprietary License',
          'Programming Language :: Python :: 2.7',
          'Topic :: System :: Hardware',
      ],
      keywords='gtilib',
      url='https://www.gyrfalcontech.ai',
      author='Gyrfalcon Technology Inc',
      author_email='sales@gyrfalcontech.com',
      license='Proprietary',
      packages=['gtilib'],
      install_requires=[
          'numpy',
          'functools32',
          'enum',
      ],
      package_data = { 'gtilib': ['libGTILibrary.so'] },
      include_package_data=True,
      zip_safe=False)

