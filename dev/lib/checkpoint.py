import tree
from tree import Tree

from collect import *
import enums

class Checkpoint:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'radius': '0,0',
			'reset': 'false'
		})

		self.pos = tree.vec3(root, 'pos')
		self.rad = tree.vec2(root, 'radius')
		self.rsp = tree.vec3(root, 'respawn')
		self.rst = tree.boolean(root, 'reset')

		self.anim = 'theme' in root.attrib
		if self.anim:
			self.grad = tree.enum(root, 'theme', enums.theme)
			self.time = tree.ticks(root, 'time')

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.vec2(self.rad)
		dump.vec3(self.rsp)
		dump.u8(self.rst)
		if self.anim:
			dump.u16(self.time)
			dump.u8(self.grad)
		else:
			dump.u16(0)