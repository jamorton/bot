
import Skype4Py

class SkypeChat(object):
	def __init__(self, name):

		self.skype = Skype4Py.Skype()

		self.skype.FriendlyName = name
		self.skype.Attach()

		self.create_listeners()

	def all_channels(self):
		return self.skype.Chats

	def create_listeners(self):

		def _msg(cm, mtype):
			if mtype == "RECEIVED":

				# standard message
				if cm.Type == "SAID":
					self.on_msg(cm)

				# topic change
				elif cm.Type == "SETTOPIC":
					self.on_topic(cm)

				# emote
				elif cm.Type == "EMOTED":
					self.on_emote(cm)

		self.skype.OnMessageStatus = _msg

		def _attach(status):
			if status == Skype4Py.apiAttachAvailable:
				self.skype.Attach()

		self.skype.OnAttachmentStatus = _attach

	def on_msg(self, cm):
		pass

	def on_topic(self, cm):
		pass

	def on_emote(self, cm):
		pass
