
from ctypes import *

import os
import pkg_resources

_lib = CDLL(pkg_resources.resource_filename(__name__, "libgigahal.so"))

_lib.gh_new_brain_default.restype = c_void_p
_lib.gh_new_brain_default.argtypes = []
_lib.gh_input_no_reply.restype = None
_lib.gh_input_no_reply.argtypes = [c_void_p, c_char_p]
_lib.gh_free.restype = None
_lib.gh_free.argtypes = [c_void_p]
_lib.gh_input_with_reply.restype = c_void_p
_lib.gh_input_with_reply.argtypes = [c_void_p, c_char_p]
_lib.gh_save.restype = None
_lib.gh_save.argtypes = [c_void_p, c_char_p]
_lib.gh_load.restype = c_void_p
_lib.gh_load.argtypes = [c_char_p]

class Gigahal(object):
	def __init__(self, filename):
		self.filename = filename
		self.gh_ptr = None
		if os.path.isfile(filename):
			self.gh_ptr = _lib.gh_load(filename)
		if self.gh_ptr is None:
			self.gh_ptr = _lib.gh_new_brain_default()

	def _input_str(self, inp):
		if isinstance(inp, unicode):
			inp = inp.encode('utf-8')
		return c_char_p(inp)

	def input_no_reply(self, inp):
		_lib.gh_input_no_reply(self.gh_ptr, self._input_str(inp))

	def input_with_reply(self, inp):
		retptr = _lib.gh_input_with_reply(self.gh_ptr, self._input_str(inp))
		ret = c_char_p(retptr).value
		_lib.gh_free(retptr)
		return ret

	def save(self):
		_lib.gh_save(self.gh_ptr, self.filename)

if __name__ == '__main__':
	gh = Gigahal('brain.gh')

	while True:
		inp = raw_input('>')
		if inp == '!q':
			gh.save()
			break
		print gh.input_with_reply(inp)
