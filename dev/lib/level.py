import tree
from tree import Tree

from collect import *
import bits
import enums

from moving import *
from falling import *
from bumper import *
from button import *
from prism import *
from resizer import *
from checkpoint import *
from camera import *
from teleport import *
from darkcube import *

def colorToBlock(r: int, g: int, b: int) -> int:
	t = (b >= 85) + (b >= 85 and b < 170)
	s = (r < 128) << 1 | (g < 128)
	return s << 2 | t

class Level:
	def __init__(self, root: Tree) -> None:
		if root.tag.lower() != 'level':
			tree.error(f'Root tag must be "level" (got {root.tag})')

		self.ID    = tree.u16(root, 'id')
		self.name  = tree.string(root, 'name')
		self.size  = tree.vecs(root, 'size')
		self.spawn = tree.vec3(root, 'spawn')
		self.exit  = tree.vec3(root, 'exit')
		self.zoom  = tree.enum(root, 'zoom', enums.zoom) + 1
		self.grad  = tree.enum(root, 'theme', enums.theme)
		self.music = tree.enum(root, 'music', enums.music)

		self.terrain = bits.Bits()

		self.thresholds = tree.vector(root, 'time', ',', tree.Parser.u16, 4)
		self.data = tree.list(root, {
			'moving': Moving,
			'falling': Falling,
			'bumper': Bumper,
			'button': Button,
			'prism': Prism,
			'shrink': Resizer,
			'grow': Resizer,
			'checkpoint': Checkpoint,
			'cameratrigger': CameraTrigger,
			'cameralock': CameraLock,
			'teleporter': Teleporter,
			'darkcube': DarkCube
		})

	def output(self, dump: Dump) -> None:
		dump.movs = self.data['moving']

		dump.u16(self.ID)
		dump.str(self.name)
		dump.u16(self.thresholds[0])
		dump.u16(self.thresholds[1])
		dump.u16(self.thresholds[2])
		dump.u16(self.thresholds[3])
		dump.u16(len(self.data['prism']))
		dump.vecs(self.size)
		dump.vec3(self.spawn)
		dump.vec3(self.exit)
		dump.u8(self.zoom)
		dump.u8(self.grad)
		dump.u8(self.music)

		# terrain
		"""i = 0
		for z in range(self.size[2]):
			for y in range(self.size[1]):
				for x in range(self.size[0]):
					dump.u8(self.terrain[i])
					i += 1"""
		dump.array(self.terrain.get())

		dump.list(self.data['moving'])
		dump.list(self.data['falling'])
		dump.list(self.data['bumper'])
		dump.list(self.data['shrink'] + self.data['grow'])
		dump.list(self.data['prism'], False)
		dump.list(self.data['checkpoint'])
		dump.list(self.data['button'])
		dump.list(self.data['cameratrigger'])
		dump.list(self.data['cameralock'])
		dump.list(self.data['teleporter'])
		dump.list(self.data['darkcube'])