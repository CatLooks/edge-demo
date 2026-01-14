import tree
from tree import Tree

from collect import *

class MoveCommand:
	def __init__(self, root: Tree) -> None:
		self.id = tree.string(root, 'id')
		self.wp = tree.u16(root, 'waypoint')

	def output(self, dump: Dump) -> None:
		dump.after(self.id, 'moving')
		dump.u16(self.wp)

class ButtonCommand:
	def __init__(self, root: Tree) -> None:
		self.id = tree.string(root, 'id')
		self.press = tree.boolean(root, 'pressed')

	def output(self, dump: Dump) -> None:
		dump.after(self.id, 'button')
		dump.u8(self.press)

class BumpCommand:
	action = 'disable', 'enable', 'once'

	def __init__(self, root: Tree) -> None:
		self.id = tree.string(root, 'id')
		self.act = tree.enum(root, 'act', self.action)

	def output(self, dump: Dump) -> None:
		dump.after(self.id, 'bumper')
		dump.u8(self.act)

class Button:
	def __init__(self, root: Tree) -> None:
		tree.default(root, {
			'stayup': 'false',
			'pressed': 'false'
		})

		self.id = root.get('id', None)

		self.attached = 'moving' in root.attrib
		if self.attached:
			self.moving = tree.string(root, 'moving')
		else:
			self.pos = tree.vec3(root, 'pos')

		self.press = tree.boolean(root, 'pressed')

		self.vis = 'radius' not in root.attrib
		if not self.vis:
			self.rad = tree.vec2(root, 'radius')

		self.anim = 'lock' in root.attrib
		if self.anim:
			if root.get('lock').lower() == 'none':
				self.lock = None
			else:
				self.lock = tree.string(root, 'lock')

		self.stayup = tree.boolean(root, 'stayup')

		inner = tree.list(root, {
			'move': MoveCommand,
			'click': ButtonCommand,
			'bump': BumpCommand
		})
		self.iMove = inner['move']
		self.iPress = inner['click']
		self.iBump = inner['bump']

	def output(self, dump: Dump) -> None:
		dump.reg(self.id, 'button')
		dump.u8(self.press
			| self.vis << 1
			| self.anim << 2
			| self.attached << 3
			| self.stayup << 4)
		if self.attached:
			dump.after(self.moving, 'moving')
		else:
			dump.vec3(self.pos)
		if not self.vis:
			dump.vec2(self.rad)
		if self.anim:
			if self.lock == None:
				dump.u16(0xffff)
			else:
				dump.after(self.lock, 'lock')
		dump.list(self.iMove)
		dump.list(self.iPress)
		dump.list(self.iBump)