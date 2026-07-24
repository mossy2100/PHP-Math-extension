# Complex Operators

Operator overloading for `OceanMoon\Math\Complex`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Complex`
(properties, factory methods, conversion, comparison, all the trigonometric/transcendental methods, etc.), see the
package's own [Complex documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md) - this page
covers only the operators the extension adds on top of it.

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$z;
```

Returns an equal `Complex` with the same real and imaginary parts. There's no package method this maps to - value
identity needs no explicit method call in ordinary PHP code. PHP has no dedicated opcode for unary `+`/`-`; the compiler
lowers `+$z` to `$z * 1`, which the extension handles via a `Complex * int` operation, as documented below.

**Example:**

```php
$z = new Complex(3, 4);
$copy = +$z;  // 3 + 4i
```

---

### - (negate)

```php
$negated = -$z;
```

Equivalent to `$z->neg()`.

**Example:**

```php
$z = new Complex(3, 4);
$negated = -$z;  // -3 - 4i
```

Equivalence table for the unary `-` operator, where `$z` is a `Complex`.

| Operation | Equivalent to |
| --------- | ------------- |
| `-$z`     | `$z->neg()`   |

---

### ~ (conjugate)

```php
$conjugate = ~$z;
```

Equivalent to `$z->conj()`. There's no natural PHP operator for "complex conjugate", so `~` (usually bitwise NOT) is
repurposed for it, being the closest visual/conceptual fit among the operators PHP allows a `do_operation` handler to
intercept for a single operand.

This notation originated with GCC (GNU Compiler Collection), which extends standard C the same way, giving `~`
complex-conjugate behavior when applied to its built-in `_Complex` types (alongside its traditional bitwise-NOT meaning
for integers).

**Example:**

```php
$z = new Complex(3, 4);
$result = ~$z;  // 3 - 4i
```

Equivalence table for the `~` operator, where `$z` is a `Complex`.

| Operation | Equivalent to |
| --------- | ------------- |
| `~$z`     | `$z->conj()`  |

---

## Binary Arithmetic Operators

Each operator accepts a `Complex` or `int|float` operand on either side. Each is equivalent to calling the same-named
method directly, and throws the same exceptions, under the same conditions, as that method - see the linked method docs
for specifics.

This is a genuine flexibility gain over the fluent method API, not just shorter syntax. `$z->add($x)` is naturally
`Complex + int|float` - the method is called _on_ a `Complex`, so that side is fixed. There's no equally natural way to
write `int|float + Complex` with the fluent API alone; you'd need to promote the scalar first, e.g.
`(new Complex($x))->add($z)`. The `+` operator accepts either order directly, so `$x + $z` and `$z + $x` both just work.

This matters most for the non-commutative operators (`-`, `/`, `**`), where `$x - $z` and `$z - $x` are genuinely
different values and the operator lets you express whichever order the math actually calls for, without a scalar
promotion detour.

Operators also come with precedence and associativity rules that PHP itself resolves, whereas a fluent chain of method
calls has no notion of precedence at all - it only ever evaluates in the order you nest the calls, so you have to work
out the correct grouping yourself and encode it directly as nested `->` calls. `$z + $w * $c` reads the same as ordinary
arithmetic and PHP evaluates `$w * $c` first automatically; the fluent equivalent, `$z->add($w->mul($c))`, requires you
to have already done that grouping in your head before writing a single method call. This applies equally to every class
this extension adds operators to, so see [Operator Precedence](../README.md#operator-precedence) in the main README for
the shared precedence table and PHP manual link.

---

### + (add)

```php
$sum = $z + $w;
```

Equivalent to [`$z->add($w)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#add).

Forms:

1. `Complex + Complex`.
2. `Complex + int|float`.
3. `int|float + Complex`. Addition is commutative, so this gives the same result as #2.

**Example:**

```php
$z = new Complex(3, 4);
$w = new Complex(1, 2);
$z + $w;    // 4 + 6i    (Complex + Complex)
$z + 2;     // 5 + 4i    (Complex + int)
2 + $z;     // 5 + 4i    (int + Complex - same result, addition is commutative)
$z + 2.5;   // 5.5 + 4i  (Complex + float)
2.5 + $z;   // 5.5 + 4i  (float + Complex - same result, addition is commutative)
```

Equivalence table for the `+` operator, where `$z` and `$w` are `Complex` values, and `$x` is an `int` or `float`.

| Operation | Equivalent to              | Also equivalent |
| --------- | --------------------------- | ---------------- |
| `$z + $w` | `$z->add($w)`              |                  |
| `$z + $x` | `$z->add($x)`              |                  |
| `$x + $z` | `new Complex($x)->add($z)` | `$z->add($x)`    |

---

### - (subtract)

```php
$diff = $z - $w;
```

Equivalent to [`$z->sub($w)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#sub).

Forms:

1. `Complex - Complex`.
2. `Complex - int|float`.
3. `int|float - Complex`. Subtraction isn't commutative, so this is a distinct case from #2 - the scalar is promoted to
   a `Complex` with a zero imaginary part, then subtracted normally.

**Example:**

```php
$z = new Complex(5, 7);
$w = new Complex(2, 3);
$z - $w;    // 3 + 4i     (Complex - Complex)
$z - 2;     // 3 + 7i     (Complex - int)
2 - $z;     // -3 - 7i    (int - Complex)
$z - 2.5;   // 2.5 + 7i   (Complex - float)
2.5 - $z;   // -2.5 - 7i  (float - Complex)
```

Equivalence table for the `-` operator, where `$z` and `$w` are `Complex` values, and `$x` is an `int` or `float`.

| Operation | Equivalent to              | Also equivalent |
| --------- | --------------------------- | ---------------- |
| `$z - $w` | `$z->sub($w)`              |                  |
| `$z - $x` | `$z->sub($x)`              |                  |
| `$x - $z` | `new Complex($x)->sub($z)` | `-$z->sub($x)`   |

---

### \* (multiply)

```php
$product = $z * $w;
```

Equivalent to [`$z->mul($w)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#mul).

Forms:

1. `Complex * Complex`.
2. `Complex * int|float`.
3. `int|float * Complex`. Multiplication is commutative, so this gives the same result as #2.

**Example:**

```php
$z = new Complex(1, 2);
$w = new Complex(3, 4);
$z * $w;    // -5 + 10i   (Complex * Complex)
$z * 2;     // 2 + 4i     (Complex * int)
2 * $z;     // 2 + 4i     (int * Complex - same result, multiplication is commutative)
$z * 2.5;   // 2.5 + 5i   (Complex * float)
2.5 * $z;   // 2.5 + 5i   (float * Complex - same result, multiplication is commutative)
```

Equivalence table for the `*` operator, where `$z` and `$w` are `Complex` values, and `$x` is an `int` or `float`.

| Operation | Equivalent to              | Also equivalent |
| --------- | --------------------------- | ---------------- |
| `$z * $w` | `$z->mul($w)`              |                  |
| `$z * $x` | `$z->mul($x)`              |                  |
| `$x * $z` | `new Complex($x)->mul($z)` | `$z->mul($x)`    |

---

### / (divide)

```php
$quotient = $z / $w;
```

Equivalent to [`$z->div($w)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#div).

Forms:

1. `Complex / Complex`.
2. `Complex / int|float`.
3. `int|float / Complex`. Division isn't commutative, so this is a distinct case from #2 - the scalar is promoted to a
   `Complex` with a zero imaginary part, then divided normally. This also gives a way to compute a multiplicative
   inverse - see "Inverting with `1 / z` or `z ** -1`" below.

**Example:**

```php
$z = new Complex(6, 8);
$w = new Complex(3, 4);
$z / $w;    // 2               (Complex / Complex; real only - the imaginary parts cancel here)
$z / 2;     // 3 + 4i          (Complex / int)
1 / $w;     // 0.12 - 0.16i    (int / Complex - the inverse of $w)
$z / 2.5;   // 2.4 + 3.2i      (Complex / float)
2.5 / $z;   // 0.15 - 0.2i     (float / Complex)
```

Equivalence table for the `/` operator, where `$z` and `$w` are `Complex` values, and `$x` is an `int` or `float`.

| Operation | Equivalent to              |
| --------- | -------------------------- |
| `$z / $w` | `$z->div($w)`              |
| `$z / $x` | `$z->div($x)`              |
| `$x / $z` | `new Complex($x)->div($z)` |

---

### \*\* (power)

```php
$result = $z ** $w;
```

Equivalent to [`$z->pow($w)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#pow).

Forms:

1. `Complex ** Complex`.
2. `Complex ** int|float`.
3. `int|float ** Complex`. Not commutative with #2 in general - the scalar is promoted to a `Complex` with a zero
   imaginary part, then raised to the (possibly complex) power on the right, using the same `pow()` method (which
   already accepts a `Complex` exponent for exactly this reason).

**Example:**

```php
$z = new Complex(3, 4);
$w = new Complex(1, 1);
$z ** $w;   // -1.63 + 1.12i   (Complex ** Complex)
$z ** 2;    // -7 + 24i        (Complex ** int)
2 ** $z;    // -7.46 + 2.89i   (int ** Complex)
$z ** 0.5;  // 2 + i           (Complex ** float; principal square root)
2.5 ** $z;  // -13.53 - 7.81i  (float ** Complex)
```

Equivalence table for the `**` operator, where `$z` and `$w` are `Complex` values, and `$x` is an `int` or `float`.

| Operation  | Equivalent to              |
| ---------- | -------------------------- |
| `$z ** $w` | `$z->pow($w)`              |
| `$z ** $x` | `$z->pow($x)`              |
| `$x ** $z` | `new Complex($x)->pow($z)` |

---

## Inverting with `1 / z` or `z ** -1`

The fluent API provided by the package includes an `inv()` method, which inverts a `Complex`. The operators give you a
couple of alternative ways to achieve the same thing.

Since `int|float / Complex` is supported (see `/` above), `1 / $z` computes the same multiplicative inverse as
[`$z->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#inv). `$z ** -1` gives the same result via
`pow()` too, since raising to the power -1 is the mathematical definition of a multiplicative inverse. All three throw
`ArithmeticException` if `$z` is zero, matching `inv()`.

**Example:**

```php
$z = new Complex(3, 4);
$z->inv();  // 0.12 - 0.16i
1 / $z;     // 0.12 - 0.16i, same result
$z ** -1;   // 0.12 - 0.16i, same result
```

---

## See Also

- **[Complex](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md)** - the full class documentation in the
  Math package
- **[Rational Operators](Rational.md)** - `Complex` deliberately gets no comparison operators (`==`, `!=`, `<`, etc.)
  since it has no natural ordering; `Rational` does, and gets the full set
