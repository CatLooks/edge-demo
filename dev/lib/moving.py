import tree
from tree import Tree
from copy import deepcopy

from collect import *
import enums

class Waypoint:
	action = 'halt', 'next', 'loop'

	@staticmethod
	def add3(a: [int], b: [int]) -> [int]:
		return list(map(sum, zip(a, b)))

	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'travel': '0',
			'pause': '0'
		})

		self.dest   = tree.vec3(root, 'to')
		self.travel = tree.ticks(root, 'travel')
		self.pause  = tree.ticks(root, 'pause')
		self.act    = tree.enum(root, 'act', self.action)
		self.off    = [0, 0, 0]

	def output(self, dump: Dump) -> None:
		dump.vec3(self.add3(self.dest, self.off))
		dump.u16(self.travel)
		dump.u16(self.pause)
		dump.u8(self.act)

class Moving:
	def __init__(self, root: Tree) -> None:
		self.id     = root.get('id', None)

		self.rel = 'follow' in root.attrib
		if self.rel:
			self.follow = root.get('follow', None)
			self.offset = tree.vec3(root, 'offset')
		else:
			self.start  = tree.vec3(root, 'pos')
			self.ways = tree.list(root, {'waypoint': Waypoint})['waypoint']
			self.offset = [0, 0, 0]

		self.theme  = tree.enum(root, 'theme', enums.theme)
		self.half   = tree.boolean(root, 'half')
		self.active = tree.boolean(root, 'active')

	def output(self, dump: Dump) -> None:
		dump.reg(self.id, 'moving')
		if self.rel:
			for mov in dump.movs:
				if mov.id == self.follow:
					self.start = mov.start
					self.ways = deepcopy(mov.ways)
					for way in self.ways:
						way.off = self.offset
					break
			else:
				tree.error(f'Moving platform "{self.follow}" is not defined yet')

		dump.vec3(Waypoint.add3(self.start, self.offset))
		dump.u8(self.theme
			| (not self.active) << 2
			| self.half << 3)
		dump.list(self.ways)
