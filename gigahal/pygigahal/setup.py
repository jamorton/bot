
from setuptools import setup

setup(
	name = 'PyGigahal',
	packages = ['gigahal'],
	package_data = {'gigahal': ['libgigahal.so']},
	version = '0.2',
	zip_safe = False
)
