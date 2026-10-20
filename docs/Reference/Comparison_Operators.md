# Comparison Operators

Comparison operators are backed by PHP's `compare` object handler rather than `do_operation`, and they're the one area
where the extension's four classes differ from each other.

This document covers what the extension changes. For how comparison works *without* the extension - including the
defaults PHP applies to any object - see the matching [Comparison Operators](https://github.com/mossy2100/PHP-Math/blob/main/docs/Comparison_Operators.md) document in the Math package.

There are two types of comparison operators in PHP:

1. **Loose**: `<=>`, `==`, `!=`, `<`, `<=`, `>`, `>=`. Flexible about type.
2. **Strict**: `===`, `!==`. These include the operands' types in the comparison, and for objects they mean reference
   identity rather than value equality.

---

## Loose Comparison Operators

A PHP extension can't override a subset of the loose operators independently: a single `compare` object handler backs
`<=>`, and PHP derives the other six (`==`, `!=`, `<`, `<=`, `>`, `>=`) from its result. There's no way to implement
some of the seven and fall back to PHP's default for the rest.

`Complex` and `Rational` provide `compare` handlers, so their comparison operators accept `int` and `float` operands as
well as their own type. Without one - as when using the package alone - comparing with an `int` or `float` instead
emits a notice and reads the object as `1`, so `Complex(5, 0) == 5` and `Rational(1, 2) == 0.5` are both `false`.

With the extension, both handlers:

- Accept an `int` or `float` operand on either side, promoted the same way their `equal()` method promotes one.
- Treat any other operand as uncomparable, including `NAN`, rather than throwing:
  - `==` is `false` and `!=` is `true`, so generic code such as `in_array()` works on mixed arrays.
  - The ordering operators give PHP's results for uncomparable values, which mean nothing: `<` and `<=` are `false`,
    `>` and `>=` are `true`, and `<=>` is `1`, whichever side the object is on. That matches how PHP treats `NAN` for a
    float.
- Leave `null` and `bool` to PHP, which compares them with an object itself, without calling the handler, by treating
  the object as `true`. So `$z == null` is `false` and `$z == true` is `true`.

The methods stay strict: `equal()`, `compare()` and the rest throw `InvalidArgumentException` for an unsupported type
and `DomainException` for `NAN`.

`Vector` and `Matrix` provide no `compare` handler. There's no natural way to order one list of elements against
another, as there is for a `(real, imaginary)` pair or a single rational value. So they keep PHP's own default object
`compare` handler, like any plain PHP object.

What the orderings mean:

- `Rational` has a genuine natural ordering, so its comparison operators mean exactly what you'd expect. See
  [Rational Operators](Rational.md#comparison-operators).
- `Complex`'s ordering is lexicographic (real part first, then imaginary). It's useful for sorting and deduplication,
  but not mathematically meaningful unless both values are real. See [Complex Operators](Complex.md#comparison-operators).

---

## Strict Comparison Operators

`===` and `!==` can't be overridden by a PHP extension, so they behave as normal. For objects, they always mean
reference identity: two distinct `Complex`/`Rational`/`Vector`/`Matrix` instances representing the same value are never
`===`, even when they are `==` or `equal()`:

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(3, 4);

$z1 == $z2;   // true  (same value)
$z1 === $z2;  // false (different instances)
```

---

## Equality Methods

`Vector` and `Matrix` provide no comparison operators of their own, so the default object comparison described under
[Loose comparison operators](#loose-comparison-operators) above applies to them. For `==`/`!=` that happens to give a reasonable element-wise equality
result, but without `approxEqual()`'s floating-point tolerance; their ordering operators (`<`, `<=`, `>`, `>=`, `<=>`)
aren't mathematically meaningful, for the same reason `Complex`'s aren't.

Some coding standards (PHPStan strict rules, Slevomat, and others) discourage or disallow `==`/`!=` in favour of
explicit method calls regardless, so `equal()`/`approxEqual()` remain the recommended way to test value equality across
all four classes. All four are documented in the Math package documentation, which applies equally to the extension.

See:

- [`Complex::equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#equal)
- [`Complex::approxEqual()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#approxequal)
- [`Rational::equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#equal)
- [`Rational::approxEqual()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md#approxequal)
- [`Vector::equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#equal)
- [`Vector::approxEqual()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md#approxequal)
- [`Matrix::equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#equal)
- [`Matrix::approxEqual()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md#approxequal)

---

## See Also

- **[Complex Operators](Complex.md)** and **[Rational Operators](Rational.md)** - the two classes with an extension-provided `compare` handler, and
  what their orderings mean
- **[Vector Operators](Vector.md)** and **[Matrix Operators](Matrix.md)** - neither gets an extension-provided comparison handler
- **[Operator Precedence](../../README.md#operator-precedence)** - where comparison operators sit in the precedence order, covered in the main README
- **[Comparison Operators](https://github.com/mossy2100/PHP-Math/blob/main/docs/Comparison_Operators.md)** (Math package) - the full account of how comparison behaves without the extension
