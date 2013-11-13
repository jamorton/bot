
import random

@chathook("roll (from )?(?P<low>\d+) (to )?(?P<high>\d+)( ?, ?please)?")
def roll(bot, user, msg):
	low  = msg.Match.group("low")
	high = msg.Match.group("high")
	if len(low) > 15 or len(high) > 15:
		return True
	low = int(low)
	high = int(high)
	try:
		res = random.randint(low, high)
		msg.Chat.SendMessage("Your roll from %d to %d is: %d" % (low, high, res))
	except ValueError:
		pass
	return True
