# Comparison Operators

Comparison operators are backed by PHP's `compare` object handler rather than `do_operation`, and they're the one area
where the extension's four classes differ from each other. This page covers what the extension changes; for how
comparison works *without* the extension - including the defaults PHP applies to any object - see the Math package's own
[Comparison Operators](https://github.com/mossy2100/PHP-Math/blob/main/docs/Comparison_Operators.md) doc.

There are two types of comparison operators in PHP:

1. **Loose**: `<=>`, `==`, `!=`, `<`, `<=`, `>`, `>=`. Flexible about type.
2. **Strict**: `===`, `!==`. These include type in the comparison, and for objects they mean reference identity rather
   than value equality.

---

## Loose comparison operators

A PHP extension can't override a subset of the loose group independently: a single `compare` object handler backs `<=>`,
and PHP derives the other five (`==`, `!=`, `<`, `<=`, `>`, `>=`) from its result - there's no way to implement some of
the six and fall back to PHP's default for the rest.

`Complex` and `Rational` each provide one; `Vector` and `Matrix` don't, since there's no natural way to order a whole
element list against another the way there is for a 2-element `(real, imaginary)` tuple or a single rational value.
Without an extension-provided handler, `Vector`/`Matrix` still get PHP's own default object `compare` handler (like
any plain PHP object).

- `Rational` has a genuine natural ordering, so its comparison operators mean exactly what you'd expect - see
  [Rational operators](Rational.md#comparison-operators).
- `Complex`'s ordering is plain lexicographic (real part first, then imaginary) - useful for sorting and deduplication,
  but not mathematically meaningful, since there's no total order compatible with complex arithmetic. It's exactly what
  PHP's own default object comparison already gives two `Complex` instances for free (`$real` is declared before
  `$imaginary`); the operators only add accepting an `int`/`float` operand on either side. See
  [Complex operators](Complex.md#comparison-operators) for the details.

Both accept an `int`/`float` operand on either side, promoted the same way their `equal()` method promotes one, and
throw for a `NAN` operand (no meaningful comparison result) - see each class's own docs for specifics.

---

## Strict comparison operators

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

## Equality methods

`Vector` and `Matrix` do still have comparison operators - PHP's default per-property comparison, same as any plain
PHP object, not an extension-provided handler (see [Loose comparison operators](#loose-comparison-operators) above).
For `==`/`!=` this happens to give a reasonable element-wise equality result, but without `approxEqual()`'s
floating-point tolerance; the ordering operators (`<`, `<=`, `>`, `>=`, `<=>`) aren't mathematically meaningful, for
the same reason `Complex`'s aren't.

Some coding standards (PHPStan strict rules, Slevomat, and others) discourage `==`/`!=` in favour of explicit method
calls regardless, so `equal()`/`approxEqual()` remain the recommended way to test value equality across all four
classes. All four are documented in the Math package documentation, which applies equally to the extension.

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

- **[Complex Operators](Complex.md)** and **[Rational Operators](Rational.md)** - the two classes with an
  extension-provided `compare` handler, and what their orderings mean
- **[Vector Operators](Vector.md)** and **[Matrix Operators](Matrix.md)** - neither gets an extension-provided
  comparison handler
- **[Operator Precedence](../../README.md#operator-precedence)** - where comparison operators sit in the precedence
  order, covered in the main README
- **[Comparison Operators](https://github.com/mossy2100/PHP-Math/blob/main/docs/Comparison_Operators.md)** (Math
  package) - the full account of how comparison behaves without the extension
