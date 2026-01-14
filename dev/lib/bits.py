class Bits:
	def __init__(self):
		self.arr = []

	def add(self, bits: [bool]) -> None:
		for bit in bits:
			self.arr.append(bool(bit))

	def push(self, data: int, bits: int) -> None:
		arr = []
		for i in range(bits):
			j = bits - i - 1
			arr.append(data >> j & 1)
		self.add(arr)

	def get(self) -> bytearray:
		result = bytearray()
		for i in range(0, len(self.arr), 8):
			byte = 0
			for j in range(0, min(8, len(self.arr) - i)):
				byte |= self.arr[i + j] << (7 - j)
			result.append(byte)
		return result