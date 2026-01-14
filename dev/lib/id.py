class IDMap:
	def __init__(self):
		self.ids = []

	def get(self, ID: str) -> int:
		try:
			return int(ID)
		except ValueError:
			pass
		try:
			return self.ids.index(ID)
		except IndexError:
			self.ids.append(ID)
			return len(self.ids) - 1

idMoving = IDMap()
idButton = IDMap()
idBumper = IDMap()
idLock = IDMap()