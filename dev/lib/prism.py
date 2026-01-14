import tree
from tree import Tree

from collect import *

class Prism:
	def __init__(self, root: Tree) -> None:
		self.pos = tree.vec3(root, 'pos')

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)