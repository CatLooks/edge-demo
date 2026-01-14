import tree
from tree import Tree

from collect import *

class Teleporter:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'radius': '0,0'
		})
		
		self.pos = tree.vec3(root, 'pos')
		self.rad = tree.vec2(root, 'radius')
		self.target = tree.vec3(root, 'target')

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.vec2(self.rad)
		dump.vec3(self.target)