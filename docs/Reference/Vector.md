# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded. Plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Vector` (properties,
factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md). This page covers only the operators the extension adds on top of it.

`Vector` gets the four basic arithmetic operators: `+`, `-`, `*`, and `/`. It does not get (nor require) `**` as raising
a vector to a power is not a valid mathematical operation. The extension adds no comparison operators either: there's no
natural sort order for a vector, so no `compare` handler is installed. PHP's own default object comparison still
applies, as it does to any plain PHP object (see [Comparison Operators](Comparison_Operators.md)).

On this page, `$v`, `$v1` and `$v2` are `Vector` values, `$m` is a `Matrix`, and `$x` is an `int` or `float`.

---

## Unary Arithmetic Operators

### + (clone)

```php
$copy = +$v;  // +Vector → Vector
```

Returns a new `Vector` with the same elements. Since `Vector` is mutable, this matters: mutating the copy never affects
the original, and vice versa.

**Examples:**

```php
$v = Vector::fromArray([1, 2, 3]);
$copy = +$v;  // [1, 2, 3]
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `+$v`     | `clone $v`    |

---

### - (negate)

```php
$negated = -$v;  // -Vector → Vector
```

Each element of the `Vector` is negated.

**Examples:**

```php
$v = Vector::fromArray([1, -2, 3]);
$negated = -$v;  // [-1, 2, -3]
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `-$v`     | `$v->neg()`   |

---

## Binary Arithmetic Operators

The equivalence table in each section gives the matching method.

---

### + (add)

```php
$sum = $v1 + $v2;  // Vector + Vector → Vector
```

Element-wise; both vectors must be the same count. No other form is supported: there's no scalar addition for `Vector`.

**Throws:**

- `LengthException` if the vectors have different counts.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Vector`.

**Examples:**

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v2 = Vector::fromArray([4, 5, 6]);
$v1 + $v2;  // [5, 7, 9]
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$v1 + $v2` | `$v1->add($v2)` |

---

### - (subtract)

```php
$diff = $v1 - $v2;  // Vector - Vector → Vector
```

Element-wise; both vectors must be the same count. As with `+`, no other form is supported.

**Throws:**

- `LengthException` if the vectors have different counts.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Vector`.

**Examples:**

```php
$v1 = Vector::fromArray([4, 5, 6]);
$v2 = Vector::fromArray([1, 2, 3]);
$v1 - $v2;  // [3, 3, 3]
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$v1 - $v2` | `$v1->sub($v2)` |

---

### \* (multiply)

```php
$result = $v * $x;    // Vector * int|float → Vector
$result = $x * $v;    // int|float * Vector → Vector
$result = $v1 * $v2;  // Vector * Vector → Vector (Hadamard product)
$result = $v * $m;    // Vector * Matrix → Vector
```

Multiplying by a scalar is element-wise, and commutative, so reversing the operands gives the same result.

For `Vector * Matrix`, the `Vector` is treated as a 1×n row matrix and multiplied by the `Matrix` using standard matrix
multiplication. The number of rows in the `Matrix` must equal the `Vector`'s count. The `Matrix * Vector` operation is
documented in [Matrix Operators](Matrix.md#-multiply).

`Vector * Vector` is the Hadamard (element-wise) product, matching [`mul()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul) with a vector. The vectors must have the same
count. For the other ways to multiply two vectors, call the method: [`dot()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#dot), [`cross()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#cross) or [`outer()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#outer).

**Throws:**

- `DomainException` if the scalar isn't finite.
- `LengthException` if the vectors have different counts, or the matrix's row count doesn't equal the vector's count.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Vector`, `Matrix`, `int` or `float`.

**Examples:**

```php
$v = Vector::fromArray([1, 2, 3]);
$v * 2;     // [2, 4, 6]
2 * $v;     // [2, 4, 6]
$v * 2.5;   // [2.5, 5, 7.5]
2.5 * $v;   // [2.5, 5, 7.5]

$v1 = Vector::fromArray([1, 2, 3]);
$v2 = Vector::fromArray([4, 5, 6]);
$v1 * $v2;  // [4, 10, 18]

$m = Matrix::fromArray([[1, 0], [0, 1]]);
$v = Vector::fromArray([1, 2]);
$v * $m;    // [1, 2]  ($v is treated as a 1×2 row matrix)
```

**Equivalence table:**

| Operation   | Equivalent to   |
| ----------- | --------------- |
| `$v * $x`   | `$v->mul($x)`   |
| `$x * $v`   | `$v->mul($x)`   |
| `$v1 * $v2` | `$v1->mul($v2)` |
| `$v * $m`   | `$v->mul($m)`   |

---

### / (divide)

```php
$result = $v / $x;    // Vector / int|float → Vector
$result = $x / $v;    // int|float / Vector → Vector
$result = $v1 / $v2;  // Vector / Vector → Vector (Hadamard division)
```

All three forms are element-wise, and not commutative. `int|float / Vector` divides the scalar by each element of the
`Vector` in turn. There's no equivalent method for it, but the equivalence table shows how to get the same result. It
throws `ArithmeticException` if any of the vector's elements equal zero, matching `reciprocal()`.

`Vector / Vector` divides each element of `$v1` by the matching element of `$v2`, matching [`div()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#div) with a vector. The
vectors must have the same count, and it throws `ArithmeticException` if any element of `$v2` is zero.

Unsupported form:

1. `Vector / Matrix`. Division of a vector by a matrix isn't a typical operation. If the intent is to multiply a vector
   by a matrix's inverse (i.e. compute v ⨉ M⁻¹) it can be achieved with `$v * $m ** -1` or, equivalently,
   `$v * $m->inv()`. This requires the matrix to be square, in addition to having the same number of columns as the
   vector has elements.

**Throws:**

- `ArithmeticException` if dividing by zero, or by a vector with a zero element.
- `DomainException` if the scalar divisor isn't finite.
- `LengthException` if the vectors have different counts.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Vector`, `int` or `float`.

**Examples:**

```php
$v = Vector::fromArray([2, 4, 6]);
$v / 2;     // [1, 2, 3]
$v / 2.5;   // [0.8, 1.6, 2.4]

$v = Vector::fromArray([1, 2, 4]);
8 / $v;     // [8, 4, 2]

$v1 = Vector::fromArray([2, 9, 6]);
$v2 = Vector::fromArray([1, 3, 4]);
$v1 / $v2;  // [2, 3, 1.5]
```

**Equivalence table:**

| Operation   | Equivalent to                                          |
| ----------- | ------------------------------------------------------ |
| `$v / $x`   | `$v->div($x)`                                          |
| `$x / $v`   | `$x * $v->reciprocal()` or `$v->reciprocal()->mul($x)` |
| `$v1 / $v2` | `$v1->div($v2)`                                        |

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the Math package.
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix side of the
  interaction.
- **[Comparison Operators](Comparison_Operators.md)** - how comparison works across all four classes; `Vector` gets no extension-provided
  comparison handler, only PHP's own default object comparison.
