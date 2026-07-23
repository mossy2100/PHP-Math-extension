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
$copy = +$m;
```

Returns a new `Matrix` with the same elements. There's no package method this maps to; value identity needs no explicit
method call in ordinary PHP code. PHP has no dedicated opcode for unary `+`/`-`; the compiler lowers `+$m` to `$m * 1`,
which the extension handles via the scalar form of `*` (see below). Since `Matrix` is mutable, this returns a distinct
copy rather than `$this`, so mutating the result never affects the original - the same rule the package's own arithmetic
methods follow (see the note on `pow(1)` in the package's `Matrix` docs).

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
have the same dimensions.

Forms:

1. `Matrix + Matrix`. No other form is supported - there's no scalar addition for `Matrix`.

**Example:**

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
have the same dimensions.

Forms:

1. `Matrix - Matrix`. As with `+`, no other form is supported.

**Example:**

```php
$a = Matrix::fromArray([[5, 6], [7, 8]]);
$b = Matrix::fromArray([[1, 2], [3, 4]]);
$a - $b;  // [[4, 4], [4, 4]]
```

### \* (multiply)

```php
$result = $a * $other;
```

Forms:

1. `Matrix * int|float`. Element-wise multiplication of every element by a scalar - equivalent to
   [`$a->mul($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul).
2. `int|float * Matrix`. Scalar multiplication is commutative, so this gives the same result as #1.
3. `Matrix * Matrix`. Standard matrix multiplication - the result is a `Matrix` - equivalent to
   [`$a->mul($b)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul). The number of columns in the
   first `Matrix` must equal the number of rows in the second `Matrix`.
4. `Matrix * Vector`. The `Vector` is treated as a single-column `Matrix`, and the result is a `Vector` - equivalent to
   [`$mat->mulVector($vec)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mulvector). Not commutative
   - see #5.
5. `Vector * Matrix`. A distinct calculation, documented in [Vector Operators](Vector.md#-multiply).

**Example:**

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m * 2;  // [[2, 4], [6, 8]]   (Matrix * int)
2 * $m;  // [[2, 4], [6, 8]]   (int * Matrix - same result, scalar multiplication is commutative)

$m2 = Matrix::fromArray([[5, 6], [7, 8]]);
$m * $m2;  // [[19, 22], [43, 50]]   (Matrix * Matrix)

$v = Vector::fromArray([1, 2]);
$m * $v;  // Vector(5, 11)   (Matrix * Vector - $v treated as a 2x1 column matrix)
```

### / (divide)

```php
$result = $a / $other;
```

Equivalent to [`$a->div($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#div).

Forms:

1. `Matrix / int|float`. Element-wise division of every element by a scalar.

Unsupported forms:

1. `Matrix / Matrix`
2. `int|float / Matrix`

These forms are unsupported for the same reason `Vector / Matrix` is unsupported. Division by a matrix is not a common
operation, and can be ambiguous, since A/B can mean B⁻¹\*A or A\*B⁻¹. A `Matrix / Matrix` operation can be achieved by
`$a->mul($b->inv())` (i.e. A\*B⁻¹). A `int|float / Matrix` operation can be achieved by `$x * $a->inv()` (i.e. x\*A⁻¹).

**Example:**

```php
$m = Matrix::fromArray([[2, 4], [6, 8]]);
$m / 2;  // [[1, 2], [3, 4]]   (Matrix / int)
```

### \*\* (power)

```php
$result = $m ** $exponent;
```

Equivalent to [`$m->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#pow).

Forms:

1. `Matrix ** int`. The matrix must be square; negative exponents use the matrix inverse.

Unsupported forms:

1. `int ** Matrix`. A scalar raised to a matrix power isn't a standard operation in linear algebra.

**Example:**

```php
$m = Matrix::fromArray([[1, 1], [0, 1]]);
$m ** 2;   // [[1, 2], [0, 1]]
$m ** -1;  // inverse matrix
```

---

## Inverting with `m ** -1`

Since `Matrix ** int` is supported, `$m ** -1` computes the same inverse as
[`$m->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#inv), since raising to the power -1 is the
mathematical definition of a matrix inverse. Either will throw `DomainException` if `$m` isn't square, and
`ArithmeticException` if `$m` isn't invertible (zero determinant).

**Example:**

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m->inv();  // [[-2, 1], [1.5, -0.5]]
$m ** -1;   // [[-2, 1], [1.5, -0.5]], same result
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the
  Math package
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector side
  of the interaction
