extends Node

var passed := 0
var failed := 0

func assert_eq(test_name: String, result: String, expected: String) -> void:
	if result == expected:
		print("✅ " + test_name)
		passed += 1
	else:
		print("❌ " + test_name + " → got: " + result + " expected: " + expected)
		failed += 1

func assert_true(test_name: String, result: bool) -> void:
	if result:
		print("✅ " + test_name)
		passed += 1
	else:
		print("❌ " + test_name)
		failed += 1

func assert_false(test_name: String, result: bool) -> void:
	assert_true(test_name, !result)

func _ready() -> void:
	test_create()
	test_from_string()
	test_plus()
	test_minus()
	test_multiply()
	test_divide()
	test_mod()
	test_minimum_maximum()
	test_absolute()
	test_plus_int()
	test_minus_int()
	test_multiply_int()
	test_divide_int()
	test_static_comparisons()
	test_mutating_add()
	test_mutating_sub()
	test_mutating_mul()
	test_mutating_div()
	test_mutating_int()
	test_instance_comparisons()
	test_is_zero()
	test_is_negative()
	test_large_numbers()
	test_edge_cases()
	print("")
	print("─────────────────────────────")
	print("Results: %d passed, %d failed" % [passed, failed])

# ─── create ──────────────────────────────────────────────────────────────────

func test_create() -> void:
	print("\n── create ──")
	assert_eq("create(0)",        BigInt.create(0).to_string(),        "0")
	assert_eq("create(1)",        BigInt.create(1).to_string(),        "1")
	assert_eq("create(-1)",       BigInt.create(-1).to_string(),       "-1")
	assert_eq("create(1000000)",  BigInt.create(1000000).to_string(),  "1000000")
	assert_eq("create(INT64_MAX)", BigInt.create(INT64_MAX).to_string(), str(INT64_MAX))

# ─── from_string / create_from_string ────────────────────────────────────────

func test_from_string() -> void:
	print("\n── from_string ──")
	var a := BigInt.create(0)
	a.from_string("12345678901234567890")
	assert_eq("from_string large",        a.to_string(), "12345678901234567890")
	assert_eq("create_from_string",       BigInt.create_from_string("99999999999999999999").to_string(), "99999999999999999999")
	assert_eq("create_from_string neg",   BigInt.create_from_string("-42").to_string(), "-42")
	assert_eq("create_from_string zero",  BigInt.create_from_string("0").to_string(), "0")

# ─── plus ────────────────────────────────────────────────────────────────────

func test_plus() -> void:
	print("\n── plus ──")
	var a := BigInt.create(100)
	var b := BigInt.create(200)
	assert_eq("100 + 200",   BigInt.plus(a, b).to_string(), "300")
	assert_eq("0 + 0",       BigInt.plus(BigInt.create(0), BigInt.create(0)).to_string(), "0")
	assert_eq("-1 + 1",      BigInt.plus(BigInt.create(-1), BigInt.create(1)).to_string(), "0")
	assert_eq("large + 1",   BigInt.plus(BigInt.create_from_string("9999999999999999999"), BigInt.create(1)).to_string(), "10000000000000000000")

# ─── minus ───────────────────────────────────────────────────────────────────

func test_minus() -> void:
	print("\n── minus ──")
	assert_eq("300 - 200",   BigInt.minus(BigInt.create(300), BigInt.create(200)).to_string(), "100")
	assert_eq("0 - 0",       BigInt.minus(BigInt.create(0), BigInt.create(0)).to_string(), "0")
	assert_eq("1 - 2",       BigInt.minus(BigInt.create(1), BigInt.create(2)).to_string(), "-1")
	assert_eq("large - large", BigInt.minus(
		BigInt.create_from_string("10000000000000000000"),
		BigInt.create_from_string("9999999999999999999")
	).to_string(), "1")

# ─── multiply ────────────────────────────────────────────────────────────────

func test_multiply() -> void:
	print("\n── multiply ──")
	assert_eq("3 * 4",       BigInt.multiply(BigInt.create(3), BigInt.create(4)).to_string(), "12")
	assert_eq("0 * 999",     BigInt.multiply(BigInt.create(0), BigInt.create(999)).to_string(), "0")
	assert_eq("-3 * 4",      BigInt.multiply(BigInt.create(-3), BigInt.create(4)).to_string(), "-12")
	assert_eq("-3 * -4",     BigInt.multiply(BigInt.create(-3), BigInt.create(-4)).to_string(), "12")
	assert_eq("large * 2",   BigInt.multiply(
		BigInt.create_from_string("9999999999999999999"), BigInt.create(2)
	).to_string(), "19999999999999999998")

# ─── divide ──────────────────────────────────────────────────────────────────

func test_divide() -> void:
	print("\n── divide ──")
	assert_eq("10 / 2",      BigInt.divide(BigInt.create(10), BigInt.create(2)).to_string(), "5")
	assert_eq("10 / 3",      BigInt.divide(BigInt.create(10), BigInt.create(3)).to_string(), "3")  # floor
	assert_eq("0 / 5",       BigInt.divide(BigInt.create(0), BigInt.create(5)).to_string(), "0")
	assert_eq("-10 / 3",     BigInt.divide(BigInt.create(-10), BigInt.create(3)).to_string(), "-4") # floor div
	assert_eq("large / 3",   BigInt.divide(
		BigInt.create_from_string("9999999999999999999"), BigInt.create(3)
	).to_string(), "3333333333333333333")

# ─── mod ─────────────────────────────────────────────────────────────────────

func test_mod() -> void:
	print("\n── mod ──")
	assert_eq("10 % 3",      BigInt.mod(BigInt.create(10), BigInt.create(3)).to_string(), "1")
	assert_eq("10 % 2",      BigInt.mod(BigInt.create(10), BigInt.create(2)).to_string(), "0")
	assert_eq("0 % 5",       BigInt.mod(BigInt.create(0), BigInt.create(5)).to_string(), "0")
	assert_eq("7 % 7",       BigInt.mod(BigInt.create(7), BigInt.create(7)).to_string(), "0")

# ─── minimum / maximum ───────────────────────────────────────────────────────

func test_minimum_maximum() -> void:
	print("\n── minimum / maximum ──")
	var a := BigInt.create(10)
	var b := BigInt.create(20)
	assert_eq("min(10, 20)",  BigInt.minimum(a, b).to_string(), "10")
	assert_eq("max(10, 20)",  BigInt.maximum(a, b).to_string(), "20")
	assert_eq("min(5, 5)",    BigInt.minimum(BigInt.create(5), BigInt.create(5)).to_string(), "5")
	assert_eq("min(-1, 0)",   BigInt.minimum(BigInt.create(-1), BigInt.create(0)).to_string(), "-1")
	assert_eq("max(-1, 0)",   BigInt.maximum(BigInt.create(-1), BigInt.create(0)).to_string(), "0")

# ─── absolute ────────────────────────────────────────────────────────────────

func test_absolute() -> void:
	print("\n── absolute ──")
	assert_eq("abs(-5)",     BigInt.absolute(BigInt.create(-5)).to_string(), "5")
	assert_eq("abs(5)",      BigInt.absolute(BigInt.create(5)).to_string(), "5")
	assert_eq("abs(0)",      BigInt.absolute(BigInt.create(0)).to_string(), "0")
	assert_eq("abs large",   BigInt.absolute(BigInt.create_from_string("-99999999999999999999")).to_string(), "99999999999999999999")

# ─── plus_int / minus_int / multiply_int / divide_int ───────────────────────

func test_plus_int() -> void:
	print("\n── plus_int ──")
	assert_eq("100 + 1",     BigInt.plus_int(BigInt.create(100), 1).to_string(), "101")
	assert_eq("0 + 0",       BigInt.plus_int(BigInt.create(0), 0).to_string(), "0")
	assert_eq("large + 1",   BigInt.plus_int(BigInt.create_from_string("9999999999999999999"), 1).to_string(), "10000000000000000000")

func test_minus_int() -> void:
	print("\n── minus_int ──")
	assert_eq("100 - 1",     BigInt.minus_int(BigInt.create(100), 1).to_string(), "99")
	assert_eq("0 - 1",       BigInt.minus_int(BigInt.create(0), 1).to_string(), "-1")

func test_multiply_int() -> void:
	print("\n── multiply_int ──")
	assert_eq("100 * 3",     BigInt.multiply_int(BigInt.create(100), 3).to_string(), "300")
	assert_eq("100 * 0",     BigInt.multiply_int(BigInt.create(100), 0).to_string(), "0")
	assert_eq("100 * -1",    BigInt.multiply_int(BigInt.create(100), -1).to_string(), "-100")

func test_divide_int() -> void:
	print("\n── divide_int ──")
	assert_eq("10 / 2",      BigInt.divide_int(BigInt.create(10), 2).to_string(), "5")
	assert_eq("10 / 3",      BigInt.divide_int(BigInt.create(10), 3).to_string(), "3")
	assert_eq("0 / 5",       BigInt.divide_int(BigInt.create(0), 5).to_string(), "0")

# ─── Comparaisons statiques ───────────────────────────────────────────────────

func test_static_comparisons() -> void:
	print("\n── static comparisons ──")
	var a := BigInt.create(10)
	var b := BigInt.create(20)
	var c := BigInt.create(10)
	assert_true ("is_equal(10,10)",          BigInt.is_equal(a, c))
	assert_false("is_equal(10,20)",          BigInt.is_equal(a, b))
	assert_true ("is_less(10,20)",           BigInt.is_less(a, b))
	assert_false("is_less(20,10)",           BigInt.is_less(b, a))
	assert_true ("is_greater(20,10)",        BigInt.is_greater(b, a))
	assert_false("is_greater(10,20)",        BigInt.is_greater(a, b))
	assert_true ("is_less_or_equal(10,10)",  BigInt.is_less_or_equal(a, c))
	assert_true ("is_less_or_equal(10,20)",  BigInt.is_less_or_equal(a, b))
	assert_true ("is_greater_or_equal(10,10)", BigInt.is_greater_or_equal(a, c))
	assert_true ("is_greater_or_equal(20,10)", BigInt.is_greater_or_equal(b, a))

# ─── Mutantes BigInt ──────────────────────────────────────────────────────────

func test_mutating_add() -> void:
	print("\n── add (mutating) ──")
	var a := BigInt.create(100)
	a.add(BigInt.create(50))
	assert_eq("100.add(50)", a.to_string(), "150")
	a.add(BigInt.create(-50))
	assert_eq("150.add(-50)", a.to_string(), "100")

func test_mutating_sub() -> void:
	print("\n── sub (mutating) ──")
	var a := BigInt.create(100)
	a.sub(BigInt.create(30))
	assert_eq("100.sub(30)", a.to_string(), "70")

func test_mutating_mul() -> void:
	print("\n── mul (mutating) ──")
	var a := BigInt.create(10)
	a.mul(BigInt.create(5))
	assert_eq("10.mul(5)", a.to_string(), "50")

func test_mutating_div() -> void:
	print("\n── div (mutating) ──")
	var a := BigInt.create(10)
	a.div(BigInt.create(3))
	assert_eq("10.div(3)", a.to_string(), "3")

func test_mutating_int() -> void:
	print("\n── add/sub/mul/div_int (mutating) ──")
	var a := BigInt.create(100)
	a.add_int(50)
	assert_eq("100.add_int(50)", a.to_string(), "150")
	a.sub_int(50)
	assert_eq("150.sub_int(50)", a.to_string(), "100")
	a.mul_int(3)
	assert_eq("100.mul_int(3)", a.to_string(), "300")
	a.div_int(3)
	assert_eq("300.div_int(3)", a.to_string(), "100")

# ─── Comparaisons d'instance ──────────────────────────────────────────────────

func test_instance_comparisons() -> void:
	print("\n── instance comparisons ──")
	var a := BigInt.create(10)
	var b := BigInt.create(20)
	var c := BigInt.create(10)
	assert_true ("a.equals(c)",              a.equals(c))
	assert_false("a.equals(b)",              a.equals(b))
	assert_true ("a.less_than(b)",           a.less_than(b))
	assert_false("b.less_than(a)",           b.less_than(a))
	assert_true ("b.greater_than(a)",        b.greater_than(a))
	assert_true ("a.less_than_or_equal(c)",  a.less_than_or_equal(c))
	assert_true ("a.less_than_or_equal(b)",  a.less_than_or_equal(b))
	assert_true ("a.greater_than_or_equal(c)", a.greater_than_or_equal(c))

# ─── is_zero / is_negative ────────────────────────────────────────────────────

func test_is_zero() -> void:
	print("\n── is_zero ──")
	assert_true ("is_zero(0)",    BigInt.create(0).is_zero())
	assert_false("is_zero(1)",    BigInt.create(1).is_zero())
	assert_false("is_zero(-1)",   BigInt.create(-1).is_zero())

func test_is_negative() -> void:
	print("\n── is_negative ──")
	assert_true ("is_negative(-1)",  BigInt.create(-1).is_negative())
	assert_false("is_negative(0)",   BigInt.create(0).is_negative())
	assert_false("is_negative(1)",   BigInt.create(1).is_negative())

# ─── Grands nombres ───────────────────────────────────────────────────────────

func test_large_numbers() -> void:
	print("\n── large numbers ──")
	var big := BigInt.create_from_string("4324357313743573544542357453789213")
	var one := BigInt.create(1)
	assert_eq("big + 1 exact",
		BigInt.plus(big, one).to_string(),
		"4324357313743573544542357453789214"
	)
	assert_eq("big * 2",
		BigInt.multiply(big, BigInt.create(2)).to_string(),
		"8648714627487147089084714907578426"
	)
	assert_eq("big - big = 0",
		BigInt.minus(big, big).to_string(), "0"
	)

# ─── Edge cases ───────────────────────────────────────────────────────────────

func test_edge_cases() -> void:
	print("\n── edge cases ──")
	# Négatif × négatif
	assert_eq("-big * -1",
		BigInt.multiply(
			BigInt.create_from_string("-9999999999999999999"),
			BigInt.create(-1)
		).to_string(), "9999999999999999999"
	)
	# Chaînage mutant
	var a := BigInt.create(1)
	for i in range(10):
		a.mul_int(2)
	assert_eq("2^10 via mul_int", a.to_string(), "1024")
	# to_int64 aller-retour
	var b := BigInt.create(123456789)
	assert_eq("to_int64 round-trip", str(b.to_int64()), "123456789")
