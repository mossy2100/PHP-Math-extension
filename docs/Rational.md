# Rational Operators

Operator overloading for `OceanMoon\Math\Rational`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Rational`
(properties, factory methods, conversion, `compare()`/`approxCompare()`, rounding, etc.), see the package's own
[Rational documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md) - this page covers only the
operators the extension adds on top of it.

`Rational` is the only one of the four classes for which comparison operators are provided. Unlike `Complex`, `Vector`,
and `Matrix`, it has a genuine natural ordering.

---

## Numeric type widening

PHP already widens `int` to `float` when the two mix in an operation (e.g. `3 + 4.0` produces a `float`, even though the
result could technically be represented as an `int`).

`Rational` follows the same pattern, sitting between `int` and `float` in numeric width:

- `Rational` + `int` → the `int` is widened to a `Rational`, and the operation happens on two `Rational` values.
- `Rational` + `float` → the `Rational` is widened to a `float`, and the operation happens on two `float` values.

---

## Unary Arithmetic Operators

These are all prefix operators, and work the same way as for `int`/`float`.

---

### + (identity)

```php
$copy = +$r;
```

Returns an equal `Rational` with the same numerator and denominator.

There's no package method this maps to; value identity needs no explicit method call in ordinary PHP code. PHP has no
dedicated opcode for unary `+`/`-`; the compiler lowers `+$r` to `$r * 1`, which the extension handles via a
`Rational * int` operation, as documented below.

**Example:**

```php
$r = new Rational(2, 3);
$copy = +$r;  // 2/3
```

---

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

Equivalence table for the unary `-` operator, where `$r` is a `Rational`.

| Operation | Equivalent to |
| --------- | ------------- |
| `-$r`     | `$r->neg()`   |

---

## Binary Arithmetic Operators

`+`, `-`, `*`, and `/` each accept a `Rational`, `int`, or `float` operand on either side.

`add()`/`sub()`/`mul()`/`div()` only accept `self|int`, and can only be called on a `Rational`; there's no way to write
`$x->sub($r)` for an `int` `$x`.

The operators lift that restriction: `$r` and `$x` can appear in either order, and `int - Rational` (i.e. `$x - $r`)
computes the same result you'd get by promoting `$x` to a `Rational` first, then subtracting, matching what
`$x->sub($r)` would compute, if that was possible.

A genuine `float` operand is different: since the methods don't accept `float` at all, that case doesn't reduce to a
method call in any order - the operator instead widens per the rule above and returns a `float`.

---

### + (add)

```php
$sum = $r + $q;
```

Equivalent to [`$r->add($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#add).

Forms:

1. `Rational + Rational`.
2. `Rational + int|float`.
3. `int|float + Rational`. Addition is commutative, so this gives the same result as #2.

**Example:**

```php
$r = new Rational(1, 2);
$q = new Rational(1, 3);
$r + $q;    // 5/6   (Rational + Rational)
$r + 1;      // 3/2   (Rational + int)
1 + $r;      // 3/2   (int + Rational - same result, addition is commutative)
$r + 1.23;   // 1.73  (Rational + float)
1.23 + $r;   // 1.73  (float + Rational - same result, addition is commutative)
```

Equivalence table for the `+` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`.

| Operation | Equivalent to               | Also equivalent |
| --------- | --------------------------- | --------------- |
| `$r + $q` | `$r->add($q)`               |                 |
| `$r + $i` | `$r->add($i)`               |                 |
| `$i + $r` | `new Rational($i)->add($r)` | `$r->add($i)`   |
| `$r + $f` | `$r->toFloat() + $f`        |                 |
| `$f + $r` | `$f + $r->toFloat()`        |                 |

---

### - (subtract)

```php
$diff = $r - $q;
```

Equivalent to [`$r->sub($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#sub).

Forms:

1. `Rational - Rational`.
2. `Rational - int|float`.
3. `int|float - Rational`. Subtraction isn't commutative, so this is a distinct case from #2 - the scalar is promoted to
   a `Rational` first, then subtracted normally.

**Example:**

```php
$r = new Rational(3, 4);
$q = new Rational(1, 4);
$r - $q;    // 1/2   (Rational - Rational)
$r - 1;      // -1/4  (Rational - int)
1 - $r;      // 1/4   (int - Rational)
$r - 0.25;   // 0.5   (Rational - float)
0.25 - $r;   // -0.5  (float - Rational)
```

Equivalence table for the `-` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`.

| Operation | Equivalent to               | Also equivalent |
| --------- | --------------------------- | --------------- |
| `$r - $q` | `$r->sub($q)`               |                 |
| `$r - $i` | `$r->sub($i)`               |                 |
| `$i - $r` | `new Rational($i)->sub($r)` | `-$r->sub($i)`  |
| `$r - $f` | `$r->toFloat() - $f`        |                 |
| `$f - $r` | `$f - $r->toFloat()`        |                 |

---

### \* (multiply)

```php
$product = $r * $q;
```

Equivalent to [`$r->mul($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#mul).

Forms:

1. `Rational * Rational`.
2. `Rational * int|float`.
3. `int|float * Rational`. Multiplication is commutative, so this gives the same result as #2.

**Example:**

```php
$r = new Rational(2, 3);
$q = new Rational(3, 4);
$r * $q;    // 1/2  (Rational * Rational)
$r * 3;      // 2    (Rational * int)
3 * $r;      // 2    (int * Rational - same result, multiplication is commutative)
$r * 0.75;   // 0.5  (Rational * float)
0.75 * $r;   // 0.5  (float * Rational - same result, multiplication is commutative)
```

Equivalence table for the `*` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`.

| Operation | Equivalent to               | Also equivalent |
| --------- | --------------------------- | --------------- |
| `$r * $q` | `$r->mul($q)`               |                 |
| `$r * $i` | `$r->mul($i)`               |                 |
| `$i * $r` | `new Rational($i)->mul($r)` | `$r->mul($i)`   |
| `$r * $f` | `$r->toFloat() * $f`        |                 |
| `$f * $r` | `$f * $r->toFloat()`        |                 |

---

### / (divide)

```php
$quotient = $r / $q;
```

Equivalent to [`$r->div($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#div).

Forms:

1. `Rational / Rational`.
2. `Rational / int|float`.
3. `int|float / Rational`. Division isn't commutative, so this is a distinct case from #2 - the scalar is promoted to a
   `Rational` first, then divided normally. This also gives a way to compute a multiplicative inverse - see "Inverting
   with `1 / r` or `r ** -1`" below.

**Example:**

```php
$r = new Rational(2, 3);
$q = new Rational(3, 4);
$r / $q;    // 8/9        (Rational / Rational)
$r / 2;      // 1/3        (Rational / int)
1 / $r;      // 3/2        (int / Rational - the inverse of $r)
$r / 0.75;   // 0.8888...  (Rational / float)
0.75 / $r;   // 1.125      (float / Rational)
```

Dividing by zero always throws `ArithmeticException`, matching `div()` - including `$r / 0.0`. This is a deliberate
departure from ordinary `float` division, which returns `INF`/`-INF`/`NAN` rather than throwing; this can be a hidden
source of bugs.

Equivalence table for the `/` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`.

| Operation | Equivalent to               |
| --------- | --------------------------- |
| `$r / $q` | `$r->div($q)`               |
| `$r / $i` | `$r->div($i)`               |
| `$i / $r` | `new Rational($i)->div($r)` |
| `$r / $f` | `$r->toFloat() / $f`        |
| `$f / $r` | `$f / $r->toFloat()`        |

---

### \*\* (power)

```php
$result = $r ** $exponent;
```

Equivalent to [`$r->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#pow).

A number raised to a fractional power generally isn't itself rational (e.g. `2 ** (1/2)` is `sqrt(2)`), so `pow()`
doesn't accept a `Rational` or `float` exponent, and neither `Rational ** Rational` nor `int ** Rational` is supported
here either.

By the widening rule above, `int` only ever widens to `Rational`, never directly to `float` - so an `int` base has no
path to a `float` result unless the _other_ operand is already a genuine `float`. `float ** Rational` and
`Rational ** float` are both supported on that basis: the `Rational` is widened to a `float` first, then computed as a
regular `float ** float`.

Forms:

1. `Rational ** int`. Calculated as `(a/b)ⁿ = aⁿ/bⁿ` and simplified.
2. `Rational ** float`. The `Rational` is converted to a `float`, and the exponentiation is calculated as
   `float ** float`.
3. `float ** Rational`. As for #2, the `Rational` is converted to a `float`, and the exponentiation is calculated as
   `float ** float`.

Unsupported forms:

1. `Rational ** Rational`
2. `int ** Rational`

Equivalence table for the `**` operator, where `$r` is a `Rational`, `$i` is an `int`, and `$f` is a `float`.

| Operation  | Equivalent to         |
| ---------- | --------------------- |
| `$r ** $i` | `$r->pow($i)`         |
| `$r ** $f` | `$r->toFloat() ** $f` |
| `$f ** $r` | `$f ** $r->toFloat()` |

**Example:**

```php
$r = new Rational(2, 3);
$r ** 2;     // 4/9       (Rational ** int)
$r ** -2;    // 9/4       (Rational ** int; negative exponent = reciprocal)
$r ** 0.5;   // 0.816497  (Rational ** float)
2.5 ** $r;   // 1.842016  (float ** Rational)
```

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

- A single `compare` object handler serves `==`, `!=`, `<`, `<=`, `>`, `>=`, and `<=>` together - PHP does not provide a
  mechanism to override these individually.
- The handler is equivalent to [`compare()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#compare):
  it accepts `Rational`, `int`, or `float` operands and throws the same exceptions for anything else.

### A note on equality testing

The `==` and `!=` operators are implemented as an unavoidable side effect of implementing the "compare" object handler
in the extension. Note, however, several PHP coding standards forbid the use of these operators, since they can lead to
unpredictable type conversion and can therefore be a source of bugs. These include
[PHPStan strict rules](https://github.com/phpstan/phpstan-strict-rules) and the
[Slevomat](https://github.com/slevomat/coding-standard) standard. The PHP coding standard for WordPress also
[discourages them](https://developer.wordpress.org/coding-standards/wordpress-coding-standards/php/#clever-code).

The `===` and `!==` operators can't be overridden in a PHP extension, so they behave as normal. However, be aware that,
unlike scalars, for objects these operators test _identity_ equality, i.e. whether both operands refer to the exact same
object.

Of the four types provided by the extension, only `Rational` implements the comparison operators, because it has a
natural sort ordering. Therefore, none of the comparison operators (`==`/`!=` included) are implemented for `Complex`,
`Vector`, and `Matrix`, even though an equality comparison is valid for these types.

With all this in mind, it's recommended to use the `equal()` or `approxEqual()` methods for testing numeric equality for
all four types.

See:

- [`equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#equal)
- [`approxEqual()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#approxequal)

### Forms

The same forms apply to all comparison operators:

1. `Rational # Rational`
2. `Rational # int`
3. `Rational # float`
4. `int # Rational`
5. `float # Rational`

If either operand is a `float`, the `Rational` operand will be widened to a `float`, and the two values compared
normally as floats. Otherwise, the comparison algorithm uses only integers, to avoid floating point rounding errors. In
the event of integer overflow an `OverflowException` will occur; to avoid this, convert at least one operand to a
`float`.

---

### <=> (spaceship)

Returns `-1` for "less than", `0` for "equal" (that is, _numerically_ equal), and `1` for "greater than".

```php
$result = $r <=> $q;  // -1, 0, or 1
```

Equivalent to [`$r->compare($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#compare).

**Example:**

```php
$r = new Rational(1, 2);
$q = new Rational(1, 3);
$r <=> $q;    // 1   (1/2 > 1/3; Rational <=> Rational)
$r <=> 1;     // -1  (1/2 < 1; Rational <=> int)
$r <=> 0.5;   // 0   (1/2 == 0.5 exactly; Rational <=> float)
1 <=> $r;     // 1   (1 > 1/2; int <=> Rational)
0.5 <=> $r;   // 0   (0.5 == 1/2 exactly; float <=> Rational)
```

Equivalence table for the `<=>` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`.

| Operation   | Equivalent to                   | Also equivalent    |
| ----------- | ------------------------------- | ------------------ |
| `$r <=> $q` | `$r->compare($q)`               |                    |
| `$r <=> $i` | `$r->compare($i)`               |                    |
| `$i <=> $r` | `new Rational($i)->compare($r)` | `-$r->compare($i)` |
| `$r <=> $f` | `$r->compare($f)`               |                    |
| `$f <=> $r` | `$f <=> $r->toFloat()`          | `-$r->compare($f)` |

---

### < (less than)

```php
$result = $r < $q;
```

Equivalent to
[`$r->lessThan($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#lessthan-greaterthan-etc).

Ordering comparison, derived from the `<=>` handler. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 3);
$q = new Rational(1, 2);
$r < $q;  // true   (1/3 < 1/2; Rational < Rational)
$r < 1;    // true   (1/3 < 1; Rational < int)
1 < $r;    // false  (int < Rational)
$r < 0.5;  // true   (1/3 < 0.5; Rational < float)
0.5 < $r;  // false  (float < Rational)
```

Equivalence table for the `<` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. When the scalar is on the left, the method can be flipped to `greaterThan()`, since `$x < $r` means `$r > $x`.

| Operation | Equivalent to                    | Also equivalent       |
| --------- | -------------------------------- | --------------------- |
| `$r < $q` | `$r->lessThan($q)`               |                       |
| `$r < $i` | `$r->lessThan($i)`               |                       |
| `$i < $r` | `new Rational($i)->lessThan($r)` | `$r->greaterThan($i)` |
| `$r < $f` | `$r->lessThan($f)`               |                       |
| `$f < $r` | `$f < $r->toFloat()`             | `$r->greaterThan($f)` |

---

### <= (less than or equal)

```php
$result = $r <= $q;
```

Equivalent to
[`$r->lessThanOrEqual($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#lessthan-greaterthan-etc).

Ordering comparison, derived from the `<=>` handler. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 3);
$q = new Rational(1, 2);
$r <= $q;  // true   (1/3 <= 1/2; Rational <= Rational)
$r <= 1;    // true   (1/3 <= 1; Rational <= int)
1 <= $r;    // false  (int <= Rational)
$r <= 0.5;  // true   (1/3 <= 0.5; Rational <= float)
0.5 <= $r;  // false  (float <= Rational)
```

Equivalence table for the `<=` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. When the scalar is on the left, the method can be flipped to `greaterThanOrEqual()`, since `$x <= $r` means
`$r >= $x`.

| Operation  | Equivalent to                           | Also equivalent              |
| ---------- | --------------------------------------- | ---------------------------- |
| `$r <= $q` | `$r->lessThanOrEqual($q)`               |                              |
| `$r <= $i` | `$r->lessThanOrEqual($i)`               |                              |
| `$i <= $r` | `new Rational($i)->lessThanOrEqual($r)` | `$r->greaterThanOrEqual($i)` |
| `$r <= $f` | `$r->lessThanOrEqual($f)`               |                              |
| `$f <= $r` | `$f <= $r->toFloat()`                   | `$r->greaterThanOrEqual($f)` |

---

### > (greater than)

```php
$result = $r > $q;
```

Equivalent to
[`$r->greaterThan($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#lessthan-greaterthan-etc).

Ordering comparison, derived from the `<=>` handler. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 3);
$q = new Rational(1, 2);
$q > $r;   // true   (1/2 > 1/3; Rational > Rational)
$r > 1;     // false  (Rational > int)
1 > $r;     // true   (1 > 1/3; int > Rational)
$r > 0.5;   // false  (Rational > float)
0.5 > $r;   // true   (0.5 > 1/3; float > Rational)
```

Equivalence table for the `>` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. When the scalar is on the left, the method can be flipped to `lessThan()`, since `$x > $r` means `$r < $x`.

| Operation | Equivalent to                       | Also equivalent    |
| --------- | ----------------------------------- | ------------------ |
| `$r > $q` | `$r->greaterThan($q)`               |                    |
| `$r > $i` | `$r->greaterThan($i)`               |                    |
| `$i > $r` | `new Rational($i)->greaterThan($r)` | `$r->lessThan($i)` |
| `$r > $f` | `$r->greaterThan($f)`               |                    |
| `$f > $r` | `$f > $r->toFloat()`                | `$r->lessThan($f)` |

---

### >= (greater than or equal)

```php
$result = $r >= $q;
```

Equivalent to
[`$r->greaterThanOrEqual($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#lessthan-greaterthan-etc).

Ordering comparison, derived from the `<=>` handler. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 3);
$q = new Rational(1, 2);
$q >= $r;  // true   (1/2 >= 1/3; Rational >= Rational)
$r >= 1;    // false  (Rational >= int)
1 >= $r;    // true   (1 >= 1/3; int >= Rational)
$r >= 0.5;  // false  (Rational >= float)
0.5 >= $r;  // true   (0.5 >= 1/3; float >= Rational)
```

Equivalence table for the `>=` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. When the scalar is on the left, the method can be flipped to `lessThanOrEqual()`, since `$x >= $r` means
`$r <= $x`.

| Operation  | Equivalent to                              | Also equivalent           |
| ---------- | ------------------------------------------ | ------------------------- |
| `$r >= $q` | `$r->greaterThanOrEqual($q)`               |                           |
| `$r >= $i` | `$r->greaterThanOrEqual($i)`               |                           |
| `$i >= $r` | `new Rational($i)->greaterThanOrEqual($r)` | `$r->lessThanOrEqual($i)` |
| `$r >= $f` | `$r->greaterThanOrEqual($f)`               |                           |
| `$f >= $r` | `$f >= $r->toFloat()`                      | `$r->lessThanOrEqual($f)` |

---

### == (equal)

```php
$result = $r == $q;
```

Equivalent to [`$r->equal($q)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#equal).

Numeric equality, derived from the `<=>` handler - see "A note on equality testing" above. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 2);
$q = new Rational(2, 4);
$r == $q;  // true   (1/2 == 2/4 exactly, not identity; Rational == Rational)
$r == 1;    // false  (Rational == int)
1 == $r;    // false  (int == Rational)
$r == 0.5;  // true   (1/2 == 0.5 exactly; Rational == float)
0.5 == $r;  // true   (float == Rational)
```

Equivalence table for the `==` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. Equality is symmetric, so the reversed-operand row is identical to its un-reversed counterpart.

| Operation  | Equivalent to                 | Also equivalent |
| ---------- | ----------------------------- | --------------- |
| `$r == $q` | `$r->equal($q)`               |                 |
| `$r == $i` | `$r->equal($i)`               |                 |
| `$i == $r` | `new Rational($i)->equal($r)` | `$r->equal($i)` |
| `$r == $f` | `$r->equal($f)`               |                 |
| `$f == $r` | `$f === $r->toFloat()`        | `$r->equal($f)` |

---

### != (not equal)

```php
$result = $r != $q;
```

There's no `notEqual()` method; the negation `!$r->equal($q)` is the equivalent.

Numeric inequality, derived from the `<=>` handler - see "A note on equality testing" above. Returns a `bool`.

**Example:**

```php
$r = new Rational(1, 2);
$q = new Rational(2, 4);
$r != $q;  // false  (1/2 == 2/4 exactly; Rational != Rational)
$r != 1;    // true   (Rational != int)
1 != $r;    // true   (int != Rational)
$r != 0.5;  // false  (1/2 == 0.5 exactly; Rational != float)
0.5 != $r;  // false  (float != Rational)
```

Equivalence table for the `!=` operator, where `$r` and `$q` are `Rational` values, `$i` is an `int`, and `$f` is a
`float`. As with `==`, equality is symmetric, so the reversed-operand row is identical to its un-reversed counterpart.

| Operation  | Equivalent to                  | Also equivalent  |
| ---------- | ------------------------------ | ---------------- |
| `$r != $q` | `!$r->equal($q)`               |                  |
| `$r != $i` | `!$r->equal($i)`               |                  |
| `$i != $r` | `!new Rational($i)->equal($r)` | `!$r->equal($i)` |
| `$r != $f` | `!$r->equal($f)`               |                  |
| `$f != $r` | `$f !== $r->toFloat()`         | `!$r->equal($f)` |

---

## See Also

- **[Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)** - the full class documentation in the
  Math package
- **[Complex Operators](Complex.md)** - `Complex` deliberately gets no comparison operators, since it has no natural
  ordering
