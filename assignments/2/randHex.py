#Generates a random hex number for a user specified length.
import random
import os
import sys


filename = raw_input("Please enter a filename: ")
size = input("Please enter num digits: ")

if os.path.exists('./' + filename):
	os.remove('./' + filename)

f = open('./' + filename, 'w')

for i in xrange(0, size):
	f.write(format(int(random.random() * 16), 'X'))

f.close()

print str(size) + " digits successfully written to " + filename