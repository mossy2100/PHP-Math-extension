# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Vector` (properties,
factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md) - this page covers only the operators the extension adds on top of it.

`Vector` gets the four basic arithmetic operators: `+`, `-`, `*`, and `/`. It does not get (nor require) `**` as raising
a vector to a power is not a valid mathematical operation. No comparison operators are provided either, since vectors do
not have a natural sort order.

---

## Unary Arithmetic Operators

### + (clone)

```php
$copy = +$v;
```

Equivalent to `clone $v`. Returns a new `Vector` with the same elements. Since `Vector` is mutable, this matters:
mutating the copy never affects the original, and vice versa.

**Example:**

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v2 = +$v1;  // [1, 2, 3]
```

Equivalence table for the unary `+` operator, where `$v` is a `Vector`.

| Operation | Equivalent to |
| --------- | ------------- |
| `+$v`     | `clone $v`    |

---

### - (negate)

```php
$negated = -$v;
```

Equivalent to [`$v->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#neg). Each element of the
`Vector` is negated.

**Example:**

```php
$v = Vector::fromArray([1, -2, 3]);
$negated = -$v;  // [-1, 2, -3]
```

Equivalence table for the unary `-` operator, where `$v` is a `Vector`.

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
$sum = $v1 + $v2;
```

Equivalent to [`$v1->add($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#add). Element-wise; both
vectors must be the same count.

Forms:

1. `Vector + Vector`. No other form is supported - there's no scalar addition for `Vector`.

**Example:**

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v2 = Vector::fromArray([4, 5, 6]);
$v1 + $v2;  // [5, 7, 9]  (Vector + Vector)
```

Equivalence table for the `+` operator, where `$v1` and `$v2` are `Vector` values.

| Operation | Equivalent to |
| --------- | ------------- |
| `$v1 + $v2` | `$v1->add($v2)` |

---

### - (subtract)

```php
$diff = $v1 - $v2;
```

Equivalent to [`$v1->sub($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#sub). Element-wise; both
vectors must be the same count.

Forms:

1. `Vector - Vector`. As with `+`, no other form is supported.

**Example:**

```php
$v1 = Vector::fromArray([4, 5, 6]);
$v2 = Vector::fromArray([1, 2, 3]);
$v1 - $v2;  // [3, 3, 3]  (Vector - Vector)
```

Equivalence table for the `-` operator, where `$v1` and `$v2` are `Vector` values.

| Operation | Equivalent to |
| --------- | ------------- |
| `$v1 - $v2` | `$v1->sub($v2)` |

---

### \* (multiply)

```php
$result = $v1 * $other;
```

Forms:

1. `Vector * int|float`. Scalar multiplication, element-wise - equivalent to
   [`$v1->mul($x)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul).
2. `int|float * Vector`. Multiplication of vector by a scalar is commutative, so this gives the same result as #1.
3. `Vector * Matrix`. This `Vector` is treated as a 1×n row matrix, multiplied by a `Matrix` using standard matrix
   multiplication. The number of rows in the `Matrix` must equal the `Vector`'s count. This operation is equivalent to
   [`$v1->mul($matA)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul).

The `Matrix * Vector` operation is documented in [Matrix Operators](Matrix.md#-multiply).

Unsupported form:

1. `Vector * Vector`. This form is unsupported because there are several ways to multiply 2 vectors. Instead, call the
   relevant method:
   - [`dot()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#dot)
   - [`cross()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#cross)
   - [`outer()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#outer)
   - [`hadamardMul()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamardmul)

**Example:**

```php
$v1 = Vector::fromArray([1, 2, 3]);
$v1 * 2;    // [2, 4, 6]      (Vector * int)
2 * $v1;    // [2, 4, 6]      (int * Vector - same result, scalar multiplication is commutative)
$v1 * 2.5;  // [2.5, 5, 7.5]  (Vector * float)
2.5 * $v1;  // [2.5, 5, 7.5]  (float * Vector - same result, scalar multiplication is commutative)

$matA = Matrix::fromArray([[1, 0], [0, 1]]);
$v2 = Vector::fromArray([1, 2]);
$v2 * $matA;  // [1, 2]  (Vector * Matrix - $v2 treated as a 1x2 row matrix)
```

Equivalence table for the `*` operator, where `$v` is a `Vector`, `$x` is an `int` or `float`, and `$matA` is a `Matrix`.

| Operation    | Equivalent to    |
| ------------ | ---------------- |
| `$v * $x`    | `$v->mul($x)`    |
| `$x * $v`    | `$v->mul($x)`    |
| `$v * $matA` | `$v->mul($matA)` |

---

### / (divide)

```php
$result = $v1 / $x;
```

Performs element-wise scalar division.

Forms:

1. `Vector / int|float`. Equivalent to [`$v1->div($x)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#div).
2. `int|float / Vector`. Not commutative with `Vector / int|float`. Element-wise, divides the scalar by each
   element of the `Vector` in turn. There's no equivalent method, but the same result can be achieved by [`$x * $v->reciprocal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#reciprocal), or, equivalently, `$v->reciprocal()->mul($x)`. Throws `ArithmeticException` if any of the vector's elements equal zero, matching `reciprocal()`.

Unsupported forms:

1. `Vector / Vector`. Vectors have no multiplicative inverse for a Vector to be multiplied by. If you want element-wise
   (Hadamard) division, call [`$v1->hadamardDiv($v2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamarddiv).
2. `Vector / Matrix`. Division of a vector by a matrix isn't a typical operation. If the intent is to multiply a vector by a matrix's inverse (i.e. compute v ⨉ M⁻¹) it can be achieved with `$vec * $mat ** -1` or, equivalently, `$vec * $mat->inv()`. This requires the matrix to be square, in addition to having the same number of  columns as the vector has elements.

**Example:**

```php
$v1 = Vector::fromArray([2, 4, 6]);
$v1 / 2;    // [1, 2, 3]        (Vector / int)
$v1 / 2.5;  // [0.8, 1.6, 2.4]  (Vector / float)

$v2 = Vector::fromArray([1, 2, 4]);
8 / $v2;    // [8, 4, 2]        (int / Vector - element-wise)
```

Equivalence table for the `/` operator, where `$v` is a `Vector`, and `$x` is an `int` or `float`.

| Operation | Equivalent to           |
| --------- | ----------------------- |
| `$v / $x` | `$v->div($x)`           |
| `$x / $v` | `$x * $v->reciprocal()` |

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the
  Math package
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix side
  of the interaction
