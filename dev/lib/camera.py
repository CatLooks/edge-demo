import tree
from tree import Tree

from collect import *
import enums

class CameraTrigger:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'radius': '0,0'
		})

		self.pos = tree.vec3(root, 'pos')
		self.rad = tree.vec2(root, 'radius')
		self.time = tree.ticks(root, 'time')

		if 'zoom' in root.attrib:
			self.zoom = tree.enum(root, 'zoom', enums.zoom) + 1
		else:
			self.zoom = 0
			self.fov = tree.u8(root, 'fov')

	def output(self, dump: Dump) -> None:
		dump.vec3(self.pos)
		dump.vec2(self.rad)
		dump.u16(self.time)
		dump.u8(self.zoom)
		if self.zoom == 0:
			dump.u8(self.fov)

class CameraLock:
	def __init__(self, root: Tree) -> None:
		self.id = root.get('id', None)
		self.eye = tree.vec3(root, 'eye')

	def output(self, dump: Dump) -> None:
		dump.reg(self.id, 'lock')
		dump.vec3(self.eye)