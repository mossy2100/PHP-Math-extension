# Matrix Operators

Operator overloading for `OceanMoon\Math\Matrix`, available only when this extension is loaded. Plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Matrix` (properties,
factory methods, conversion, comparison, transpose/determinant/inverse, `ArrayAccess`, etc.), see the package's own
[Matrix documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md). This page covers only the operators the extension adds on top of it.

`Matrix` gets the four basic arithmetic operators (`+`, `-`, `*`, `/`) plus `**` for matrix powers. The extension adds
no comparison operators either: there's no natural sort order for a matrix, so no `compare` handler is installed. PHP's
own default object comparison still applies, as it does to any plain PHP object (see [Comparison Operators](Comparison_Operators.md)).

On this page, `$m`, `$m1` and `$m2` are `Matrix` values, `$v` is a `Vector`, `$i` is an `int`, and `$x` is an `int` or
`float`.

---

## Unary Arithmetic Operators

### \+ (clone)

```php
$copy = +$m;  // +Matrix → Matrix
```

Returns a new `Matrix` with the same elements. Since `Matrix` is mutable, this matters: mutating the copy never affects
the original, and vice versa, including at the row level, since cloning a `Matrix` deep-clones its row `Vector`s too
(matching the package's own `Matrix::__clone()`).

**Examples:**

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$copy = +$m;  // [[1, 2], [3, 4]]
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `+$m`     | `clone $m`    |

---

### \- (negate)

```php
$negated = -$m;  // -Matrix → Matrix
```

Each element of the `Matrix` is negated.

**Examples:**

```php
$m = Matrix::fromArray([[1, -2], [3, -4]]);
$negated = -$m;  // [[-1, 2], [-3, 4]]
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `-$m`     | `$m->neg()`   |

---

## Binary Arithmetic Operators

The equivalence table in each section gives the matching method.

---

### \+ (add)

```php
$sum = $m1 + $m2;  // Matrix + Matrix → Matrix
```

Both matrices must have the same dimensions. No other form is supported: there's no scalar addition for `Matrix`.

**Throws:**

- `LengthException` if the matrices have different dimensions.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Matrix`.

**Examples:**

```php
$m1 = Matrix::fromArray([[1, 2], [3, 4]]);
$m2 = Matrix::fromArray([[5, 6], [7, 8]]);
$m1 + $m2;  // [[6, 8], [10, 12]]
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$m1 + $m2` | `$m1->add($m2)` |

---

### \- (subtract)

```php
$diff = $m1 - $m2;  // Matrix - Matrix → Matrix
```

Both matrices must have the same dimensions. As with `+`, no other form is supported.

**Throws:**

- `LengthException` if the matrices have different dimensions.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Matrix`.

**Examples:**

```php
$m1 = Matrix::fromArray([[5, 6], [7, 8]]);
$m2 = Matrix::fromArray([[1, 2], [3, 4]]);
$m1 - $m2;  // [[4, 4], [4, 4]]
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$m1 - $m2` | `$m1->sub($m2)` |

---

### \* (multiply)

```php
$result = $m * $x;    // Matrix * int|float → Matrix
$result = $x * $m;    // int|float * Matrix → Matrix
$result = $m1 * $m2;  // Matrix * Matrix → Matrix
$result = $m * $v;    // Matrix * Vector → Vector
```

Multiplying by a scalar is element-wise, and commutative, so reversing the operands gives the same result.

`Matrix * Matrix` is standard matrix multiplication. The number of columns in the first `Matrix` must equal the number
of rows in the second.

For `Matrix * Vector`, the `Vector` is treated as a single-column `Matrix`, and the result is a `Vector`. It isn't
commutative: `Vector * Matrix` is a distinct calculation, documented in [Vector Operators](Vector.md#-multiply).

**Throws:**

- `DomainException` if the scalar isn't finite.
- `LengthException` if the right operand's row count (or a vector's count) doesn't equal the left matrix's column count.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Matrix`, `Vector`, `int` or `float`.

**Examples:**

```php
$m1 = Matrix::fromArray([[1, 2], [3, 4]]);
$m1 * 2;     // [[2, 4], [6, 8]]
2 * $m1;     // [[2, 4], [6, 8]]

$m2 = Matrix::fromArray([[5, 6], [7, 8]]);
$m1 * $m2;   // [[19, 22], [43, 50]]

$v = Vector::fromArray([1, 2]);
$m1 * $v;    // [5, 11]  ($v is treated as a 2×1 column matrix)
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$m * $x`   | `$m->mul($x)`   |
| `$x * $m`   | `$m->mul($x)`   |
| `$m1 * $m2` | `$m1->mul($m2)` |
| `$m * $v`   | `$m->mul($v)`   |

---

### / (divide)

```php
$result = $m / $x;  // Matrix / int|float → Matrix
$result = $x / $m;  // int|float / Matrix → Matrix
```

Both forms are element-wise, and not commutative. `$x / $m` divides the scalar by each element of the
`Matrix` in turn. There's no equivalent named method for this direction. `Matrix::div()` only ever divides the `Matrix`
by a scalar, never the reverse. It throws `ArithmeticException` for a zero element, matching `hadamardInv()`. This is
distinct from `x * A⁻¹` (inverse scaling). Use `$x * $m ** -1` (or `$x * $m->inv()`) for that instead.

Unsupported form:

1. `Matrix / Matrix`. Ambiguous in two different ways: it could mean `A * B⁻¹` or `B⁻¹ * A` (these differ in general,
   since matrix multiplication isn't commutative), or it could mean Hadamard division - element-wise division of two
   same-shaped matrices. Rather than guess which one you mean, make it explicit: `$m1 * $m2 ** -1` (or
   [`$m1 * $m2->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv)) for `A * B⁻¹`, `$m2 ** -1 * $m1` for `B⁻¹ * A`, or [`$m1->hadamardDiv($m2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#hadamarddiv) for the element-wise
   quotient.

**Throws:**

- `ArithmeticException` if dividing by zero, or by a matrix with a zero element.
- `DomainException` if the scalar isn't finite, on either side.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't an `int` or `float`.

**Examples:**

```php
$m = Matrix::fromArray([[2, 4], [6, 8]]);
$m / 2;     // [[1, 2], [3, 4]]

$m = Matrix::fromArray([[1, 2], [4, 8]]);
8 / $m;     // [[8, 4], [2, 1]]
```

**Equivalence table:**

| Operation | Equivalent to                |
| --------- | ---------------------------- |
| `$m / $x` | `$m->div($x)`                |
| `$x / $m` | `$m->hadamardInv()->mul($x)` |

---

### \*\* (power)

```php
$result = $m ** $i;  // Matrix ** int → Matrix
```

The matrix must be square; negative exponents use the matrix inverse.

Unsupported form:

1. `int ** Matrix`. A scalar raised to a matrix power isn't a standard operation in linear algebra.

**Throws:**

- `ArithmeticException` if the exponent is negative and the matrix is singular.
- `DomainException` if the matrix isn't square.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the exponent isn't an `int`.

**Examples:**

```php
$m = Matrix::fromArray([[1, 1], [0, 1]]);
$m ** 2;    // [[1, 2], [0, 1]]
$m ** -1;   // [[1, -1], [0, 1]]  (the inverse)
```

**Equivalence table:**

| Operation  | Equivalent to |
| ---------- | ------------- |
| `$m ** $i` | `$m->pow($i)` |

---

### % (unsupported)

`%` isn't supported, with a `Matrix` on either side. PHP throws its usual `TypeError`, e.g.
`Unsupported operand types: OceanMoon\Math\Matrix % int`.

---

## Compound Assignment Operators

`+=`, `-=`, `*=`, `/=` and `**=` accept the same operands as the matching binary operator, and give the same result.

The variable is assigned a new `Matrix`. The object it held isn't modified, so any other variable referring to that
object keeps its value. Since `Matrix` is mutable, this matters: `$m *= 2` is not an in-place update. If the operation
throws, the variable is left unchanged.

**Examples:**

```php
$m1 = Matrix::fromArray([[1, 2], [3, 4]]);
$m2 = $m1;
$m1 **= 2;                                  // $m1 is [[7, 10], [15, 22]], $m2 is still [[1, 2], [3, 4]]

$x = 2;
$x *= Matrix::fromArray([[1, 2], [3, 4]]);  // $x is now the Matrix [[2, 4], [6, 8]]
```

**Equivalence table:**

| Operation    | Equivalent to     |
| ------------ | ----------------- |
| `$m1 += $m2` | `$m1 = $m1 + $m2` |
| `$m1 -= $m2` | `$m1 = $m1 - $m2` |
| `$m *= $x`   | `$m = $m * $x`    |
| `$m1 *= $m2` | `$m1 = $m1 * $m2` |
| `$m /= $x`   | `$m = $m / $x`    |
| `$m **= $i`  | `$m = $m ** $i`   |

---

## Inverting with `A ** -1`

Since `Matrix ** int` is supported, `$m ** -1` computes the same inverse as [`$m->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv), since raising to the power -1
is the mathematical definition of a matrix inverse. Either will throw `DomainException` if `$m` isn't square, and
`ArithmeticException` if `$m` is singular.

**Example:**

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m->inv();  // [[-2, 1], [1.5, -0.5]]
$m ** -1;   // [[-2, 1], [1.5, -0.5]], same result
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the Math package.
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector side of the
  interaction.
- **[Comparison Operators](Comparison_Operators.md)** - how comparison works across all four classes; `Matrix` gets no extension-provided
  comparison handler, only PHP's own default object comparison.
