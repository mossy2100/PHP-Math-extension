# Matrix Operators

Operator overloading for `OceanMoon\Math\Matrix`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Matrix` (properties,
factory methods, conversion, comparison, transpose/determinant/inverse, `ArrayAccess`, etc.), see the package's own
[Matrix documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md) - this page covers only the
operators the extension adds on top of it.

_Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status._

---

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$m;
```

Returns a new `Matrix` with the same elements. There's no package method this maps to - value identity needs no explicit
method call in ordinary PHP code - but the extension implements the `ZEND_UNARY_PLUS` handler anyway, for parity with
PHP's built-in `+$x` on `int`/`float`. Since `Matrix` is mutable, this returns a distinct copy rather than `$this`, so
mutating the result never affects the original - the same rule the package's own arithmetic methods follow (see the note
on `pow(1)` in the package's `Matrix` docs).

**Example:**

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$copy = +$m;  // [[1, 2], [3, 4]]
```

### - (negate)

```php
$negated = -$m;
```

Equivalent to [`$m->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#neg). Element-wise.

**Example:**

```php
$m = Matrix::fromArray([[1, -2], [3, -4]]);
$negated = -$m;  // [[-1, 2], [-3, 4]]
```

---

## Binary Arithmetic Operators

Each operator is equivalent to calling the same-named method directly, and throws the same exceptions, under the same
conditions, as that method - see the linked method docs for specifics.

### + (add)

```php
$sum = $a + $b;
```

Equivalent to [`$a->add($b)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#add). Both matrices must
have the same dimensions. Only `Matrix + Matrix` is supported.

```php
$a = Matrix::fromArray([[1, 2], [3, 4]]);
$b = Matrix::fromArray([[5, 6], [7, 8]]);
$a + $b;  // [[6, 8], [10, 12]]
```

### - (subtract)

```php
$diff = $a - $b;
```

Equivalent to [`$a->sub($b)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#sub). Both matrices must
have the same dimensions. As with `+`, only `Matrix - Matrix` is supported.

```php
$a = Matrix::fromArray([[5, 6], [7, 8]]);
$b = Matrix::fromArray([[1, 2], [3, 4]]);
$a - $b;  // [[4, 4], [4, 4]]
```

### \* (multiply)

```php
$result = $a * $other;
```

Equivalent to [`$a->mul($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul). There are 3 ways
this operator can be used.

1. `Matrix` \* `int|float` or `int|float` \* `Matrix`. Scalar multiplication is commutative, so `int|float * Matrix`
   gives the same result.
2. `Matrix` \* `Matrix`. Standard matrix multiplication - the result is a Matrix.
3. `Matrix` \* `Vector`. The Vector is treated as a single-column Matrix, and the result is a Vector. This operation is
   not commutative; for `Vector` \* `Matrix`, see [Vector Operators](Vector.md#-multiply).

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m * 2;  // [[2, 4], [6, 8]]   (Matrix * int|float)
2 * $m;  // [[2, 4], [6, 8]]   (int|float * Matrix - same result, scalar multiplication is commutative)

$m2 = Matrix::fromArray([[5, 6], [7, 8]]);
$m * $m2;  // [[19, 22], [43, 50]]   (Matrix * Matrix)

$v = Vector::fromArray([1, 2]);
$m * $v;  // Vector(5, 11)   (Matrix * Vector - $v treated as a 2x1 column matrix)
```

### / (divide)

```php
$result = $a / $other;
```

Equivalent to [`$a->div($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#div). There are 3 ways
this operator can be used:

1. `Matrix / int|float`. Scalar division.
2. `Matrix / Matrix`. Division defined as A × B⁻¹ (i.e. `$a * $b->inv()`, equivalent to `$a * $b ** -1`)
3. `int|float / Matrix`. Computed as `$scalar * $other->inv()`. This also gives a way to compute a matrix inverse - see
   "Inverting with `1 / m` or `m ** -1`" below.

```php
$m = Matrix::fromArray([[2, 4], [6, 8]]);
$m / 2;  // [[1, 2], [3, 4]]   (Matrix / int|float)

$a = Matrix::fromArray([[1, 0], [0, 1]]);
$b = Matrix::fromArray([[2, 0], [0, 2]]);
$a / $b;  // [[0.5, 0], [0, 0.5]]   (Matrix / Matrix, A x B^-1)
```

### \*\* (power)

```php
$result = $m ** $exponent;
```

Equivalent to [`$m->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#pow). Only
`Matrix ** int` is supported - the operand must be an `int` (matching `pow()`'s signature), and there's no
`int ** Matrix` case, since a scalar raised to a matrix power isn't a standard operation in linear algebra. The matrix
must be square; negative exponents use the matrix inverse.

```php
$m = Matrix::fromArray([[1, 1], [0, 1]]);
$m ** 2;   // [[1, 2], [0, 1]]
$m ** -1;  // inverse matrix
```

---

## Inverting with `1 / m` or `m ** -1`

Since `int|float / Matrix` is supported (see `/` above), `1 / $m` computes the same inverse as
[`$m->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv). `$m ** -1` gives the same result too,
since raising to the power -1 is the mathematical definition of a matrix inverse. All three throw `DomainException` if
`$m` isn't square, and `ArithmeticException` if `$m` isn't invertible (zero determinant), matching `inv()`.

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m->inv();  // [[-2, 1], [1.5, -0.5]]
1 / $m;     // [[-2, 1], [1.5, -0.5]], same result
$m ** -1;   // [[-2, 1], [1.5, -0.5]], same result
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the
  Math package
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector side
  of the interaction
