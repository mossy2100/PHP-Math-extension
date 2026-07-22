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

## Unary Arithmetic Operators

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

`+`, `-`, `*`, and `/` each accept a `Rational`, `int`, or `float` operand on either side.

`add()`/`sub()`/`mul()`/`div()` only accept `self|int`, and can only be called on a `Rational`; there's no way to write
`$x->sub($r)` for an `int` `$x`.

The operators lift that restriction: `$r` and `$x` can appear in either order, and `int - Rational` (i.e. `$x - $r`)
computes the same result you'd get by promoting `$x` to a `Rational` first, then subtracting, matching what
`$x->sub($r)` would compute, if that was possible.

A genuine `float` operand is different: since the methods don't accept `float` at all, that case doesn't reduce to a
method call in any order - the operator instead widens per the rule above and returns a `float`.

The following table shows the equivalent method-style code for various forms of the `-` operator, where `$r` is a
`Rational`, `$i` is an `int`, and `$f` is a `float`. The same pattern holds for `+` and `add()`, `*` and `mul()`, and
`/` and `div()`.

| Operation | Equivalent to               |
| --------- | --------------------------- |
| `$r - $r` | `$r->sub($r)`               |
| `$r - $i` | `$r->sub($i)`               |
| `$i - $r` | `new Rational($i)->sub($r)` |
| `$r - $f` | `$r->toFloat() - $f`        |
| `$f - $r` | `$f - $r->toFloat()`        |

### + (add)

```php
$sum = $r1 + $r2;
```

Equivalent to [`$r1->add($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#add).

Forms:

1. `Rational + Rational`.
2. `Rational + int|float`.
3. `int|float + Rational`. Addition is commutative, so this gives the same result as #2.

**Example:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 + $r2;    // 5/6   (Rational + Rational)
$r1 + 1;      // 3/2   (Rational + int)
1 + $r1;      // 3/2   (int + Rational - same result, addition is commutative)
$r1 + 1.23;   // 1.73  (Rational + float)
1.23 + $r1;   // 1.73  (float + Rational - same result, addition is commutative)
```

### - (subtract)

```php
$diff = $r1 - $r2;
```

Equivalent to [`$r1->sub($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#sub).

Forms:

1. `Rational - Rational`.
2. `Rational - int|float`.
3. `int|float - Rational`. Subtraction isn't commutative, so this is a distinct case from #2 - the scalar is promoted to
   a `Rational` first, then subtracted normally.

**Example:**

```php
$r1 = new Rational(3, 4);
$r2 = new Rational(1, 4);
$r1 - $r2;    // 1/2   (Rational - Rational)
$r1 - 1;      // -1/4  (Rational - int)
1 - $r1;      // 1/4   (int - Rational)
$r1 - 0.25;   // 0.5   (Rational - float)
0.25 - $r1;   // -0.5  (float - Rational)
```

### \* (multiply)

```php
$product = $r1 * $r2;
```

Equivalent to [`$r1->mul($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#mul).

Forms:

1. `Rational * Rational`.
2. `Rational * int|float`.
3. `int|float * Rational`. Multiplication is commutative, so this gives the same result as #2.

**Example:**

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 * $r2;    // 1/2  (Rational * Rational)
$r1 * 3;      // 2    (Rational * int)
3 * $r1;      // 2    (int * Rational - same result, multiplication is commutative)
$r1 * 0.75;   // 0.5  (Rational * float)
0.75 * $r1;   // 0.5  (float * Rational - same result, multiplication is commutative)
```

### / (divide)

```php
$quotient = $r1 / $r2;
```

Equivalent to [`$r1->div($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#div).

Forms:

1. `Rational / Rational`.
2. `Rational / int|float`.
3. `int|float / Rational`. Division isn't commutative, so this is a distinct case from #2 - the scalar is promoted to a
   `Rational` first, then divided normally. This also gives a way to compute a multiplicative inverse - see "Inverting
   with `1 / r` or `r ** -1`" below.

**Example:**

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$r1 / $r2;    // 8/9        (Rational / Rational)
$r1 / 2;      // 1/3        (Rational / int)
1 / $r1;      // 3/2        (int / Rational - the inverse of $r1)
$r1 / 0.75;   // 0.8888...  (Rational / float)
0.75 / $r1;   // 1.125      (float / Rational)
```

Dividing by zero always throws `ArithmeticException`, matching `div()` - including `$r / 0.0`. This is a deliberate
departure from ordinary `float` division, which returns `INF`/`-INF`/`NAN` rather than throwing; this can be a hidden
source of bugs.

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

The following table shows the equivalent code for each form, where `$r` is a `Rational`, `$i` is an `int`, and `$f` is a
`float`. Since `**` isn't commutative and, unlike `+`/`-`/`*`/`/`, doesn't accept an `int` base with a `Rational`
exponent, there's no promotion-based equivalent for that case - it's simply unsupported.

| Operation  | Equivalent to         |
| ---------- | --------------------- |
| `$r ** $i` | `$r->pow($i)`         |
| `$r ** $f` | `$r->toFloat() ** $f` |
| `$f ** $r` | `$f ** $r->toFloat()` |
| `$r ** $r` | throws `TypeError`    |
| `$i ** $r` | throws `TypeError`    |

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

- `==` and `!=` come along as an unavoidable side effect of implementing `compare` - multiple PHP coding standards
  forbid them anyway, since they can lead to unpredictable type conversion and be a source of bugs.
- `===` and `!==` can't be overridden in a PHP extension, so they behave as normal: unlike scalars, they test _identity_
  equality for objects (whether both operands are the exact same object).
- To test _numeric_ equality between a `Rational` and another value (`Rational`, `int`, or `float`), use
  `Rational::equal()` or `Rational::approxEqual()` (e.g. `$r1->equal($r2)`).

### Forms

The same forms apply to all comparison operators:

1. `Rational # Rational`
2. `Rational # int`
3. `Rational # float`
4. `int # Rational`
5. `float # Rational`

If either operand is a `float`, the `Rational` will be widened to a `float`, and the two values compared normally as
floats.

Otherwise, only integers will be involved in the comparison, to avoid floating point rounding errors. An
`OverflowException` can occur in the event of integer overflow; to avoid, convert at least one operand to a `float`.

### <=> (spaceship)

Returns `-1` for "less than", `0` for "equal" (that is, _numerically_ equal), and `1` for "greater than".

```php
$result = $r1 <=> $r2;  // -1, 0, or 1
```

**Example:**

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$r1 <=> $r2;   // 1   (1/2 > 1/3; Rational <=> Rational)
$r1 <=> 1;     // -1  (1/2 < 1; Rational <=> int)
$r1 <=> 0.5;   // 0   (1/2 == 0.5 exactly; Rational <=> float)
1 <=> $r1;     // 1   (1 > 1/2; int <=> Rational)
0.5 <=> $r1;   // 0   (0.5 == 1/2 exactly; float <=> Rational)
```

### <, <=, >, >=

```php
$r1 < $r2;
$r1 <= $r2;
$r1 > $r2;
$r1 >= $r2;
```

Ordering comparisons, all derived from the same `<=>` handler. These operators all return a `bool`, as usual.

**Example:**

```php
$r1 = new Rational(1, 3);
$r2 = new Rational(1, 2);

var_dump($r1 < $r2);   // true (Rational < Rational)
var_dump($r2 >= $r1);  // true (Rational >= Rational)
var_dump($r1 < 1);     // true (1/3 < 1; Rational < int)
var_dump($r1 < 0.5);   // true (1/3 < 0.5; Rational < float)
var_dump(1 > $r1);     // true (1 > 1/3; int > Rational)
var_dump(0.5 > $r1);   // true (0.5 > 1/3; float > Rational)
```

---

## See Also

- **[Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)** - the full class documentation in the
  Math package
- **[Complex Operators](Complex.md)** - `Complex` deliberately gets no comparison operators, since it has no natural
  ordering
