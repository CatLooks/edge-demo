import tree
from tree import Tree

from collect import *
import enums

class Event:
	def __init__(self, root: Tree) -> None:
		self.type = tree.enum(root, 'type', enums.event)
		self.sync = self.type == 8

		if self.sync:
			self.id = tree.string(root, 'id')
			self.wp = tree.u16(root, 'waypoint')
		else:
			self.off = tree.ticks(root, 'time')

	def output(self, dump: Dump) -> None:
		dump.u8(self.type)
		if self.sync:
			dump.after(self.id, 'moving')
			dump.u16(self.wp)
		else:
			dump.u16(self.off)

def Path(root: Tree) -> [Event]:
	return tree.list(root, {
		'event': Event
	})['event']