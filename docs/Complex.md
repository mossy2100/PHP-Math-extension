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
identity needs no explicit method call in ordinary PHP code - but the extension implements the `ZEND_UNARY_PLUS` handler
anyway, for parity with PHP's built-in `+$x` on `int`/`float`.

**Example:**

```php
$z = new Complex(3, 4);
$copy = +$z;  // 3 + 4i
```

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
out the correct grouping yourself and encode it directly as nested `->` calls. `$z1 + $z2 * $z3` reads the same as
ordinary arithmetic and PHP evaluates `$z2 * $z3` first automatically; the fluent equivalent, `$z1->add($z2->mul($z3))`,
requires you to have already done that grouping in your head before writing a single method call. This applies equally
to every class this extension adds operators to, so see [Operator Precedence](../README.md#operator-precedence) in the
main README for the shared precedence table and PHP manual link.

### + (add)

```php
$sum = $z1 + $z2;
```

Equivalent to [`$z1->add($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#add). There are 2 ways
this operator can be used:

Forms:

1. `Complex + Complex`.
2. `Complex + int|float`.
3. `int|float + Complex`. Addition is commutative, so this gives the same result as #2.

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(1, 2);
$z1 + $z2;  // 4 + 6i   (Complex + Complex)
$z1 + 2;    // 5 + 4i   (Complex + int|float)
2 + $z1;    // 5 + 4i   (int|float + Complex - same result, addition is commutative)
```

### - (subtract)

```php
$diff = $z1 - $z2;
```

Equivalent to [`$z1->sub($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#sub).

Forms:

1. `Complex - Complex`.
2. `Complex - int|float`.
3. `int|float - Complex`. Subtraction isn't commutative, so this is a distinct case from #2 - the scalar is promoted to
   a `Complex` with a zero imaginary part, then subtracted normally.

```php
$z1 = new Complex(5, 7);
$z2 = new Complex(2, 3);
$z1 - $z2;  // 3 + 4i   (Complex - Complex)
$z1 - 2;    // 3 + 7i   (Complex - int|float)
2 - $z1;    // -3 - 7i  (int|float - Complex)
```

### \* (multiply)

```php
$product = $z1 * $z2;
```

Equivalent to [`$z1->mul($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#mul).

Forms:

1. `Complex * Complex`.
2. `Complex * int|float`.
3. `int|float * Complex`. Multiplication is commutative, so this give the same result as #2.

```php
$z1 = new Complex(1, 2);
$z2 = new Complex(3, 4);
$z1 * $z2;  // -5 + 10i (Complex * Complex)
$z1 * 2;    // 2 + 4i   (Complex * int|float)
2 * $z1;    // 2 + 4i   (int|float * Complex - same result, multiplication is commutative)
```

### / (divide)

```php
$quotient = $z1 / $z2;
```

Equivalent to [`$z1->div($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#div).

Forms:

1. `Complex / Complex`.
2. `Complex / int|float`.
3. `int|float / Complex`. Division isn't commutative, so this is a distinct case from #2 - the scalar is promoted to a
   `Complex` with a zero imaginary part, then divided normally. This also gives a way to compute a multiplicative
   inverse - see "Inverting with `1 / z` or `z ** -1`" below.

```php
$z1 = new Complex(6, 8);
$z2 = new Complex(3, 4);
$z1 / $z2;  // 2 (real only - the imaginary parts cancel here)
$z1 / 2;    // 3 + 4i   (Complex / int|float)
1 / $z2;    // 0.12 - 0.16i (int|float / Complex - the inverse of $z2)
```

### \*\* (power)

```php
$result = $z ** $other;
```

Equivalent to [`$z->pow($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#pow).

Forms:

1. `Complex ** Complex`.
2. `Complex ** int|float`.
3. `int|float ** Complex`. Not commutative with #2 in general - the scalar is promoted to a `Complex` with a zero
   imaginary part, then raised to the (possibly complex) power on the right, using the same `pow()` method (which
   already accepts a `Complex` exponent for exactly this reason).

```php
$z = new Complex(3, 4);
$z ** 2;  // -7 + 24i (Complex ** int|float)
2 ** $z;  // -7.46 + 2.89i (int|float ** Complex)
```

---

## Inverting with `1 / z` or `z ** -1`

The fluent API provided by the package includes an `inv()` method, which inverts a `Complex`. The operators give you a
couple of alternative ways to achieve the same thing.

Since `int|float / Complex` is supported (see `/` above), `1 / $z` computes the same multiplicative inverse as
[`$z->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#inv). `$z ** -1` gives the same result via
`pow()` too, since raising to the power -1 is the mathematical definition of a multiplicative inverse. All three throw
`ArithmeticException` if `$z` is zero, matching `inv()`.

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
