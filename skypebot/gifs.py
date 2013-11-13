
import shelve
import random

# remove '
all_gifs = {
	"almost_gaf.gif": ["idgaf", "give a fuck", "gave a fuck", "gives a fuck", "no fucks given", "give fuck"],
	"anonymom.gif": ["anonymous"],
	"anonymom2.gif": ["anonymous"],
	"awkward_boner.gif": ["awkward boner", "boner"],
	"babyspin.gif": ["baby", "spin"],
	"baked.gif": ["baked"],
	"beast_eating.gif": ["taco", "food", "hungry"],
	"bobby_backfire.gif": ["backfire"],
	"booty_had_me_like.gif": ["booty", "yum", "yes please", "drool"],
	"boring.gif": ["boring", "bored"],
	"broke.gif": ["broke", "money"],
	"brushin.gif": ["brush", "masturbat", "jacking"],
	"butt_slide.gif": ["baseball", "butt", "slide"],
	"can_fap.gif": ["fap"],
	"chapmagne.gif": ["champagne", "starcraft", "esports"],
	"chill_out_scooby.gif": ["scooby", "chill", "marijuana"],
	"cocaine.gif": ["cocaine"],
	"come_at_me_bro.gif": ["come at me bro"],
	"computer_freeze.gif": ["computers"],
	"concerned_dissappointed.gif": ["concerned", "dissappointed"],
	"crash.gif": ["football", "crash"],
	"creepy_look.gif": ["creepy", "stalk"],
	"condoms.gif": ["sex", "condom"],
	"daff_jackoff.gif": ["jackoff", "masturbate", "wack off", "hitler off"],
	"day9_apollo_chobra.gif": ["beat", "day9", "starcraft"],
	"day9_djwheat_dance.gif": ["day9", "dance", "swag"],
	"day9_djwheat_earthquake.gif": ["earthquake", "earth quake", "day9"],
	"dancing_goose.gif": ["dancing", "dancers", "dance"],
	"didnt_read_lol_2.gif": ["tldr", "tl;dr", "too long", "didnt read"],
	"didnt_read_lol.gif": ["tldr", "tl;dr", "too long", "didnt read"],
	"dis_gon_be_good.gif": ["gonna be good", "cant wait", "watch"],
	"do_it.gif": ["do it"],
	"do_not_want_woman.gif": ["do not want"],
	"double_bucket.gif": ["bucket", "buckethead"],
	"ew_nevermind.gif": ["gross", "eww", "nevermind"],
	"fingerjob.gif": ["blowjob", "fingerjob", "darth vader", "blow job"],
	"food.gif": ["food"],
	"gag.gif": ["gag", "gross", "disgusting", "nasty"],
	"group_facepalm.gif": ["facepalm"],
	"gtfo.gif": ["gtfo", "get the fuck out", "bye"],
	"guzzle_softserve.gif": ["icecream", "ice cream", "guzzle", "yum"],
	"haha_didnt_read.gif": ["tldr", "tl;dr", "didnt read", "too long"],
	"halleluja.gif": ["halleluja", "praise", "lord", "black people"],
	"haters_gonna_hate_train.gif": ["haters gonna hate", "dont hate", "haters"],
	"high.gif": ["eminem", "high", "weed"],
	"holy_shit.gif": ["holy shit", "seriously"],
	"homer_disappear.gif": ["disappear", "embarras", "oops"],
	"id_tap_that.gif": ["tap that"],
	"internet_taco.gif": ["hungry", "internet", "taco"],
	"interested_look_guy.gif": ["interesting"],
	"japanese_bedtime.gif": ["sleep", "bed"],
	"jesse_dance.gif": ["jesse", "dance"],
	"karma_points_please.gif": ["karma"],
	"kill_me.gif": ["kill me", "kill your"],
	"laundry.gif": ["laundry"],
	"llama_stare.gif": ["huh?", "wtf"],
	"look_then_laugh.gif": ["rofl", "hilarious", "laugh", "hahaha", "haha"],
	"LOST.gif": ["lost"],
	"lunch_time.gif": ["lunch", "pizza"],
	"magic.gif": ["magic"],
	"middle_fingers.gif": ["fuck you", "fuck them", "fuck it", "stfu", "shut up"],
	"mind_blown.gif": ["mind", "blow"],
	"naniwa_eye_micro.gif": ["naniwa", "boob", "eye"],
	"nelson.gif": ["jordy", "nelson"],
	"nipples.gif": ["nipple"],
	"no_way.gif": ["no way", "not happening", "no thanks"],
	"no.gif": ["boo", "no way", "thumbs down"],
	"nononono.gif": ["fuck that", "absolutely not", "nope"],
	"obnoxious_girl.gif": ["obnoxious"],
	"oh_lawd.gif": ["lawd"],
	"oh_snap.gif": ["oh snap", "holy shit"],
	"omg_screams.gif": ["scary", "monster"],
	"omg_scream2.gif": ["scary", "monster"],
	"OMG_WHAT.gif": ["wtf", "no way", "seriously"],
	"ooh_look_at_that.gif": ["not bad", "good"],
	"ooooooh.gif": ["ooh", "oooh", "ooooh"],
	"open_fuck_off.gif": ["fuck off", "go away", "shut up"],
	"picard_omg.gif": ["wtf"],
	"reblog.gif": ["tumblr", "blog"],
	"rps_wtf.gif": ["rock", "paper", "scissors", "jack", "bee"],
	"sad_rain.gif": ["sad", "rain", "forever alone"],
	"sad.gif": ["sad", "forever alone"],
	"santorum.gif": ["santorum"],
	"scared_and_horny.gif": ["scare", "scary", "horny"],
	"scared_kid.gif": ["scary"],
	"school.gif": ["school"],
	"sims_high.gif": ["marijuana", "weed", "high"],
	"slow_jerk.gif": ["bitch", "jerk", "cum"],
	"skateboarding.gif": ["skate"],
	"so_much_anger.gif": ["angry", "mad", "tantrum"],
	"sorry_cry.gif": ["sorry", "cry"],
	"speechless_wtf.gif": ["wtf"],
	"speechless.gif": ["shit", "oops"],
	"smash.gif": ["smash"],
	"starcraft_win.gif": ["starcraft"],
	"steve_jobs.gif": ["steve jobs"],
	"sup.gif": ["sup"],
	"super_ref.gif": ["ref"],
	"suprise_cat.gif": ["suprise"],
	"swag.gif": ["swag"],
	"thats_a_penis.gif": ["penis"],
	"unsee.gif": ["unsee"],
	"vietdance.gif": ["dance", "vietnam"],
	"what_me.gif": ["win", "who me", "me?"],
	"who_gives_a_shit.gif": ["who cares", "gives a shit", "give a shit", "gave a shit"],
	"wink.gif": ["wink"],
	"wtf_face.gif": ["wtf"],
	"wtf_look.gif": ["huh", "wtf", "oh shit"],
	"wtf_pokes.gif": ["poke"],
	"yay_amir.gif": ["yay"]
}

usage_db = shelve.open("gifs")

def message(msg, chat, user):
	msg = msg.lower()

	found = []

	for k, v in all_gifs.iteritems():
		for tag in v:
			if msg.find(tag) != -1:
				found.append((k, tag))

	if len(found) == 0:
		return False

	choice = []
	least_used = 99999999999
	for f in found:
		usage = usage_db.get(f[0], 0)
		if usage == least_used:
			choice.append(f)
		elif usage < least_used:
			least_used = usage
			choice = [f]

	choice = random.choice(choice)
	choice, tag = choice

	usage = usage_db.get(choice, 0)
	chance = (0.75 / ((usage / 1.7) + 1))
	if random.random() < chance:
		url = "http://jonanin.com/gifs/" + choice
		chat.SendMessage("%s:  %s" % (user, url))
		usage_db[choice] = usage + 1
		return True
	return False
