# Rational Operators

Operator overloading for `OceanMoon\Math\Rational`, available only when this extension is loaded. Plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Rational`
(properties, factory methods, conversion, `compare()`/`approxCompare()`, rounding, etc.), see the package's own
[Rational documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md). This page covers only the operators the extension adds on top of it.

`Rational` is one of two of the four classes for which comparison operators are provided (the other is `Complex`; see
[Complex Operators](Complex.md#comparison-operators)). Unlike `Complex`, `Rational` has a genuine natural ordering; `Vector` and `Matrix` get no
extension-provided comparison operators, and PHP's default object comparison still applies to both (see
[Comparison Operators](Comparison_Operators.md)).

On this page, `$r`, `$r1` and `$r2` are `Rational` values, `$i` is an `int`, `$f` is a `float`, and `$x` is an `int` or
`float`.

---

## Numeric Type Widening

PHP already widens `int` to `float` when the two mix in an operation (e.g. `3 + 4.0` produces a `float`, even though the
result could technically be represented as an `int`).

`Rational` follows the same pattern, sitting between `int` and `float` in numeric width:

- `Rational` + `int` → the `int` is widened to a `Rational`, and the operation happens on two `Rational` values.
- `Rational` + `float` → the `Rational` is widened to a `float`, and the operation happens on two `float` values.

Unlike PHP, a `Rational` result is never widened to a `float` when it gets too big for an `int`: the operation throws
`OverflowException` instead. To get a `float` result, one operand must be a `float`.

---

## Unary Arithmetic Operators

These are all prefix operators, and work the same way as for `int`/`float`.

---

### + (clone)

```php
$copy = +$r;  // +Rational → Rational
```

Returns an equal `Rational` with the same numerator and denominator.

**Examples:**

```php
$r = new Rational(2, 3);
$copy = +$r;  // 2/3
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `+$r`     | `clone $r`    |

---

### - (negate)

```php
$negated = -$r;  // -Rational → Rational
```

**Examples:**

```php
$r = new Rational(2, 3);
$negated = -$r;  // -2/3
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `-$r`     | `$r->neg()`   |

---

## Binary Arithmetic Operators

`+`, `-`, `*`, `/` and `**` each accept a `Rational`, `int`, or `float` operand on either side, widened as described in
[Numeric Type Widening](#numeric-type-widening). The equivalence table in each section gives the matching method. A form with a `float` operand
has none, since the methods accept only `Rational|int`.

Unlike PHP's own `float` arithmetic, a form with a `float` operand never accepts or returns `±INF` or `NAN`. A
non-finite operand throws `DomainException`, and a result too large to represent throws `OverflowException`.

---

### + (add)

```php
$sum = $r1 + $r2;  // Rational + Rational → Rational
$sum = $r + $i;    // Rational + int → Rational
$sum = $i + $r;    // int + Rational → Rational
$sum = $r + $f;    // Rational + float → float
$sum = $f + $r;    // float + Rational → float
```

Addition is commutative, so reversing the operands gives the same result.

**Throws:**

- `DomainException` if a `float` operand is `±INF` or `NAN`.
- `OverflowException` if the result's numerator or denominator overflows an `int`.
- `TypeError` if the other operand isn't a `Rational`, `int` or `float`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 + $r2;    // 5/6
$r1 + 1;      // 3/2
1 + $r1;      // 3/2
$r1 + 1.23;   // 1.73
1.23 + $r1;   // 1.73
```

**Equivalence table:**

| Operation   | Equivalent to                                |
| ----------- | -------------------------------------------- |
| `$r1 + $r2` | `$r1->add($r2)`                              |
| `$r + $i`   | `$r->add($i)`                                |
| `$i + $r`   | `new Rational($i)->add($r)` or `$r->add($i)` |
| `$r + $f`   | `$r->toFloat() + $f`                         |
| `$f + $r`   | `$f + $r->toFloat()`                         |

---

### - (subtract)

```php
$diff = $r1 - $r2;  // Rational - Rational → Rational
$diff = $r - $i;    // Rational - int → Rational
$diff = $i - $r;    // int - Rational → Rational
$diff = $r - $f;    // Rational - float → float
$diff = $f - $r;    // float - Rational → float
```

Subtraction isn't commutative, so `int|float - Rational` is a distinct case from `Rational - int|float`.

**Throws:**

- `DomainException` if a `float` operand is `±INF` or `NAN`.
- `OverflowException` if the result's numerator or denominator overflows an `int`.
- `TypeError` if the other operand isn't a `Rational`, `int` or `float`.

**Examples:**

```php
$r1 = new Rational(3, 4);
$r2 = new Rational(1, 4);
$r1 - $r2;    // 1/2
$r1 - 1;      // -1/4
1 - $r1;      // 1/4
$r1 - 0.25;   // 0.5
0.25 - $r1;   // -0.5
```

**Equivalence table:**

| Operation   | Equivalent to                                 |
| ----------- | --------------------------------------------- |
| `$r1 - $r2` | `$r1->sub($r2)`                               |
| `$r - $i`   | `$r->sub($i)`                                 |
| `$i - $r`   | `new Rational($i)->sub($r)` or `-$r->sub($i)` |
| `$r - $f`   | `$r->toFloat() - $f`                          |
| `$f - $r`   | `$f - $r->toFloat()`                          |

---

### \* (multiply)

```php
$product = $r1 * $r2;  // Rational * Rational → Rational
$product = $r * $i;    // Rational * int → Rational
$product = $i * $r;    // int * Rational → Rational
$product = $r * $f;    // Rational * float → float
$product = $f * $r;    // float * Rational → float
```

Multiplication is commutative, so reversing the operands gives the same result.

**Throws:**

- `DomainException` if a `float` operand is `±INF` or `NAN`.
- `OverflowException` if the result's numerator or denominator overflows an `int`, or a `float` result is too large to
  represent.
- `TypeError` if the other operand isn't a `Rational`, `int` or `float`.

**Examples:**

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 * $r2;    // 1/2
$r1 * 3;      // 2
3 * $r1;      // 2
$r1 * 0.75;   // 0.5
0.75 * $r1;   // 0.5
```

**Equivalence table:**

| Operation   | Equivalent to                                |
| ----------- | -------------------------------------------- |
| `$r1 * $r2` | `$r1->mul($r2)`                              |
| `$r * $i`   | `$r->mul($i)`                                |
| `$i * $r`   | `new Rational($i)->mul($r)` or `$r->mul($i)` |
| `$r * $f`   | `$r->toFloat() * $f`                         |
| `$f * $r`   | `$f * $r->toFloat()`                         |

---

### / (divide)

```php
$quotient = $r1 / $r2;  // Rational / Rational → Rational
$quotient = $r / $i;    // Rational / int → Rational
$quotient = $i / $r;    // int / Rational → Rational
$quotient = $r / $f;    // Rational / float → float
$quotient = $f / $r;    // float / Rational → float
```

Division isn't commutative, so `int|float / Rational` is a distinct case from `Rational / int|float`. It also gives a
way to compute a multiplicative inverse. See "Inverting with `1 / r` or `r ** -1`" below.

Dividing by zero throws `ArithmeticException`, matching `div()`, even when the divisor is a `float` (`$r / 0.0`). PHP's
own `/` also throws for a zero divisor, but a `DivisionByZeroError`, so catch `ArithmeticException` here instead.

**Throws:**

- `ArithmeticException` if dividing by zero, including `0.0`.
- `DomainException` if a `float` operand is `±INF` or `NAN`.
- `OverflowException` if the result's numerator or denominator overflows an `int`, or a `float` result is too large to
  represent.
- `TypeError` if the other operand isn't a `Rational`, `int` or `float`.

**Examples:**

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 / $r2;    // 8/9
$r1 / 2;      // 1/3
1 / $r1;      // 3/2  (the inverse of $r1)
$r1 / 0.75;   // 0.8888...
0.75 / $r1;   // 1.125
```

**Equivalence table:**

| Operation   | Equivalent to               |
| ----------- | --------------------------- |
| `$r1 / $r2` | `$r1->div($r2)`             |
| `$r / $i`   | `$r->div($i)`               |
| `$i / $r`   | `new Rational($i)->div($r)` |
| `$r / $f`   | `$r->toFloat() / $f`        |
| `$f / $r`   | `$f / $r->toFloat()`        |

---

### \*\* (power)

```php
$result = $r1 ** $r2;  // Rational ** Rational → Rational
$result = $r ** $i;    // Rational ** int → Rational
$result = $i ** $r;    // int ** Rational → Rational
$result = $r ** $f;    // Rational ** float → float
$result = $f ** $r;    // float ** Rational → float
```

`Rational ** int`, `Rational ** Rational` and `int ** Rational` are exact, calculated as [`pow()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#pow) does. The result is a
`Rational`, or the operation throws `DomainException` if the result is irrational or not real.

`Rational ** float` and `float ** Rational` widen the `Rational` to a `float` and return a `float`:

- `float ** Rational` gives a real odd root of a negative base: `(-8.0) ** new Rational(1, 3)` is `-2.0`, where PHP's
  own `**` gives `NAN`.
- Where PHP's `**` would give `NAN`, such as `(-0.25) ** 0.5`, the operation throws `DomainException` instead.
- As for the other operators, a non-finite operand throws `DomainException`, and overflow throws `OverflowException`.

**Throws:**

- `ArithmeticException` if raising zero to a negative power.
- `DomainException` if a `float` operand is `±INF` or `NAN`, or the result is irrational or not real.
- `OverflowException` if an exact result's numerator or denominator overflows an `int`, or a `float` result is too large
  to represent.
- `TypeError` if the other operand isn't a `Rational`, `int` or `float`.

**Examples:**

```php
$r = new Rational(2, 3);
$r ** 2;                                     // 4/9
$r ** -2;                                    // 9/4 (a negative exponent gives the reciprocal)
4 ** new Rational(1, 2);                     // 2
new Rational(4, 9) ** new Rational(1, 2);    // 2/3
new Rational(-8, 27) ** new Rational(1, 3);  // -2/3
2 ** new Rational(1, 2);                     // throws DomainException (irrational)
$r ** 0.5;                                   // 0.816497
2.5 ** $r;                                   // 1.842016
```

**Equivalence table:**

| Operation    | Equivalent to                                                     |
| ------------ | ----------------------------------------------------------------- |
| `$r1 ** $r2` | `$r1->pow($r2)`                                                   |
| `$r ** $i`   | `$r->pow($i)`                                                     |
| `$i ** $r`   | `new Rational($i)->pow($r)`                                       |
| `$r ** $f`   | `$r->toFloat() ** $f`                                             |
| `$f ** $r`   | `$f ** $r->toFloat()`, but with real odd roots of a negative `$f` |

---

## Inverting with `1 / r` or `r ** -1`

Since `int|float / Rational` is supported (see `/` above), `1 / $r` computes the same multiplicative inverse as
[`$r->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#inv).

`$r ** -1` gives the same result, since raising to the power -1 is the mathematical definition of a multiplicative
inverse.

All three throw `ArithmeticException` if `$r`'s numerator is zero, matching `inv()`.

**Example:**

```php
$r = new Rational(3, 4);
$r->inv();  // 4/3
1 / $r;     // 4/3, same result
$r ** -1;   // 4/3, same result
```

---

## Comparison Operators

The `compare` handler behind all seven operators is equivalent to [`compare()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#compare): it accepts `Rational`, `int`, or `float`
operands and throws the same exceptions for anything else.

If either operand is a `float`, the `Rational` operand will be widened to a `float`, and the two values compared
normally as floats. Otherwise, the comparison algorithm uses only integers, to avoid floating point rounding errors. If
that overflows, the values are compared as floats instead, so two very close `Rational` values can compare as equal.

See [Comparison Operators](Comparison_Operators.md) for how comparison works across all four classes, including `null` and `bool` operands, and the
`equal()`/`approxEqual()` methods recommended over the operators.

---

### <=> (spaceship)

```php
$result = $r1 <=> $r2;  // Rational <=> Rational → int
$result = $r <=> $i;    // Rational <=> int → int
$result = $i <=> $r;    // int <=> Rational → int
$result = $r <=> $f;    // Rational <=> float → int
$result = $f <=> $r;    // float <=> Rational → int
```

Returns `-1` for "less than", `0` for "equal" (that is, _numerically_ equal), and `1` for "greater than".

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 <=> $r2;   // 1
$r1 <=> 1;     // -1
1 <=> $r1;     // 1
$r1 <=> 0.5;   // 0  (1/2 equals 0.5 exactly)
0.5 <=> $r1;   // 0
```

**Equivalence table:**

| Operation     | Equivalent to                                         |
| ------------- | ----------------------------------------------------- |
| `$r1 <=> $r2` | `$r1->compare($r2)`                                   |
| `$r <=> $i`   | `$r->compare($i)`                                     |
| `$i <=> $r`   | `new Rational($i)->compare($r)` or `-$r->compare($i)` |
| `$r <=> $f`   | `$r->compare($f)`                                     |
| `$f <=> $r`   | `$f <=> $r->toFloat()` or `-$r->compare($f)`          |

---

### < (less than)

```php
$result = $r1 < $r2;  // Rational < Rational → bool
$result = $r < $i;    // Rational < int → bool
$result = $i < $r;    // int < Rational → bool
$result = $r < $f;    // Rational < float → bool
$result = $f < $r;    // float < Rational → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 3);
$r2 = new Rational(1, 2);
$r1 < $r2;   // true
$r1 < 1;     // true
1 < $r1;     // false
$r1 < 0.5;   // true
0.5 < $r1;   // false
```

**Equivalence table:**

| Operation   | Equivalent to                                             |
| ----------- | --------------------------------------------------------- |
| `$r1 < $r2` | `$r1->lessThan($r2)`                                      |
| `$r < $i`   | `$r->lessThan($i)`                                        |
| `$i < $r`   | `new Rational($i)->lessThan($r)` or `$r->greaterThan($i)` |
| `$r < $f`   | `$r->lessThan($f)`                                        |
| `$f < $r`   | `$f < $r->toFloat()` or `$r->greaterThan($f)`             |

---

### <= (less than or equal)

```php
$result = $r1 <= $r2;  // Rational <= Rational → bool
$result = $r <= $i;    // Rational <= int → bool
$result = $i <= $r;    // int <= Rational → bool
$result = $r <= $f;    // Rational <= float → bool
$result = $f <= $r;    // float <= Rational → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 3);
$r2 = new Rational(1, 2);
$r1 <= $r2;   // true
$r1 <= 1;     // true
1 <= $r1;     // false
$r1 <= 0.5;   // true
0.5 <= $r1;   // false
```

**Equivalence table:**

| Operation    | Equivalent to                                                           |
| ------------ | ----------------------------------------------------------------------- |
| `$r1 <= $r2` | `$r1->lessThanOrEqual($r2)`                                             |
| `$r <= $i`   | `$r->lessThanOrEqual($i)`                                               |
| `$i <= $r`   | `new Rational($i)->lessThanOrEqual($r)` or `$r->greaterThanOrEqual($i)` |
| `$r <= $f`   | `$r->lessThanOrEqual($f)`                                               |
| `$f <= $r`   | `$f <= $r->toFloat()` or `$r->greaterThanOrEqual($f)`                   |

---

### > (greater than)

```php
$result = $r1 > $r2;  // Rational > Rational → bool
$result = $r > $i;    // Rational > int → bool
$result = $i > $r;    // int > Rational → bool
$result = $r > $f;    // Rational > float → bool
$result = $f > $r;    // float > Rational → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 > $r2;   // true
$r1 > 1;     // false
1 > $r1;     // true
$r1 > 0.5;   // false
0.5 > $r1;   // false
```

**Equivalence table:**

| Operation   | Equivalent to                                             |
| ----------- | --------------------------------------------------------- |
| `$r1 > $r2` | `$r1->greaterThan($r2)`                                   |
| `$r > $i`   | `$r->greaterThan($i)`                                     |
| `$i > $r`   | `new Rational($i)->greaterThan($r)` or `$r->lessThan($i)` |
| `$r > $f`   | `$r->greaterThan($f)`                                     |
| `$f > $r`   | `$f > $r->toFloat()` or `$r->lessThan($f)`                |

---

### >= (greater than or equal)

```php
$result = $r1 >= $r2;  // Rational >= Rational → bool
$result = $r >= $i;    // Rational >= int → bool
$result = $i >= $r;    // int >= Rational → bool
$result = $r >= $f;    // Rational >= float → bool
$result = $f >= $r;    // float >= Rational → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 >= $r2;   // true
$r1 >= 1;     // false
1 >= $r1;     // true
$r1 >= 0.5;   // true
0.5 >= $r1;   // true
```

**Equivalence table:**

| Operation    | Equivalent to                                                           |
| ------------ | ----------------------------------------------------------------------- |
| `$r1 >= $r2` | `$r1->greaterThanOrEqual($r2)`                                          |
| `$r >= $i`   | `$r->greaterThanOrEqual($i)`                                            |
| `$i >= $r`   | `new Rational($i)->greaterThanOrEqual($r)` or `$r->lessThanOrEqual($i)` |
| `$r >= $f`   | `$r->greaterThanOrEqual($f)`                                            |
| `$f >= $r`   | `$f >= $r->toFloat()` or `$r->lessThanOrEqual($f)`                      |

---

### == (equal)

```php
$result = $r1 == $r2;  // Rational == Rational → bool
$result = $r == $i;    // Rational == int → bool
$result = $i == $r;    // int == Rational → bool
$result = $r == $f;    // Rational == float → bool
$result = $f == $r;    // float == Rational → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(2, 4);
$r1 == $r2;   // true   (equal values, though different instances)
$r1 == 1;     // false
1 == $r1;     // false
$r1 == 0.5;   // true   (1/2 equals 0.5 exactly)
0.5 == $r1;   // true
```

**Equivalence table:**

| Operation    | Equivalent to                                    |
| ------------ | ------------------------------------------------ |
| `$r1 == $r2` | `$r1->equal($r2)`                                |
| `$r == $i`   | `$r->equal($i)`                                  |
| `$i == $r`   | `new Rational($i)->equal($r)` or `$r->equal($i)` |
| `$r == $f`   | `$r->equal($f)`                                  |
| `$f == $r`   | `$f === $r->toFloat()` or `$r->equal($f)`        |

---

### != (not equal)

```php
$result = $r1 != $r2;  // Rational != Rational → bool
$result = $r != $i;    // Rational != int → bool
$result = $i != $r;    // int != Rational → bool
$result = $r != $f;    // Rational != float → bool
$result = $f != $r;    // float != Rational → bool
```

There's no `notEqual()` method, so each equivalent negates `equal()`.

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Rational`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(2, 4);
$r1 != $r2;   // false
$r1 != 1;     // true
1 != $r1;     // true
$r1 != 0.5;   // false
0.5 != $r1;   // false
```

**Equivalence table:**

| Operation    | Equivalent to                                      |
| ------------ | -------------------------------------------------- |
| `$r1 != $r2` | `!$r1->equal($r2)`                                 |
| `$r != $i`   | `!$r->equal($i)`                                   |
| `$i != $r`   | `!new Rational($i)->equal($r)` or `!$r->equal($i)` |
| `$r != $f`   | `!$r->equal($f)`                                   |
| `$f != $r`   | `$f !== $r->toFloat()` or `!$r->equal($f)`         |

---

## Casting to PHP Number Types

A `Rational` can be cast to either PHP number type:

- `(float) $r` gives the same value as `$r->toFloat()`: the nearest float.
- `(int) $r` gives the same value as `$r->trunc()`, truncating toward zero, as `(int)` does for a float. For example,
  `(int) new Rational(-7, 3)` is `-2`, like `(int) (-7 / 3)`. To round another way, use `round()`, `floor()` or
  `ceil()`.

The cast is exact, so unlike `(int) $r->toFloat()`, `(int) $r` is right even when the numerator is beyond a float's
53-bit precision. It can't overflow, since the result is never larger in magnitude than the numerator.

`floatval()`, `intval()` and `settype()` use the same casts.

Other casts behave as for any object:

- `(string) $r` calls `__toString()`, e.g. `'-3/4'`.
- `(bool) $r` is always `true`, even for zero.

Without the extension, `(float) $r` and `(int) $r` warn and give `1.0` and `1`, so code that relies on them needs the
extension. PHPStan reports both casts as errors. See [Static Analysis](../Usage/Static_Analysis.md#known-limitation-number-casts).

**Examples:**

```php
$r = new Rational(-7, 3);
(float) $r;   // -2.3333333333333335
(int) $r;     // -2
floatval($r); // -2.3333333333333335
intval($r);   // -2
(string) $r;  // '-7/3'
```

**Equivalence table:**

| Operation     | Equivalent to      |
| ------------- | ------------------ |
| `(float) $r`  | `$r->toFloat()`    |
| `(int) $r`    | `$r->trunc()`      |
| `(string) $r` | `$r->__toString()` |

---

## See Also

- **[Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)** - the full class documentation in the Math package.
- **[Complex Operators](Complex.md)** - the other class with comparison operators, though `Complex`'s ordering is plain lexicographic.
  (real, then imaginary) rather than a genuine natural ordering like `Rational`'s.
