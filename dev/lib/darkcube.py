import tree
from tree import Tree

from collect import *
from path import *

class DarkCube:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'radius': '0,0',
			'mini': 'false',
			'rainbow': 'false'
		})

		self.pos = tree.vec3(root, 'pos')
		self.rad = tree.vec2(root, 'radius')

		self.spawn = tree.vec3(root, 'spawn')
		self.mini = tree.boolean(root, 'mini')
		self.rain = tree.boolean(root, 'rainbow')
		self.path = Path(root)

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.vec2(self.rad)
		dump.vec3(self.spawn)
		dump.u8(self.mini | self.rain << 1)
		dump.list(self.path)