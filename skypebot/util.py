
import threading

class _RepeatingFuncThread(threading.Thread):
	def __init__(self, delay, func, *args, **kwargs):
		threading.Thread.__init__(self)
		self.delay = delay
		self.func = func
		self.finished = threading.Event()
		self.args = args
		self.kwargs = kwargs

	def run(self):
		while not self.finished.wait(self.delay):
			self.func(*self.args, **self.kwargs)

class RepeatingFunc(object):
	def __init__(self, delay, func, *args, **kwargs):
		self.thread = None
		self.factory = lambda: _RepeatingFuncThread(delay, func, *args, **kwargs)

	def start(self):
		if self.thread is not None:
			raise Exception("RepeatingFunc: start called while already running")
		self.thread = self.factory()
		self.thread.start()

	def stop(self):
		self.thread.finished.set()
		self.thread = None
