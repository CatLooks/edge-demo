import tree
from tree import Tree

from collect import *
import enums

class Bumper:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'pause': '0',
			'phase': '0',
			'power': '4'
		})

		self.id     = root.get('id', None)
		self.pos    = tree.vec3(root, 'pos')
		self.dir    = tree.enum(root, 'dir', enums.direction)
		self.theme  = tree.enum(root, 'theme', enums.theme)
		self.active = tree.boolean(root, 'active')
		self.bump   = tree.ticks(root, 'bump')
		self.pause  = tree.ticks(root, 'pause')
		self.phase  = tree.ticks(root, 'phase')
		self.power  = tree.u8(root, 'power')

	def output(self, dump: Dump) -> None:
		dump.reg(self.id, 'bumper')
		dump.vec3(self.pos)
		dump.u8(self.dir | self.theme << 2 | self.active << 4)
		dump.u16(self.bump + self.pause)
		dump.u16(self.bump)
		dump.u16(self.phase)
		dump.u8(self.power)