#!/usr/bin/env python

# generates a character mapping for the
# string to symbol list parser in syntax.c

nonword = " !\"&()*+,./:;<>=?[]\\^`{}|~"
word = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

chars = ['C_SKIP' for i in xrange(256)]

for c in nonword:
	chars[ord(c)] = 'C_NONWORD'
for c in word:
	chars[ord(c)] = 'C_WORD'

#chars[ord("'")] = chars[ord("-")] = 'C_INTERWORD'
#chars[ord("$")] = chars[ord("#")] = 'C_WORD_PREFIX'
#chars[ord("%")] = 'C_WORD_SUFFX'

print '{' + ', '.join(chars) + '}'
