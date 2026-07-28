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
$matB = +$matA;
```

Returns a new `Matrix` with the same elements. There's no package method this maps to; value identity needs no explicit
method call in ordinary PHP code. PHP has no dedicated opcode for unary `+`/`-`; the compiler lowers `+$matA` to
`$matA * 1`, which the extension handles via the scalar form of `*` (see below). Since `Matrix` is mutable, this returns
a distinct copy rather than `$this`, so mutating the result never affects the original - the same rule the package's own
arithmetic methods follow (see the note on `pow(1)` in the package's `Matrix` docs).

**Example:**

```php
$matA = Matrix::fromArray([[1, 2], [3, 4]]);
$matB = +$matA;  // [[1, 2], [3, 4]]
```

---

### - (negate)

```php
$negated = -$matA;
```

Equivalent to [`$matA->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#neg). Element-wise.

**Example:**

```php
$matA = Matrix::fromArray([[1, -2], [3, -4]]);
$negated = -$matA;  // [[-1, 2], [-3, 4]]
```

Equivalence table for the unary `-` operator, where `$matA` is a `Matrix`.

| Operation | Equivalent to    |
| --------- | ---------------- |
| `-$matA`  | `$matA->neg()`   |

---

## Binary Arithmetic Operators

Each operator is equivalent to calling the same-named method directly, and throws the same exceptions, under the same
conditions, as that method - see the linked method docs for specifics.

---

### + (add)

```php
$sum = $matA + $matB;
```

Equivalent to [`$matA->add($matB)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#add). Both matrices
must have the same dimensions.

Forms:

1. `Matrix + Matrix`. No other form is supported - there's no scalar addition for `Matrix`.

**Example:**

```php
$matA = Matrix::fromArray([[1, 2], [3, 4]]);
$matB = Matrix::fromArray([[5, 6], [7, 8]]);
$matA + $matB;  // [[6, 8], [10, 12]]
```

Equivalence table for the `+` operator, where `$matA` and `$matB` are `Matrix` values.

| Operation       | Equivalent to        |
| --------------- | -------------------- |
| `$matA + $matB` | `$matA->add($matB)`  |

---

### - (subtract)

```php
$diff = $matA - $matB;
```

Equivalent to [`$matA->sub($matB)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#sub). Both matrices
must have the same dimensions.

Forms:

1. `Matrix - Matrix`. As with `+`, no other form is supported.

**Example:**

```php
$matA = Matrix::fromArray([[5, 6], [7, 8]]);
$matB = Matrix::fromArray([[1, 2], [3, 4]]);
$matA - $matB;  // [[4, 4], [4, 4]]
```

Equivalence table for the `-` operator, where `$matA` and `$matB` are `Matrix` values.

| Operation       | Equivalent to        |
| --------------- | -------------------- |
| `$matA - $matB` | `$matA->sub($matB)`  |

---

### \* (multiply)

```php
$result = $matA * $other;
```

Forms:

1. `Matrix * int|float`. Element-wise multiplication of every element by a scalar - equivalent to
   [`$matA->mul($x)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul).
2. `int|float * Matrix`. Scalar multiplication is commutative, so this gives the same result as #1.
3. `Matrix * Matrix`. Standard matrix multiplication - the result is a `Matrix` - equivalent to
   [`$matA->mul($matB)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul). The number of columns in
   the first `Matrix` must equal the number of rows in the second `Matrix`.
4. `Matrix * Vector`. The `Vector` is treated as a single-column `Matrix`, and the result is a `Vector` - equivalent to
   [`$matA->mulVector($v)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mulvector). Not commutative
   - see #5.

`Vector * Matrix` is a distinct calculation, documented in [Vector Operators](Vector.md#-multiply).

**Example:**

```php
$matA = Matrix::fromArray([[1, 2], [3, 4]]);
$matA * 2;  // [[2, 4], [6, 8]]   (Matrix * int)
2 * $matA;  // [[2, 4], [6, 8]]   (int * Matrix - same result, scalar multiplication is commutative)

$matB = Matrix::fromArray([[5, 6], [7, 8]]);
$matA * $matB;  // [[19, 22], [43, 50]]   (Matrix * Matrix)

$v = Vector::fromArray([1, 2]);
$matA * $v;  // Vector(5, 11)   (Matrix * Vector - $v treated as a 2x1 column matrix)
```

Equivalence table for the `*` operator, where `$matA` and `$matB` are `Matrix` values, `$x` is an `int` or `float`, and
`$v` is a `Vector`.

| Operation       | Equivalent to          |
| --------------- | ---------------------- |
| `$matA * $x`    | `$matA->mul($x)`       |
| `$x * $matA`    | `$matA->mul($x)`       |
| `$matA * $matB` | `$matA->mul($matB)`    |
| `$matA * $v`    | `$matA->mulVector($v)` |

---

### / (divide)

```php
$result = $matA / $x;
```

Equivalent to [`$matA->div($x)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#div).

Forms:

1. `Matrix / int|float`. Element-wise division of every element by a scalar.
2. `int|float / Matrix`. Not commutative with `Matrix / int|float` - element-wise, divides the scalar by each element
   of the `Matrix` in turn, equivalent to
   [`$x * $matA->reciprocal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#reciprocal). There's no
   equivalent named method for this direction - `Matrix::div()` only ever divides the `Matrix` by a scalar, never the
   reverse. Throws `ArithmeticException` for a zero element, matching `reciprocal()`. This is distinct from `x * A⁻¹`
   (inverse scaling) - use `$x * $matA ** -1` (or `$x * $matA->inv()`) for that instead.

Unsupported form:

1. `Matrix / Matrix`. Ambiguous in two different ways: it could mean `A * B⁻¹` or `B⁻¹ * A` (these differ in general,
   since matrix multiplication isn't commutative), or it could mean Hadamard division - element-wise division of two
   same-shaped matrices. Rather than guess which one you mean, make it explicit: `$matA * $matB ** -1` (or
   [`$matA * $matB->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv)) for `A * B⁻¹`,
   `$matB ** -1 * $matA` for `B⁻¹ * A`, or
   [`$matA->hadamardDiv($matB)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#hadamarddiv) for the
   element-wise quotient.

**Example:**

```php
$matA = Matrix::fromArray([[2, 4], [6, 8]]);
$matA / 2;  // [[1, 2], [3, 4]]   (Matrix / int)

$matB = Matrix::fromArray([[1, 2], [4, 8]]);
8 / $matB;  // [[8, 4], [2, 1]]   (int / Matrix - element-wise)
```

Equivalence table for the `/` operator, where `$matA` is a `Matrix`, and `$x` is an `int` or `float`.

| Operation    | Equivalent to              |
| ------------ | --------------------------- |
| `$matA / $x` | `$matA->div($x)`           |
| `$x / $matA` | `$x * $matA->reciprocal()` |

---

### \*\* (power)

```php
$result = $matA ** $i;
```

Equivalent to [`$matA->pow($i)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#pow).

Forms:

1. `Matrix ** int`. The matrix must be square; negative exponents use the matrix inverse.

Unsupported forms:

1. `int ** Matrix`. A scalar raised to a matrix power isn't a standard operation in linear algebra.

**Example:**

```php
$matA = Matrix::fromArray([[1, 1], [0, 1]]);
$matA ** 2;   // [[1, 2], [0, 1]]
$matA ** -1;  // inverse matrix
```

Equivalence table for the `**` operator, where `$matA` is a `Matrix`, and `$i` is an `int`.

| Operation     | Equivalent to     |
| ------------- | ----------------- |
| `$matA ** $i` | `$matA->pow($i)`  |

---

## Inverting with `A ** -1`

Since `Matrix ** int` is supported, `$matA ** -1` computes the same inverse as
[`$matA->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv), since raising to the power -1 is
the mathematical definition of a matrix inverse. Either will throw `DomainException` if `$matA` isn't square, and
`ArithmeticException` if `$matA` isn't invertible (zero determinant).

**Example:**

```php
$matA = Matrix::fromArray([[1, 2], [3, 4]]);
$matA->inv();  // [[-2, 1], [1.5, -0.5]]
$matA ** -1;   // [[-2, 1], [1.5, -0.5]], same result
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the
  Math package
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector side
  of the interaction
