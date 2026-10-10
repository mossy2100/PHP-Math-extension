# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Vector` (properties,
factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md) - this page covers only the operators the extension adds on top of it.

`Vector` gets the four basic arithmetic operators: `+`, `-`, `*`, and `/`. It does not get (nor require) `**` as raising
a vector to a power is not a valid mathematical operation. The extension adds no comparison operators either: there's no
natural sort order for a vector, so no `compare` handler is installed - PHP's own default object comparison still
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

Each operator is equivalent to calling the same-named method directly, and throws the same exceptions, under the same
conditions, as that method - see the linked method docs for specifics.

---

### + (add)

```php
$sum = $v1 + $v2;  // Vector + Vector → Vector
```

Element-wise; both vectors must be the same count. No other form is supported - there's no scalar addition for `Vector`.

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
$result = $v * $x;  // Vector * int|float → Vector
$result = $x * $v;  // int|float * Vector → Vector
$result = $v * $m;  // Vector * Matrix → Vector
```

Multiplying by a scalar is element-wise, and commutative, so reversing the operands gives the same result.

For `Vector * Matrix`, the `Vector` is treated as a 1×n row matrix and multiplied by the `Matrix` using standard matrix
multiplication. The number of rows in the `Matrix` must equal the `Vector`'s count. The `Matrix * Vector` operation is
documented in [Matrix Operators](Matrix.md#-multiply).

Unsupported form:

1. `Vector * Vector`. This form is unsupported because there are several ways to multiply 2 vectors. Instead, call the
   relevant method:
   - [`dot()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#dot)
   - [`cross()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#cross)
   - [`outer()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#outer)
   - [`hadamardMul()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamardmul)

**Examples:**

```php
$v = Vector::fromArray([1, 2, 3]);
$v * 2;     // [2, 4, 6]
2 * $v;     // [2, 4, 6]
$v * 2.5;   // [2.5, 5, 7.5]
2.5 * $v;   // [2.5, 5, 7.5]

$m = Matrix::fromArray([[1, 0], [0, 1]]);
$v = Vector::fromArray([1, 2]);
$v * $m;    // [1, 2]  ($v is treated as a 1×2 row matrix)
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `$v * $x` | `$v->mul($x)` |
| `$x * $v` | `$v->mul($x)` |
| `$v * $m` | `$v->mul($m)` |

---

### / (divide)

```php
$result = $v / $x;  // Vector / int|float → Vector
$result = $x / $v;  // int|float / Vector → Vector
```

Both forms are element-wise, and not commutative. `int|float / Vector` divides the scalar by each element of the
`Vector` in turn. There's no equivalent method for it, but the equivalence table shows how to get the same result. It
throws `ArithmeticException` if any of the vector's elements equal zero, matching `reciprocal()`.

Unsupported forms:

1. `Vector / Vector`. Vectors have no multiplicative inverse for a Vector to be multiplied by. If you want element-wise
   (Hadamard) division, call [`$v1->hadamardDiv($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamarddiv).
2. `Vector / Matrix`. Division of a vector by a matrix isn't a typical operation. If the intent is to multiply a vector
   by a matrix's inverse (i.e. compute v ⨉ M⁻¹) it can be achieved with `$v * $m ** -1` or, equivalently,
   `$v * $m->inv()`. This requires the matrix to be square, in addition to having the same number of columns as the
   vector has elements.

**Examples:**

```php
$v = Vector::fromArray([2, 4, 6]);
$v / 2;     // [1, 2, 3]
$v / 2.5;   // [0.8, 1.6, 2.4]

$v = Vector::fromArray([1, 2, 4]);
8 / $v;     // [8, 4, 2]
```

**Equivalence table:**

| Operation | Equivalent to                                          |
| --------- | ------------------------------------------------------ |
| `$v / $x` | `$v->div($x)`                                          |
| `$x / $v` | `$x * $v->reciprocal()` or `$v->reciprocal()->mul($x)` |

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the Math package.
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix side of the
  interaction.
- **[Comparison Operators](Comparison_Operators.md)** - how comparison works across all four classes; `Vector` gets no extension-provided
  comparison handler, only PHP's own default object comparison.
