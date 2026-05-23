extends Node

func _ready() -> void:
	var a = BigInt.create(100_000_000_000)
	var b = BigInt.create(100_000_000_000)
	var c = BigInt.multiply(a, b)
	var d = BigInt.multiply(c, c)
	print(c.to_string())
	print(INT64_MAX)
	print(d.to_string())
	print(BigInt.plus(d, BigInt.create(1)).to_string())
