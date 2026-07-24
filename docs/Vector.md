# Vector Operators

Operator overloading for `OceanMoon\Math\Vector`, available only when this extension is loaded - plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Vector` (properties,
factory methods, conversion, comparison, dot/cross product, `ArrayAccess`, etc.), see the package's own
[Vector documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md) - this page covers only the
operators the extension adds on top of it.

`Vector` gets the four basic arithmetic operators: `+`, `-`, `*`, and `/`. It does not get (nor require) `**` as raising
a vector to a power is not a valid mathematical operation. No comparison operators are provided either, since vectors do
not have a natural sort order.

---

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$v;
```

Returns a new `Vector` with the same elements. There's no package method this maps to; value identity needs no explicit
method call in ordinary PHP code. PHP has no dedicated opcode for unary `+`/`-`; the compiler lowers `+$v` to `$v * 1`,
which the extension handles via the scalar form of `*` (see below). Since `Vector` is mutable, this returns a distinct
copy rather than `$this`, so mutating the result never affects the original - the same rule the package's own arithmetic
methods follow.

**Example:**

```php
$v = Vector::fromArray([1, 2, 3]);
$copy = +$v;  // [1, 2, 3]
```

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
$sum = $v + $u;
```

Equivalent to [`$v->add($u)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#add). Element-wise; both
vectors must be the same size.

Forms:

1. `Vector + Vector`. No other form is supported - there's no scalar addition for `Vector`.

**Example:**

```php
$v = Vector::fromArray([1, 2, 3]);
$u = Vector::fromArray([4, 5, 6]);
$v + $u;  // [5, 7, 9]  (Vector + Vector)
```

Equivalence table for the `+` operator, where `$v` and `$u` are `Vector` values.

| Operation | Equivalent to  |
| --------- | --------------- |
| `$v + $u` | `$v->add($u)`   |

---

### - (subtract)

```php
$diff = $v - $u;
```

Equivalent to [`$v->sub($u)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#sub). Element-wise; both
vectors must be the same size.

Forms:

1. `Vector - Vector`. As with `+`, no other form is supported.

**Example:**

```php
$v = Vector::fromArray([4, 5, 6]);
$u = Vector::fromArray([1, 2, 3]);
$v - $u;  // [3, 3, 3]  (Vector - Vector)
```

Equivalence table for the `-` operator, where `$v` and `$u` are `Vector` values.

| Operation | Equivalent to  |
| --------- | --------------- |
| `$v - $u` | `$v->sub($u)`   |

---

### \* (multiply)

```php
$result = $v * $other;
```

Forms:

1. `Vector * int|float`. Scalar multiplication, element-wise - equivalent to
   [`$v->mul($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul).
2. `int|float * Vector`. Multiplication of vector by a scalar is commutative, so this gives the same result as #1.
3. `Vector * Matrix`. This `Vector` is treated as a 1×n row matrix, multiplied by a `Matrix` using standard matrix
   multiplication. The number of rows in the `Matrix` must equal the `Vector` size. This operation is equivalent to
   [`$vec->mul($mat)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#mul). Not commutative - the
   `Matrix * Vector` operation is a distinct calculation, documented in [Matrix Operators](Matrix.md#-multiply).

Unsupported form:

1. `Vector * Vector`. This form is unsupported because there are several ways to multiply 2 vectors. Instead, call the
   relevant method:
   - [`dot()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#dot)
   - [`cross()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#cross)
   - [`outer()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#outer)
   - [`hadamardMul()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamardmul)

**Example:**

```php
$v = Vector::fromArray([1, 2, 3]);
$v * 2;    // [2, 4, 6]      (Vector * int)
2 * $v;    // [2, 4, 6]      (int * Vector - same result, scalar multiplication is commutative)
$v * 2.5;  // [2.5, 5, 7.5]  (Vector * float)
2.5 * $v;  // [2.5, 5, 7.5]  (float * Vector - same result, scalar multiplication is commutative)

$A = Matrix::fromArray([[1, 0], [0, 1]]);
$u = Vector::fromArray([1, 2]);
$u * $A;  // [1, 2]  (Vector * Matrix - $u treated as a 1x2 row matrix)
```

Equivalence table for the `*` operator, where `$v` is a `Vector`, `$x` is an `int` or `float`, and `$A` is a `Matrix`.

| Operation | Equivalent to      |
| --------- | ------------------ |
| `$v * $x` | `$v->mul($x)`      |
| `$x * $v` | `$v->mul($x)`      |
| `$v * $A` | `$v->mul($A)`      |

---

### / (divide)

```php
$result = $v / $scalar;
```

Equivalent to [`$v->div($scalar)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#div). Performs
element-wise scalar division.

Forms:

1. `Vector / int|float`.

Unsupported forms:

1. `int|float / Vector`. Vectors have no multiplicative inverse for a scalar to be divided by. `x / V` is ambiguous
   between "no defined operation" and "divide `x` by every element of `V`" (element-wise) - for the latter, use
   [`$x * $v->reciprocal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#reciprocal).
2. `Vector / Vector`. Vectors have no multiplicative inverse for a Vector to be multiplied by. If you want element-wise
   (Hadamard) division, use
   [`$v->hadamardDiv($u)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#hadamarddiv).
3. `Vector / Matrix`. Division by a `Matrix` isn't a typical operation, and using a division operator for this purpose
   is technically ambiguous, since v/M could mean M⁻¹\*v or v\*M⁻¹, and these are not necessarily equal because matrix multiplication
   is not commutative. You can multiply a vector by a matrix's inverse (i.e. compute v\*M⁻¹) using methods, e.g.
   `$v * $A->inv()`. This requires the matrix to be square, in addition to having the same number of columns as the
   vector has elements.

**Example:**

```php
$v = Vector::fromArray([2, 4, 6]);
$v / 2;    // [1, 2, 3]        (Vector / int)
$v / 2.5;  // [0.8, 1.6, 2.4]  (Vector / float)
```

Equivalence table for the `/` operator, where `$v` is a `Vector`, and `$x` is an `int` or `float`.

| Operation | Equivalent to |
| --------- | ------------- |
| `$v / $x` | `$v->div($x)` |

---

## See Also

- **[Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)** - the full class documentation in the
  Math package
- **[Matrix Operators](Matrix.md)** - `Vector * Matrix` and `Matrix * Vector` both work; see there for the matrix side
  of the interaction
