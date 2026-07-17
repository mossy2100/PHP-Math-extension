# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Vector`
(properties, factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md) - this page covers only the
operators the extension adds on top of it.

*Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status.*

`Vector` gets the four basic arithmetic operators. It does not get `**` - the package has no `pow()` method for
`Vector` to map an operator to, and no design for one yet - nor any comparison operators, since element-wise
ordering has no single natural definition.

---

## Binary Arithmetic Operators

Each operator below is equivalent to calling the same-named method directly, and accepts the same operand type(s)
and throws the same exceptions - see the linked method docs for full details.

### + (add)

```php
$sum = $v1 + $v2;
```

Equivalent to [`$v1->add($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#add). Element-wise;
both vectors must be the same size.

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v2 = Vector::fromArray([4, 5, 6]);
$sum = $v1 + $v2;  // [5, 7, 9]
```

### - (subtract)

```php
$diff = $v1 - $v2;
```

Equivalent to [`$v1->sub($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#sub). Element-wise;
both vectors must be the same size.

```php
$v1 = Vector::fromArray([4, 5, 6]);
$v2 = Vector::fromArray([1, 2, 3]);
$diff = $v1 - $v2;  // [3, 3, 3]
```

### * (multiply)

```php
$result = $v * $other;
```

Equivalent to [`$v->mul($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul). `$other` may
be a `float` (scalar multiplication) or a `Matrix` (this vector treated as a 1×n row matrix).

```php
$v = Vector::fromArray([1, 2, 3]);
$result = $v * 2;  // [2, 4, 6]
```

### / (divide)

```php
$result = $v / $scalar;
```

Equivalent to [`$v->div($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#div). `$scalar`
must be a `float`; unlike `*`, division by a `Matrix` isn't defined.

```php
$v = Vector::fromArray([2, 4, 6]);
$result = $v / 2;  // [1, 2, 3]
```

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the
  Math package
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix
  side of the interaction
