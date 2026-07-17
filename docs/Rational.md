# Rational Operators

Operator overloading for `OceanMoon\Math\Rational`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Rational`
(properties, factory methods, conversion, `compare()`/`approxCompare()`, rounding, etc.), see the package's own
[Rational documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md) - this page covers only the
operators the extension adds on top of it.

_Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status._

`Rational` is the only one of the four classes to get comparison operators, since - unlike `Complex`, `Vector`, and
`Matrix` - it has a genuine natural ordering.

---

## Unary Arithmetic Operators

### + (identity)

```php
$copy = +$r;
```

Returns an equal `Rational` with the same numerator and denominator. There's no package method this maps to - value
identity needs no explicit method call in ordinary PHP code - but the extension implements the `ZEND_UNARY_PLUS` handler
anyway, for parity with PHP's built-in `+$x` on `int`/`float`.

**Example:**

```php
$r = new Rational(2, 3);
$copy = +$r;  // 2/3
```

### - (negate)

```php
$negated = -$r;
```

Equivalent to [`$r->neg()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#neg).

**Example:**

```php
$r = new Rational(2, 3);
$negated = -$r;  // -2/3
```

---

## Binary Arithmetic Operators

`+`, `-`, `*`, and `/` each accept a `Rational`, `int`, or `float` operand on either side. Each is equivalent to calling
the same-named method directly, and throws the same exceptions, under the same conditions, as that method - see the
linked method docs for specifics.

### + (add)

```php
$sum = $r1 + $r2;
```

Equivalent to [`$r1->add($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#add). There are 2 ways
this operator can be used:

1. `Rational + Rational`.
2. `Rational + int|float` or `int|float + Rational`. Addition is commutative, so both give the same result.

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 + $r2;  // 5/6    (Rational + Rational)
$r1 + 1;    // 3/2    (Rational + int|float)
1 + $r1;    // 3/2    (int|float + Rational - same result, addition is commutative)
```

### - (subtract)

```php
$diff = $r1 - $r2;
```

Equivalent to [`$r1->sub($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#sub). There are 3 ways
this operator can be used:

1. `Rational - Rational`.
2. `Rational - int|float`.
3. `int|float - Rational`. Subtraction isn't commutative, so this is a distinct case from #2 - the scalar is promoted to
   a `Rational` first, then subtracted normally.

```php
$r1 = new Rational(3, 4);
$r2 = new Rational(1, 4);
$r1 - $r2;  // 1/2    (Rational - Rational)
$r1 - 1;    // -1/4   (Rational - int|float)
1 - $r1;    // 1/4    (int|float - Rational)
```

### \* (multiply)

```php
$product = $r1 * $r2;
```

Equivalent to [`$r1->mul($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#mul). There are 2 ways
this operator can be used:

1. `Rational * Rational`.
2. `Rational * int|float` or `int|float * Rational`. Multiplication is commutative, so both give the same result.

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 * $r2;  // 1/2    (Rational * Rational)
$r1 * 3;    // 2      (Rational * int|float)
3 * $r1;    // 2      (int|float * Rational - same result, multiplication is commutative)
```

### / (divide)

```php
$quotient = $r1 / $r2;
```

Equivalent to [`$r1->div($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#div). There are 3 ways
this operator can be used:

1. `Rational / Rational`.
2. `Rational / int|float`.
3. `int|float / Rational`. Division isn't commutative, so this is a distinct case from #2 - the scalar is promoted to a
   `Rational` first, then divided normally. This also gives a way to compute a multiplicative inverse - see "Inverting
   with `1 / r` or `r ** -1`" below.

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 / $r2;  // 8/9    (Rational / Rational)
$r1 / 2;    // 1/3    (Rational / int|float)
1 / $r1;    // 3/2    (int|float / Rational - the inverse of $r1)
```

### \*\* (power)

```php
$result = $r ** $exponent;
```

Equivalent to [`$r->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#pow). Unlike the
other three arithmetic operators, this one only works as `Rational ** int` - the operand must be an `int` (matching
`pow()`'s signature). There's no `int ** Rational` case: a number raised to a fractional power is mathematically
well-defined (e.g. `2 ** (1/2)` is `sqrt(2)`), but the result generally isn't itself a rational number, so it can't be
represented exactly in this exact-arithmetic domain - which is also why `pow()` doesn't accept a `Rational` or `float`
exponent in the first place (unlike `Complex::pow()`, where a complex exponent always yields another complex number).

```php
$r = new Rational(2, 3);
$r ** 2;   // 4/9
$r ** -2;  // 9/4  (negative exponent = reciprocal)
```

---

## Inverting with `1 / r` or `r ** -1`

Since `int|float / Rational` is supported (see `/` above), `1 / $r` computes the same multiplicative inverse as
[`$r->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#inv). `$r ** -1` gives the same result
too, since raising to the power -1 is the mathematical definition of a multiplicative inverse. All three throw
`ArithmeticException` if `$r`'s numerator is zero, matching `inv()`.

```php
$r = new Rational(3, 4);
$r->inv();  // 4/3
1 / $r;     // 4/3, same result
$r ** -1;   // 4/3, same result
```

---

## Comparison Operators

A native `compare` object handler backs `<`, `<=`, `>`, `>=`, and `<=>` together - PHP has no way to wire up a subset of
these independently. The handler is equivalent to
[`compare()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#compare): it accepts `Rational`, `int`,
or `float` operands and throws the same exceptions for anything else. A scalar operand can be on either side (`$r < 5`
and `5 < $r` are both valid and give the mathematically correct, appropriately-flipped result) - the handler works
either way, since PHP calls it whichever side the `Rational` instance is on.

### <=> (spaceship)

```php
$result = $r1 <=> $r2;  // -1, 0, or 1
```

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 <=> $r2;   // 1  (1/2 > 1/3)
$r1 <=> 0.5;   // 0  (1/2 == 0.5 exactly)
```

### <, <=, >, >=

```php
$r1 < $r2;
$r1 <= $r2;
$r1 > $r2;
$r1 >= $r2;
```

Ordering comparisons, all derived from the same `<=>` handler.

```php
$r1 = new Rational(1, 3);
$r2 = new Rational(1, 2);

var_dump($r1 < $r2);   // true
var_dump($r2 >= $r1);  // true
var_dump($r1 < 0.5);   // true (1/3 < 0.5)
```

### == and !=

```php
$r1 == $r2;
$r1 != $r2;
```

**A note on style:** modern PHP coding standards generally favor `===`/`!==` over `==`/`!=`, to avoid PHP's
loose-comparison type-juggling pitfalls, and this project's own conventions follow that guidance. `==`/`!=` are
documented here anyway because PHP wires all six comparison operators (`<`, `<=`, `>`, `>=`, `<=>`, and `==`/`!=`) to
the same `compare` object handler - once `<=>` exists for ordering, `==`/`!=` come along as an unavoidable side effect,
using exact value comparison (not the default identity-only comparison PHP objects get without a custom handler), rather
than something intentionally designed in. Prefer `equal()`/`approxCompare()` in new code.

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(2, 4);

var_dump($r1 == $r2);   // true  (1/2 == 2/4 exactly, not identity)
var_dump($r1 != $r2);   // false
```

---

## See Also

- **[Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)** - the full class documentation in the
  Math package
- **[Complex Operators](Complex.md)** - `Complex` deliberately gets no comparison operators, since it has no natural
  ordering
