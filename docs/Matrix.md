# Matrix Operators

Operator overloading for `OceanMoon\Math\Matrix`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Matrix`
(properties, factory methods, conversion, comparison, transpose/determinant/inverse, `ArrayAccess`, etc.), see the
package's own [Matrix documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md) - this page
covers only the operators the extension adds on top of it.

*Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status.*

---

## Binary Arithmetic Operators

Each operator below is equivalent to calling the same-named method directly, and accepts the same operand type(s)
and throws the same exceptions - see the linked method docs for full details.

### + (add)

```php
$sum = $a + $b;
```

Equivalent to [`$a->add($b)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#add). Both matrices
must have the same dimensions.

```php
$a = Matrix::fromArray([[1, 2], [3, 4]]);
$b = Matrix::fromArray([[5, 6], [7, 8]]);
$sum = $a + $b;  // [[6, 8], [10, 12]]
```

### - (subtract)

```php
$diff = $a - $b;
```

Equivalent to [`$a->sub($b)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#sub). Both matrices
must have the same dimensions.

```php
$a = Matrix::fromArray([[5, 6], [7, 8]]);
$b = Matrix::fromArray([[1, 2], [3, 4]]);
$diff = $a - $b;  // [[4, 4], [4, 4]]
```

### * (multiply)

```php
$result = $a * $other;
```

Equivalent to [`$a->mul($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#mul). `$other` may
be a `float` (scalar), a `Vector` (treated as a column vector - result is a `Vector`), or a `Matrix` (standard matrix
multiplication - result is a `Matrix`).

```php
$m = Matrix::fromArray([[1, 2], [3, 4]]);
$m2 = Matrix::fromArray([[5, 6], [7, 8]]);
$product = $m * $m2;  // [[19, 22], [43, 50]]
```

### / (divide)

```php
$result = $a / $other;
```

Equivalent to [`$a->div($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#div). `$other` may
be a `float` (scalar) or a `Matrix` (division defined as A × B⁻¹).

```php
$m = Matrix::fromArray([[2, 4], [6, 8]]);
$result = $m / 2;  // [[1, 2], [3, 4]]
```

### ** (power)

```php
$result = $m ** $exponent;
```

Equivalent to [`$m->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#pow). The
operand must be an `int` (matching `pow()`'s signature); the matrix must be square. Negative exponents use the
matrix inverse.

```php
$m = Matrix::fromArray([[1, 1], [0, 1]]);
$m2 = $m ** 2;   // [[1, 2], [0, 1]]
$mi = $m ** -1;  // inverse matrix
```

---

## See Also

- **[Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)** - the full class documentation in the
  Math package
- **[Vector Operators](Vector.md)** - `Matrix * Vector` and `Vector * Matrix` both work; see there for the vector
  side of the interaction
