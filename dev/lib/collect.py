import tree

class Dump:
	def __init__(self):
		self.arr = bytearray()
		self.ids = {
			'moving': {
				'insts': [],
				'names': [],
			},
			'button': {
				'insts': [],
				'names': [],
			},
			'bumper': {
				'insts': [],
				'names': [],
			},
			'lock': {
				'insts': [],
				'names': [],
			}
		}

	def u8(self, num: int) -> None:
		self.arr += num.to_bytes(1, 'little')
	def u16(self, num: int) -> None:
		self.arr += num.to_bytes(2, 'little')
	def u32(self, num: int) -> None:
		self.arr += num.to_bytes(4, 'little')

	def s8(self, num: int) -> None:
		self.u8(num if num >= 0 else num + 0x100)
	def s16(self, num: int) -> None:
		self.u16(num if num >= 0 else num + 0x10000)
	def s32(self, num: int) -> None:
		self.u32(num if num >= 0 else num + 0x100000000)

	def vec3(self, vec: [int]) -> None:
		self.s16(vec[0])
		self.s16(vec[1])
		self.s16(vec[2])

	def vecs(self, vec: [int]) -> None:
		self.u16(vec[0])
		self.u16(vec[1])
		self.u16(vec[2])

	def vec2(self, vec: [int]) -> None:
		self.u8(vec[0])
		self.u8(vec[1])

	def str(self, text: str) -> None:
		for c in text:
			self.u8(ord(c))
		self.u8(0)

	def array(self, arr: bytearray) -> None:
		self.arr += arr

	def list(self, items: ..., header: bool = True) -> None:
		if header:
			self.u16(len(items))
		for item in items:
			item.output(self)

	def reg(self, name: str, group: str) -> None:
		if name == None:
			self.ids[group]['names'].append(None)
			return
		if name in self.ids[group]['names']:
			tree.error(f'ID "{name}" in group "{group}" already exists')
		self.ids[group]['names'].append(name)

	def after(self, name: str, group: str) -> None:
		if name == None:
			return
		self.ids[group]['insts'].append({
			'idx': name,
			'pos': len(self.arr)
		})
		self.u16(0)

	def id(self, name: str, group: str) -> int:
		if name == None:
			tree.error(f'Implicit ID is not allowed')
		try:
			return self.ids[group]['names'].index(name)
		except ValueError:
			tree.error(f'Could not find ID "{name}" in group "{group}"')

	def post(self) -> None:
		for group, col in self.ids.items():
			for inst in col['insts']:
				idx = self.id(inst['idx'], group)

				tree.info(f'Replacing ID "{inst["idx"]}" in group "{group}" with value "{idx}"')

				self.arr[inst['pos'] + 0] = idx & 0xFF
				self.arr[inst['pos'] + 1] = idx >> 8