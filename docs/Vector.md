# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Vector` (properties,
factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md) - this page covers only the
operators the extension adds on top of it.

_Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status._

`Vector` gets the four basic arithmetic operators. It does not get `**` - the package has no `pow()` method for `Vector`
to map an operator to, and no design for one yet - nor any comparison operators, since element-wise ordering has no
single natural definition.

---

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$v;
```

Returns a new `Vector` with the same elements. There's no package method this maps to - value identity needs no explicit
method call in ordinary PHP code - but the extension implements the `ZEND_UNARY_PLUS` handler anyway, for parity with
PHP's built-in `+$x` on `int`/`float`. Since `Vector` is mutable, this returns a distinct copy rather than `$this`, so
mutating the result never affects the original - the same rule the package's own arithmetic methods follow.

**Example:**

```php
$v = Vector::fromArray([1, 2, 3]);
$copy = +$v;  // [1, 2, 3]
```

### - (negate)

```php
$negated = -$v;
```

Equivalent to [`$v->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#neg). Element-wise.

**Example:**

```php
$v = Vector::fromArray([1, -2, 3]);
$negated = -$v;  // [-1, 2, -3]
```

---

## Binary Arithmetic Operators

Each operator is equivalent to calling the same-named method directly, and throws the same exceptions, under the same
conditions, as that method - see the linked method docs for specifics.

### + (add)

```php
$sum = $v1 + $v2;
```

Equivalent to [`$v1->add($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#add). Element-wise; both
vectors must be the same size. Only `Vector + Vector` is supported.

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v2 = Vector::fromArray([4, 5, 6]);
$v1 + $v2;  // [5, 7, 9]
```

### - (subtract)

```php
$diff = $v1 - $v2;
```

Equivalent to [`$v1->sub($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#sub). Element-wise; both
vectors must be the same size. As with `+`, only `Vector - Vector` is supported.

```php
$v1 = Vector::fromArray([4, 5, 6]);
$v2 = Vector::fromArray([1, 2, 3]);
$v1 - $v2;  // [3, 3, 3]
```

### \* (multiply)

```php
$result = $v * $other;
```

Equivalent to [`$v->mul($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul). There are 2 ways
this operator can be used:

1. `Vector * int|float` or `int|float * Vector`. Scalar multiplication, element-wise. Commutative, so both give the same
   result.
2. `Vector * Matrix`. This vector is treated as a 1×n row matrix, multiplied by `$other` using standard matrix
   multiplication. Not commutative - `Matrix * Vector` is a distinct operation (different multiplication order),
   documented in [Matrix Operators](Matrix.md#-multiply).

```php
$v = Vector::fromArray([1, 2, 3]);
$v * 2;  // [2, 4, 6]   (Vector * int|float)
2 * $v;  // [2, 4, 6]   (int|float * Vector - same result, scalar multiplication is commutative)

$m = Matrix::fromArray([[1, 0], [0, 1]]);
$v2 = Vector::fromArray([1, 2]);
$v2 * $m;  // [1, 2]   (Vector * Matrix - $v2 treated as a 1x2 row matrix)
```

### / (divide)

```php
$result = $v / $scalar;
```

Equivalent to [`$v->div($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#div). `$scalar` must
be an `int|float`; unlike `*`, division by a `Matrix` isn't defined, and neither is `int|float / Vector` - vectors have
no multiplicative inverse for a scalar to be divided by.

```php
$v = Vector::fromArray([2, 4, 6]);
$v / 2;  // [1, 2, 3]
```

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the
  Math package
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix side
  of the interaction
