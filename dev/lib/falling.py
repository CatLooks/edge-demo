import tree
from tree import Tree

from collect import *
import enums

class Falling:
	def __init__(self, root: Tree) -> None:
		self.pos   = tree.vec3(root, 'pos')
		self.decay = tree.ticks(root, 'float')
		self.theme = tree.enum(root, 'theme', enums.theme)

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.u16(self.decay)
		dump.u8(self.theme)