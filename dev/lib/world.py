import tree
from tree import Tree

from collect import *
from path import *
import enums

class Effect:
	def __init__(self, root: Tree) -> None:
		self.id = tree.u16(root, 'id')

	def output(self, dump: Dump) -> None:
		dump.u16(self.id)

class Effects:
	def __init__(self, root: Tree) -> None:
		self.data = tree.list(root, {
			'press': Effect
		})['press']

	def output(self, dump: Dump) -> None:
		dump.list(self.data)

class CubePath:
	def __init__(self, root: Tree) -> None:
		self.path = Path(root)

	def output(self, dump: Dump) -> None:
		dump.list(self.path)

class Route:
	def __init__(self, root: Tree) -> None:
		self.pos = tree.vec3(root, 'pos')
		self.mini = tree.boolean(root, 'mini')
		self.grad = tree.enum(root, 'theme', enums.theme)
		self.file = tree.string(root, 'file')
		data = tree.list(root, {
			'path': CubePath,
			'effects': Effects
		})
		self.path = data['path']
		self.effects = data['effects']

		if len(self.path) != 1:
			tree.error(f'Route must contain 1 path tag')
		if len(self.effects) != 1:
			tree.error(f'Route must contain 1 effects tag')

		self.path = self.path[0]
		self.effects = self.effects[0]

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.u8(self.mini << 2 | self.grad)
		self.path.output(dump)
		self.effects.output(dump)
		dump.str(self.file)

class DarkPrism:
	def __init__(self, root: Tree) -> None:
		self.id = tree.u16(root, 'id')

	def output(self, dump: Dump) -> None:
		dump.u16(self.id)

class WorldData:
	def __init__(self, root: Tree) -> None:
		if root.tag.lower() != 'world':
			tree.error(f'Root tag must be "world" (got {root.tag})')

		data = tree.list(root, {
			'route': Route,
			'darkprism': DarkPrism
		})
		self.routes = data['route']
		self.prisms = data['darkprism']

	def output(self, dump: Dump) -> None:
		dump.list(self.routes)
		dump.list(self.prisms)