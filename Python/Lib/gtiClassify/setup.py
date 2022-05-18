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


setup(name='gtiClassify',
      version='3.1',
      description='Python wrapper for gtiClassify library',
      long_description=readme(),
      classifiers=[
          'Development Status :: 3 - Alpha',
          'License :: Other/Proprietary License',
          'Programming Language :: Python :: 2.7',
          'Topic :: System :: Hardware',
      ],
      keywords='gtiClassify',
      url='https://www.gyrfalcontech.ai',
      author='Gyrfalcon Technology Inc',
      author_email='sales@gyrfalcontech.com',
      license='Proprietary',
      packages=['gtiClassify'],
      install_requires=[
          'functools32',
      ],
      package_data = { 'gtiClassify': ['libGtiClassify.so'] },
      include_package_data=True,
      zip_safe=False)

