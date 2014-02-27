
import sys
sys.dont_write_bytecode = True

import random
import re
import string
import signal
import threading
import os
import traceback
from gigahal import Gigahal
from skype import SkypeChat
from glob import glob
from os import path

import config
import util

class Hook(object):
	def __init__(self, pattern, func):
		self.pattern = pattern
		self.func    = func
		self.name    = func.__name__

class Bot(SkypeChat):
	def __init__(self):
		super(Bot, self).__init__(config.botname)
		self.version = "2.0a"
		self.hooks = []
		self.create_hooks()
		self.gh = Gigahal("brain.gh")
		self.re_addr = re.compile("^\s*(" + "|".join(config.names).lower() + ")\s*[:, ]?\s*", re.I)
		self.gh_log = open("log/gh_input.txt", "a")
		self.seen_names = {}
		self.save_lock = threading.Lock()

		def save_all():
			self.save_lock.acquire()
			self.gh_log.flush()
			os.fsync(self.gh_log)
			self.gh.save()
			self.save_lock.release()

		self.save_thread = util.RepeatingFunc(60 * 30, save_all)
		self.save_thread.start()

	def exit(self):
		self.save_lock.acquire()
		self.save_thread.stop()
		self.gh.save()
		self.gh_log.close()
		self.save_lock.release()

	#-----------------------------
	# HOOK SYSTEM
	#-----------------------------
	def create_hooks(self):

		re_vars = [
			("addrs", "(((hey|yo|lol|derp) )?((guys|dudes) )?\s?[,:]?\s?)"),
		]

		class makehook(object):
			def __init__(me, pattern):
				me.pattern = pattern

			def __call__(me, func):
				pstr = me.pattern
				for var in re_vars:
					pstr = pstr.replace("${" + var[0] + "}", var[1])
				pattern = re.compile("^" + pstr + "$", re.I)
				hk = Hook(pattern, func)
				self.hooks.append(hk)

		import __builtin__
		__builtin__.chathook = makehook

		globpath = path.join(path.dirname(__file__), "hooks", "*.py")
		for fn in glob(globpath):
			mod = path.basename(fn)[:-3]
			if mod != "__init__" and not mod.startswith("_"):
				__import__("hooks." + mod)

		del __builtin__.chathook

	#-------------------------------
	# MAIN MESSAGE HANDLER
	#-------------------------------
	trim_end = re.compile("[\s" + re.escape(string.punctuation) + "]+$")
	detect_timestamp = re.compile("^\s*\[(\d+[/:]\d+[/:]\d+ )?\d+:\d+:\d+ (AM|PM|am|pm)\]", re.I)

	def on_msg(self, cm):
		self.save_lock.acquire()
		try:
			self.on_msg_real(cm)
		except:
			print traceback.format_exc()
		self.save_lock.release()

	def on_msg_real(self, cm):

		if cm.Chat.Name not in self.seen_names:
			self.seen_names[cm.Chat.Name] = set()
		self.seen_names[cm.Chat.Name].add(cm.FromDisplayName)

		if cm.Chat.Name in config.ignore:
			return

		if config.debug and cm.Chat.Name != config.debug_chat:
			return

		if cm.FromDisplayName == self.skype.CurrentUser.DisplayName:
			return

		if self.detect_timestamp.match(cm.Body) is not None:
			return

		was_addressed = self.re_addr.match(cm.Body) is not None

		cleaned = cm.Body.strip()
		cleaned = re.sub("\s+", " ", cleaned)
		cleaned = self.re_addr.sub("", cleaned)

		matchable = self.trim_end.sub("", cleaned)

		for hk in self.hooks:
			m = hk.pattern.match(matchable)
			if m is not None:
				cm.Match = m
				if hk.func(self, cm.Sender, cm):
					return

		if "http" not in cleaned and len(cleaned) < 200:
			self.gh_log.write(cleaned.replace("\1", "") + "\1\n")
			if was_addressed or random.random() < config.gh_chance:
				name = random.choice(list(self.seen_names[cm.Chat.Name])).strip().split(" ")[0]
				ghinp = re.sub("walrus", name, cleaned, 0, re.I)
				cm.Chat.SendMessage(self.gh.input_with_reply(ghinp))
				return
			else:
				self.gh.input_no_reply(cleaned)

if __name__ == "__main__":

	def signal_handler(signal, frame):
		print "Don't use Ctrl+C to exit! (use q)"
	signal.signal(signal.SIGINT, signal_handler)

	bot = Bot()

	while True:

		inp = ""
		try:
			inp = raw_input(">")
		except EOFError:
			pass

		if inp == "c":
			for x in bot.all_channels():
				print x.Name

		elif inp == "q":
			bot.exit()
			break

		elif len(inp) > 2 and inp[0] == "l":
			try:
				f = open(inp[2:], "r")
				for line in f.read().split("\1\n"):
					bot.gh.input_no_reply(line)
				f.close()
			except IOError:
				print "No such file"

		elif len(inp) > 2 and inp[0] == 'f':
			try:
				f = open(inp[2:], "r")
				for line in f:
					bot.gh.input_no_reply(line.strip())
				f.close()
			except IOError:
				print "No such file"
