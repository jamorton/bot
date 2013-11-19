
import random

answers = [
	"Yes",
	"No",
	"Absolutely not",
	"Absolutely",
	"Of course",
	"I think so",
	"Why not",
	"Do it",
	"Not a good idea",
	"Good idea",
	"Terrible idea",
	"Nope",
	"Yep",
	"There will be dire consequences",
	"Yes, but not too much",
	"In moderation",
	"That's a sin",
	"No way",
	"I would",
	"Only if you believe in yourself",
	"Yes ma'am",
	"No sir",
	"I think no",
	"I wouldn't",
	"IT'S A TRAP"
]

because = [
	"Because you touch yourself at night",
	"Becuase I said so",
	"Because that's how it is",
	"I don't know",
	"Who knows",
	"What a stupid question..",
	"Because god made the world that way",
	"Because the devil is an asshole",
	"Because",
	"The world may never know",
	"Because science",
	"Because religion",
	"Because Rick santorum",
	"Don't ask",
	"Pray to jesus for answers",
]

endings = [
	"", "", "",
	".", ".", ".", ".", ".", ".",
	"!",
	"...",
	" =]",
	". =|",
	"!!!",
	" ;)"
]

@chathook(r"should .+")
def should(bot, user, msg):
	msg.Chat.SendMessage(random.choice(answers) + random.choice(endings))
	return True

@chathook(r"why .+")
def why(bot, user, msg):
	if random.random() > 0.7:
		msg.Chat.SendMessage(random.choice(because) + random.choice(endings))
		return True
