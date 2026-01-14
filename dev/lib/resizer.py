import tree
from tree import Tree

from collect import *

class Resizer:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'radius': '0,0'
		})
		
		self.mode = root.tag.lower() == 'grow'
		self.pos = tree.vec3(root, 'pos')
		self.rad = tree.vec2(root, 'radius')
		self.vis = tree.boolean(root, 'visible')

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.vec2(self.rad)
		dump.u8(self.mode | self.vis << 1)