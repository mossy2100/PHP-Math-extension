# Matrix Operators

Operator overloading for `OceanMoon\Math\Matrix`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Matrix` (properties,
factory methods, conversion, comparison, transpose/determinant/inverse, `ArrayAccess`, etc.), see the package's own
[Matrix documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md) - this page covers only the
operators the extension adds on top of it.

`Matrix` gets the four basic arithmetic operators (`+`, `-`, `*`, `/`) plus `**` for matrix powers. No comparison
operators are provided, since matrices have no natural sort order.

---

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$A;
```

Returns a new `Matrix` with the same elements. There's no package method this maps to; value identity needs no explicit
method call in ordinary PHP code. PHP has no dedicated opcode for unary `+`/`-`; the compiler lowers `+$A` to `$A * 1`,
which the extension handles via the scalar form of `*` (see below). Since `Matrix` is mutable, this returns a distinct
copy rather than `$this`, so mutating the result never affects the original - the same rule the package's own arithmetic
methods follow (see the note on `pow(1)` in the package's `Matrix` docs).

**Example:**

```php
$A = Matrix::fromArray([[1, 2], [3, 4]]);
$copy = +$A;  // [[1, 2], [3, 4]]
```

---

### - (negate)

```php
$negated = -$A;
```

Equivalent to [`$A->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#neg). Element-wise.

**Example:**

```php
$A = Matrix::fromArray([[1, -2], [3, -4]]);
$negated = -$A;  // [[-1, 2], [-3, 4]]
```

Equivalence table for the unary `-` operator, where `$A` is a `Matrix`.

| Operation | Equivalent to |
| --------- | ------------- |
| `-$A`     | `$A->neg()`   |

---

## Binary Arithmetic Operators

Each operator is equivalent to calling the same-named method directly, and throws the same exceptions, under the same
conditions, as that method - see the linked method docs for specifics.

---

### + (add)

```php
$sum = $A + $B;
```

Equivalent to [`$A->add($B)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#add). Both matrices must
have the same dimensions.

Forms:

1. `Matrix + Matrix`. No other form is supported - there's no scalar addition for `Matrix`.

**Example:**

```php
$A = Matrix::fromArray([[1, 2], [3, 4]]);
$B = Matrix::fromArray([[5, 6], [7, 8]]);
$A + $B;  // [[6, 8], [10, 12]]
```

Equivalence table for the `+` operator, where `$A` and `$B` are `Matrix` values.

| Operation | Equivalent to |
| --------- | ------------- |
| `$A + $B` | `$A->add($B)` |

---

### - (subtract)

```php
$diff = $A - $B;
```

Equivalent to [`$A->sub($B)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#sub). Both matrices must
have the same dimensions.

Forms:

1. `Matrix - Matrix`. As with `+`, no other form is supported.

**Example:**

```php
$A = Matrix::fromArray([[5, 6], [7, 8]]);
$B = Matrix::fromArray([[1, 2], [3, 4]]);
$A - $B;  // [[4, 4], [4, 4]]
```

Equivalence table for the `-` operator, where `$A` and `$B` are `Matrix` values.

| Operation | Equivalent to |
| --------- | ------------- |
| `$A - $B` | `$A->sub($B)` |

---

### \* (multiply)

```php
$result = $A * $other;
```

Forms:

1. `Matrix * int|float`. Element-wise multiplication of every element by a scalar - equivalent to
   [`$A->mul($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul).
2. `int|float * Matrix`. Scalar multiplication is commutative, so this gives the same result as #1.
3. `Matrix * Matrix`. Standard matrix multiplication - the result is a `Matrix` - equivalent to
   [`$A->mul($B)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul). The number of columns in the
   first `Matrix` must equal the number of rows in the second `Matrix`.
4. `Matrix * Vector`. The `Vector` is treated as a single-column `Matrix`, and the result is a `Vector` - equivalent to
   [`$mat->mulVector($vec)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mulvector). Not commutative
   - see #5.

`Vector * Matrix` is a distinct calculation, documented in [Vector Operators](Vector.md#-multiply).

**Example:**

```php
$A = Matrix::fromArray([[1, 2], [3, 4]]);
$A * 2;  // [[2, 4], [6, 8]]   (Matrix * int)
2 * $A;  // [[2, 4], [6, 8]]   (int * Matrix - same result, scalar multiplication is commutative)

$B = Matrix::fromArray([[5, 6], [7, 8]]);
$A * $B;  // [[19, 22], [43, 50]]   (Matrix * Matrix)

$v = Vector::fromArray([1, 2]);
$A * $v;  // Vector(5, 11)   (Matrix * Vector - $v treated as a 2x1 column matrix)
```

Equivalence table for the `*` operator, where `$A` and `$B` are `Matrix` values, `$x` is an `int` or `float`, and `$v`
is a `Vector`.

| Operation | Equivalent to       |
| --------- | ------------------- |
| `$A * $x` | `$A->mul($x)`       |
| `$x * $A` | `$A->mul($x)`       |
| `$A * $B` | `$A->mul($B)`       |
| `$A * $v` | `$A->mulVector($v)` |

---

### / (divide)

```php
$result = $A / $scalar;
```

Equivalent to [`$A->div($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#div).

Forms:

1. `Matrix / int|float`. Element-wise division of every element by a scalar.

Unsupported forms:

1. `Matrix / Matrix`
2. `int|float / Matrix`

`Matrix / Matrix` is unsupported because it's ambiguous in two different ways: it could mean `A * B⁻¹` or `B⁻¹ * A`
(these differ in general, since matrix multiplication isn't commutative), or it could mean Hadamard division -
element-wise division of two same-shaped matrices. Rather than guess which one you mean, make it explicit:
`$A * $B ** -1` (or [`$A * $B->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv)) for
`A * B⁻¹`, `$B ** -1 * $A` for `B⁻¹ * A`, or
[`$A->hadamardDiv($B)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#hadamarddiv) for the element-wise
quotient.

`int|float / Matrix` is unsupported for a related but distinct reason. Unlike `Matrix / Matrix`, scalar multiplication
with a matrix is commutative (i.e. `x * A` and `A * x` are always equal), so there's no order ambiguity here - but
`x / A` is still ambiguous, between `x * A⁻¹` (inverse scaling) and dividing `x` by every element of `A` element-wise.
Use `$x * $A ** -1` (or `$x * $A->inv()`) for the former; use
[`$x * $A->reciprocal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#reciprocal) for the latter.

**Example:**

```php
$A = Matrix::fromArray([[2, 4], [6, 8]]);
$A / 2;  // [[1, 2], [3, 4]]   (Matrix / int)
```

Equivalence table for the `/` operator, where `$A` is a `Matrix`, and `$x` is an `int` or `float`.

| Operation | Equivalent to |
| --------- | ------------- |
| `$A / $x` | `$A->div($x)` |

---

### \*\* (power)

```php
$result = $A ** $exponent;
```

Equivalent to [`$A->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#pow).

Forms:

1. `Matrix ** int`. The matrix must be square; negative exponents use the matrix inverse.

Unsupported forms:

1. `int ** Matrix`. A scalar raised to a matrix power isn't a standard operation in linear algebra.

**Example:**

```php
$A = Matrix::fromArray([[1, 1], [0, 1]]);
$A ** 2;   // [[1, 2], [0, 1]]
$A ** -1;  // inverse matrix
```

Equivalence table for the `**` operator, where `$A` is a `Matrix`, and `$i` is an `int`.

| Operation  | Equivalent to |
| ---------- | ------------- |
| `$A ** $i` | `$A->pow($i)` |

---

## Inverting with `A ** -1`

Since `Matrix ** int` is supported, `$A ** -1` computes the same inverse as
[`$A->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv), since raising to the power -1 is the
mathematical definition of a matrix inverse. Either will throw `DomainException` if `$A` isn't square, and
`ArithmeticException` if `$A` isn't invertible (zero determinant).

**Example:**

```php
$A = Matrix::fromArray([[1, 2], [3, 4]]);
$A->inv();  // [[-2, 1], [1.5, -0.5]]
$A ** -1;   // [[-2, 1], [1.5, -0.5]], same result
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the
  Math package
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector side
  of the interaction
