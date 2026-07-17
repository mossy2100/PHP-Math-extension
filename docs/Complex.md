# Complex Operators

Operator overloading for `OceanMoon\Math\Complex`, available only when this extension is loaded - plain PHP classes
can't overload operators, so none of this exists in the userland package. For everything else about `Complex`
(properties, factory methods, conversion, comparison, all the trigonometric/transcendental methods, etc.), see the
package's own [Complex documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md) - this page
covers only the operators the extension adds on top of it.

_Planned - not yet implemented. See `docs/planning/MATH_EXTENSION.md` in the main repo for current status._

---

## Unary Arithmetic Operators

### ~ (conjugate)

```php
$conjugate = ~$z;
```

Equivalent to `$z->conj()`. There's no natural PHP operator for "complex conjugate", so `~` (bitwise NOT) is repurposed
for it - the closest visual/conceptual fit among the operators PHP allows a `do_operation` handler to intercept for a
single operand.

**Example:**

```php
$z = new Complex(3, 4);
$result = ~$z;  // 3 - 4i
```

---

## Binary Arithmetic Operators

Each operator below is equivalent to calling the same-named method directly, and accepts the same operand types
(`Complex` or `float`) and throws the same exceptions - see the linked method docs for full details.

### + (add)

```php
$sum = $z1 + $z2;
```

Equivalent to [`$z1->add($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#add).

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(1, 2);
$sum = $z1 + $z2;  // 4 + 6i
```

### - (subtract)

```php
$diff = $z1 - $z2;
```

Equivalent to [`$z1->sub($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#sub).

```php
$z1 = new Complex(5, 7);
$z2 = new Complex(2, 3);
$diff = $z1 - $z2;  // 3 + 4i
```

### \* (multiply)

```php
$product = $z1 * $z2;
```

Equivalent to [`$z1->mul($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#mul).

```php
$z1 = new Complex(1, 2);
$z2 = new Complex(3, 4);
$product = $z1 * $z2;  // -5 + 10i
```

### / (divide)

```php
$quotient = $z1 / $z2;
```

Equivalent to [`$z1->div($z2)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#div).

```php
$z = new Complex(6, 8);
$result = $z / 2;  // 3 + 4i
```

### \*\* (power)

```php
$result = $z ** $other;
```

Equivalent to [`$z->pow($other)`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#pow).

```php
$z = new Complex(3, 4);
$result = $z ** 2;  // -7 + 24i
```

---

## See Also

- **[Complex](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md)** - the full class documentation in the
  Math package
- **[Rational Operators](Rational.md)** - `Complex` deliberately gets no comparison operators (`==`, `!=`, `<`, etc.)
  since it has no natural ordering; `Rational` does, and gets the full set
