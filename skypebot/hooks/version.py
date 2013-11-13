
@chathook("(what (is your )?)?version( number)?( are you)?")
def version(bot, user, msg):
	msg.Chat.SendMessage("I am running skypebot %s and chatterbot %s" % (bot.version, bot.gh.version))
	return True
