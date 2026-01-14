from PIL import Image
import xml.etree.ElementTree as Tree
import sys

from world import *

# get files root
if len(sys.argv) == 1:
	file = input("File: ")
else:
	file = sys.argv[1]
	print("File:", file)
root = ".".join(file.split('.')[:-1])

# read route file
try:
	xml = Tree.parse(file).getroot()
except Tree.ParseError as exp:
	print(exp)
	raise SystemExit
except FileNotFoundError:
	print("File not found.")
	raise SystemExit

# read route data
try:
	data = WorldData(xml)
except tree.Exit as exp:
	raise SystemExit

# save level data
dump = Dump()
data.output(dump)
with open(root + '.dat', 'wb+') as file:
	file.write(dump.arr)