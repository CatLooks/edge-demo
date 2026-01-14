import xml.etree.ElementTree as Tree
import builtins
from id import *

class Exit(Exception):
	pass

def error(*args, **kwargs):
	print(end = "\033[38;5;9m\033[1m")
	print(*args, **kwargs)
	print(end = "\033[0m")
	raise Exit

def info(*args, **kwargs):
	print(end = "\033[38;5;12m\033[1m")
	print(*args, **kwargs)
	print(end = "\033[0m")

class Parser:
	@staticmethod
	def intRange(value: str, lim: range, name: str) -> int:
		try:
			i = builtins.int(value)
			if lim != None and (i not in lim):
				error(f'Integer "{i}" must be in {name} range')
			return i
		except ValueError:
			error(f'Could not parse "{value}" as an integer')

	@staticmethod
	def u8(value: str) -> int:
		return Parser.intRange(value, range(0, 0x100), "unsigned 8-bit")

	@staticmethod
	def u16(value: str) -> int:
		return Parser.intRange(value, range(0, 0x10000), "unsigned 16-bit")

	@staticmethod
	def u32(value: str) -> int:
		return Parser.intRange(value, range(0, 0x100000000), "unsigned 32-bit")

	@staticmethod
	def s8(value: str) -> int:
		return Parser.intRange(value, range(-0x80, 0x80), "signed 8-bit")

	@staticmethod
	def s16(value: str) -> int:
		return Parser.intRange(value, range(-0x8000, 0x8000), "signed 16-bit")

	@staticmethod
	def s32(value: str) -> int:
		return Parser.intRange(value, range(-0x80000000, 0x80000000), "signed 32-bit")

def string(tree: Tree, key: str) -> str:
	data = tree.get(key)
	if data == None:
		error(f'Unable to find field "{key}" in element "{tree.tag}"')
	return data

def u8(tree: Tree, key: str) -> int:
	return Parser.u8(string(tree, key))

def u16(tree: Tree, key: str) -> int:
	return Parser.u16(string(tree, key))

def u32(tree: Tree, key: str) -> int:
	return Parser.u32(string(tree, key))

def vector(tree: Tree, key: str, sep: str, parser: ..., count: int = None) -> [int]:
	values = string(tree, key).split(sep)
	if count != None and len(values) != count:
		error(f'Vector field "{key}" in element "{tree.tag}" must have {count} elements')
	return [parser(value.strip()) for value in values]

def vec3(tree: Tree, key: str) -> [int]:
	return vector(tree, key, ',', Parser.s16, 3)

def vecs(tree: Tree, key: str) -> [int]:
	return vector(tree, key, 'x', Parser.u16, 3)

def vec2(tree: Tree, key: str) -> [int]:
	return vector(tree, key, ',', Parser.u8, 2)

def enum(tree: Tree, key: str, opts: [str]) -> str:
	value = string(tree, key).lower()
	if value not in opts:
		error(f'Value "{value}" of field "{key}" in element "{tree.tag}" is not a valid option')
	return opts.index(value)

def boolean(tree: Tree, key: str) -> bool:
	return bool(enum(tree, key, ('false', 'true')))

def ticks(tree: Tree, key: str) -> int:
	value = string(tree, key).split('!')
	if len(value) == 1:
		return Parser.u16(value[0].strip())
	if len(value) != 2:
		error(f'Tick count "{value}" must contain only one "!" delimiter')
	i = Parser.u16(value[0]) * 240
	if value[1].strip() != '':
		i += Parser.u16(value[1].strip())
	if i >= 0x10000:
		error(f'Tick count "{value}" exceeded 16-bit limit (value = {i})')
	return i

def list(tree: Tree, parsers: dict) -> {}:
	result = {}
	for name, parser in parsers.items():
		result[name] = []
	for item in tree:
		if item.tag in parsers.keys():
			result[item.tag].append(parsers[item.tag](item))
		else:
			error(f'Unexpected tag "{item.tag}"')
	return result

def default(tree: Tree, settings: dict) -> None:
	for key, value in settings.items():
		if key not in tree.attrib:
			tree.attrib[key] = value