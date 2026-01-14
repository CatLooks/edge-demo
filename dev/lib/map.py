from PIL import Image
import xml.etree.ElementTree as Tree
import sys

from level import *
import bits

# get files root
if len(sys.argv) == 1:
	file = input("File: ")
else:
	file = sys.argv[1]
	print("File:", file)
root = ".".join(file.split('.')[:-1])

# read level core file
try:
	xml = Tree.parse(file).getroot()
except Tree.ParseError as exp:
	print(exp)
	raise SystemExit
except FileNotFoundError:
	print("File not found.")
	raise SystemExit

# read level data
try:
	level = Level(xml)

	# read terrain data
	for z in range(level.size[2]):
		# read layer image
		try:
			img = Image.open(f'{root}{z + 1}.png')
		except FileNotFoundError:
			tree.info(f'Skipping layer #{z + 1}')
			for y in range(level.size[1]):
				for x in range(level.size[0]):
					level.terrain.add([0])
			continue
		if img.width != level.size[0] or img.height != level.size[1]:
			tree.error(f'Layer #{z + 1} has invalid size')
		for y in range(img.height):
			for x in range(img.width):
				color = img.getpixel((x, y))
				block = colorToBlock(color[0], color[1], color[2])
				if block & 3 == 0:
					level.terrain.add([0])
				else:
					level.terrain.add([1, block & 3 == 2])
					level.terrain.push(block >> 2, 2)

except tree.Exit as exp:
	raise SystemExit

# save level data
dump = Dump()
level.output(dump)
try:
	dump.post()
except tree.Exit as exp:
	raise SystemExit
with open(root + '.dat', 'wb+') as file:
	file.write(dump.arr)