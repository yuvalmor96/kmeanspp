from platform import version
from setuptools import setup, find_packages, Extension
module = Extension('mykmeanssp',  sources = ['kmeans.c'])
setup(
    name='mykmeanssp',
    version='0.1.0',
    description='setup for kmeans',
    ext_modules = [module]
)
    
