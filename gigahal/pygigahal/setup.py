
from setuptools import setup

setup(
	name = 'PyGigahal',
	packages = ['gigahal'],
	package_data = {'gigahal': ['libgigahal.so']},
	version = '0.1',
	zip_safe = False
)
