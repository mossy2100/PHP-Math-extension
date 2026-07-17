# Rational Operators

Operator overloading for `OceanMoon\Math\Rational`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Rational`
(properties, factory methods, conversion, `compare()`/`approxCompare()`, rounding, etc.), see the package's own
[Rational documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md) - this page covers only
the operators the extension adds on top of it.

*Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status.*

`Rational` is the only one of the four classes to get comparison operators, since - unlike `Complex`, `Vector`, and
`Matrix` - it has a genuine natural ordering.

---

## Binary Arithmetic Operators

Each operator below is equivalent to calling the same-named method directly, and accepts the same operand types
(`Rational`, `int`, or `float`) and throws the same exceptions - see the linked method docs for full details.

### + (add)

```php
$sum = $r1 + $r2;
```

Equivalent to [`$r1->add($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#add).

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(1, 3);
$sum = $r1 + $r2;  // 5/6
```

### - (subtract)

```php
$diff = $r1 - $r2;
```

Equivalent to [`$r1->sub($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#sub).

```php
$r1 = new Rational(3, 4);
$r2 = new Rational(1, 4);
$diff = $r1 - $r2;  // 1/2
```

### * (multiply)

```php
$product = $r1 * $r2;
```

Equivalent to [`$r1->mul($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#mul).

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$product = $r1 * $r2;  // 1/2
```

### / (divide)

```php
$quotient = $r1 / $r2;
```

Equivalent to [`$r1->div($r2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#div).

```php
$r1 = new Rational(2, 3);
$r2 = new Rational(3, 4);
$quotient = $r1 / $r2;  // 8/9
```

### ** (power)

```php
$result = $r ** $exponent;
```

Equivalent to [`$r->pow($exponent)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#pow). Unlike
the other three arithmetic operators, the operand must be an `int` (matching `pow()`'s signature) - there's no
general definition of a rational number raised to a fractional or another rational's power.

```php
$r = new Rational(2, 3);
$result = $r ** 2;   // 4/9
$result2 = $r ** -2; // 9/4 (negative exponent = reciprocal)
```

---

## Comparison Operators

A native `compare` object handler backs `<`, `<=`, `>`, `>=`, and `<=>` together - PHP has no way to wire up a
subset of these independently. The handler is equivalent to
[`compare()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#compare): it accepts `Rational`,
`int`, or `float` operands and throws the same exceptions for anything else.

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
the same `compare` object handler - once `<=>` exists for ordering, `==`/`!=` come along as an unavoidable side
effect, using exact value comparison (not the default identity-only comparison PHP objects get without a custom
handler), rather than something intentionally designed in. Prefer `equal()`/`approxCompare()` in new code.

```php
$r1 = new Rational(1, 2);
$r2 = new Rational(2, 4);

var_dump($r1 == $r2);   // true  (1/2 == 2/4 exactly, not identity)
var_dump($r1 != $r2);   // false
```

---

## See Also

- **[Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)** - the full class documentation in
  the Math package
- **[Complex Operators](Complex.md)** - `Complex` deliberately gets no comparison operators, since it has no natural
  ordering
