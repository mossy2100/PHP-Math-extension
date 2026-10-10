# Complex Operators

Operator overloading for `OceanMoon\Math\Complex`, available only when this extension is loaded. Plain PHP classes can't
overload operators, so none of this exists in the userland package. For everything else about `Complex` (properties,
factory methods, conversion, comparison, all the trigonometric/transcendental methods, etc.), see the package's own
[Complex documentation](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md). This page covers only the operators the extension adds on top of it.

On this page, `$z`, `$z1` and `$z2` are `Complex` values, and `$x` is an `int` or `float`.

## Unary Arithmetic Operators

### + (clone)

```php
$copy = +$z;  // +Complex → Complex
```

Returns an equal `Complex` with the same real and imaginary parts.

**Examples:**

```php
$z = new Complex(3, 4);
$copy = +$z;  // 3 + 4i
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `+$z`     | `clone $z`    |

---

### - (negate)

```php
$negated = -$z;  // -Complex → Complex
```

**Examples:**

```php
$z = new Complex(3, 4);
$negated = -$z;  // -3 - 4i
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `-$z`     | `$z->neg()`   |

---

### ~ (conjugate)

```php
$conjugate = ~$z;  // ~Complex → Complex
```

There's no natural PHP operator for "complex conjugate", so `~` (which usually means bitwise `NOT`) is repurposed for
it. This notation originated with GCC (GNU Compiler Collection), which extends standard C the same way, giving `~`
complex-conjugate behaviour when applied to its built-in `_Complex` types.

**Examples:**

```php
$z = new Complex(3, 4);
$conjugate = ~$z;  // 3 - 4i
```

**Equivalence table:**

| Operation | Equivalent to |
| --------- | ------------- |
| `~$z`     | `$z->conj()`  |

---

## Binary Arithmetic Operators

Each operator accepts a `Complex` or `int|float` operand on either side. Each is equivalent to calling the same-named
method directly, and throws the same exceptions, under the same conditions, as that method. See the linked method docs
for specifics.

This is a genuine flexibility gain over the fluent method API, not just shorter syntax. `$z->add($x)` is naturally
`Complex + int|float` - the method is called _on_ a `Complex`, so that side is fixed. There's no equally natural way to
write `int|float + Complex` with the fluent API alone; you'd need to promote the scalar first, e.g.
`(new Complex($x))->add($z)`. The `+` operator accepts either order directly, so `$x + $z` and `$z + $x` both just work.

This matters most for the non-commutative operators (`-`, `/`, `**`), where `$x - $z` and `$z - $x` are genuinely
different values and the operator lets you express whichever order the math actually calls for, without a scalar
promotion detour.

Operators also come with precedence and associativity rules that PHP itself resolves, whereas a fluent chain of method
calls has no notion of precedence at all. It only ever evaluates in the order you nest the calls, so you have to work
out the correct grouping yourself and encode it directly as nested `->` calls. `$z1 + $z2 * $z3` reads the same as
ordinary arithmetic and PHP evaluates `$z2 * $z3` first automatically; the fluent equivalent, `$z1->add($z2->mul($z3))`,
requires you to have already done that grouping in your head before writing a single method call. This applies equally
to every class this extension adds operators to, so see [Operator Precedence](../../README.md#operator-precedence) in the main README for the shared precedence
table and PHP manual link.

---

### + (add)

```php
$sum = $z1 + $z2;  // Complex + Complex → Complex
$sum = $z + $x;    // Complex + int|float → Complex
$sum = $x + $z;    // int|float + Complex → Complex
```

Addition is commutative, so reversing the operands gives the same result.

**Throws:**

- `DomainException` if an `int` or `float` operand is `±INF` or `NAN`, or the result isn't finite.
- `TypeError` if the other operand isn't a `Complex`, `int` or `float`.

**Examples:**

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(1, 2);
$z1 + $z2;   // 4 + 6i
$z1 + 2;     // 5 + 4i
2 + $z1;     // 5 + 4i
$z1 + 2.5;   // 5.5 + 4i
2.5 + $z1;   // 5.5 + 4i
```

**Equivalence table:**

| Operation   | Equivalent to                               |
| ----------- | ------------------------------------------- |
| `$z1 + $z2` | `$z1->add($z2)`                             |
| `$z + $x`   | `$z->add($x)`                               |
| `$x + $z`   | `new Complex($x)->add($z)` or `$z->add($x)` |

---

### - (subtract)

```php
$diff = $z1 - $z2;  // Complex - Complex → Complex
$diff = $z - $x;    // Complex - int|float → Complex
$diff = $x - $z;    // int|float - Complex → Complex
```

Subtraction isn't commutative, so `int|float - Complex` is a distinct case from `Complex - int|float`.

**Throws:**

- `DomainException` if an `int` or `float` operand is `±INF` or `NAN`, or the result isn't finite.
- `TypeError` if the other operand isn't a `Complex`, `int` or `float`.

**Examples:**

```php
$z1 = new Complex(5, 7);
$z2 = new Complex(2, 3);
$z1 - $z2;   // 3 + 4i
$z1 - 2;     // 3 + 7i
2 - $z1;     // -3 - 7i
$z1 - 2.5;   // 2.5 + 7i
2.5 - $z1;   // -2.5 - 7i
```

**Equivalence table:**

| Operation   | Equivalent to                                |
| ----------- | -------------------------------------------- |
| `$z1 - $z2` | `$z1->sub($z2)`                              |
| `$z - $x`   | `$z->sub($x)`                                |
| `$x - $z`   | `new Complex($x)->sub($z)` or `-$z->sub($x)` |

---

### \* (multiply)

```php
$product = $z1 * $z2;  // Complex * Complex → Complex
$product = $z * $x;    // Complex * int|float → Complex
$product = $x * $z;    // int|float * Complex → Complex
```

Multiplication is commutative, so reversing the operands gives the same result.

**Throws:**

- `DomainException` if an `int` or `float` operand is `±INF` or `NAN`, or the result isn't finite.
- `TypeError` if the other operand isn't a `Complex`, `int` or `float`.

**Examples:**

```php
$z1 = new Complex(1, 2);
$z2 = new Complex(3, 4);
$z1 * $z2;   // -5 + 10i
$z1 * 2;     // 2 + 4i
2 * $z1;     // 2 + 4i
$z1 * 2.5;   // 2.5 + 5i
2.5 * $z1;   // 2.5 + 5i
```

**Equivalence table:**

| Operation   | Equivalent to                               |
| ----------- | ------------------------------------------- |
| `$z1 * $z2` | `$z1->mul($z2)`                             |
| `$z * $x`   | `$z->mul($x)`                               |
| `$x * $z`   | `new Complex($x)->mul($z)` or `$z->mul($x)` |

---

### / (divide)

```php
$quotient = $z1 / $z2;  // Complex / Complex → Complex
$quotient = $z / $x;    // Complex / int|float → Complex
$quotient = $x / $z;    // int|float / Complex → Complex
```

Division isn't commutative, so `int|float / Complex` is a distinct case from `Complex / int|float`. It also gives a way
to compute a multiplicative inverse. See "Inverting with `1 / z` or `z ** -1`" below.

**Throws:**

- `ArithmeticException` if dividing by zero.
- `DomainException` if an `int` or `float` operand is `±INF` or `NAN`.
- `OverflowException` if the result is too large to represent.
- `TypeError` if the other operand isn't a `Complex`, `int` or `float`.

**Examples:**

```php
$z1 = new Complex(6, 8);
$z2 = new Complex(3, 4);
$z1 / $z2;   // 2             (the imaginary parts cancel)
$z1 / 2;     // 3 + 4i
1 / $z2;     // 0.12 - 0.16i  (the inverse of $z2)
$z1 / 2.5;   // 2.4 + 3.2i
2.5 / $z1;   // 0.15 - 0.2i
```

**Equivalence table:**

| Operation   | Equivalent to              |
| ----------- | -------------------------- |
| `$z1 / $z2` | `$z1->div($z2)`            |
| `$z / $x`   | `$z->div($x)`              |
| `$x / $z`   | `new Complex($x)->div($z)` |

---

### \*\* (power)

```php
$result = $z1 ** $z2;  // Complex ** Complex → Complex
$result = $z ** $x;    // Complex ** int|float → Complex
$result = $x ** $z;    // int|float ** Complex → Complex
```

Exponentiation isn't commutative, so `int|float ** Complex` is a distinct case from `Complex ** int|float`.

**Throws:**

- `ArithmeticException` if raising zero to a negative or non-real power.
- `DomainException` if an `int` or `float` operand is `±INF` or `NAN`, or the result isn't finite.
- `TypeError` if the other operand isn't a `Complex`, `int` or `float`.

**Examples:**

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(1, 1);
$z1 ** $z2;   // -1.63 + 1.12i
$z1 ** 2;     // -7 + 24i
2 ** $z1;     // -7.46 + 2.89i
$z1 ** 0.5;   // 2 + i           (the principal square root)
2.5 ** $z1;   // -13.53 - 7.81i
```

**Equivalence table:**

| Operation    | Equivalent to              |
| ------------ | -------------------------- |
| `$z1 ** $z2` | `$z1->pow($z2)`            |
| `$z ** $x`   | `$z->pow($x)`              |
| `$x ** $z`   | `new Complex($x)->pow($z)` |

---

## Inverting with `1 / z` or `z ** -1`

The fluent API provided by the package includes an `inv()` method, which inverts a `Complex`. The operators give you a
couple of alternative ways to achieve the same thing.

Since `int|float / Complex` is supported (see `/` above), `1 / $z` computes the same multiplicative inverse as
[`$z->inv()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#inv). `$z ** -1` gives the same result via `pow()` too, since raising to the power -1 is the mathematical
definition of a multiplicative inverse. All three throw `ArithmeticException` if `$z` is zero, matching `inv()`.

**Example:**

```php
$z = new Complex(3, 4);
$z->inv();  // 0.12 - 0.16i
1 / $z;     // 0.12 - 0.16i, same result
$z ** -1;   // 0.12 - 0.16i, same result
```

---

## Comparison Operators

The ordering operators (`<`, `<=`, `>`, `>=`, and `<=>`) aren't backed by a named method in the package, unlike the
other operators on this page. `Complex` has no `compare()` or `lessThan()`, because ordering complex numbers isn't
mathematically meaningful unless both are real.

They exist anyway because of how PHP extensions work. Letting `==` and `!=` compare a `Complex` with an `int` or `float`
requires a `compare` handler, and PHP derives all seven comparison operators from that one handler's result. There's no
way to provide `==` and `!=` without the others.

The ordering is lexicographic: real parts are compared first, then imaginary parts if the real parts are equal. This is
what PHP's default object comparison already gives two `Complex` instances, since `$real` is declared before
`$imaginary`. The handler adds one thing: an `int` or `float` operand is accepted on either side, promoted to a
`Complex` with a zero imaginary part, as [`equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#equal) does.

A `NAN` operand throws `DomainException`, matching `equal()`. There's no meaningful comparison with "not a number".
`INF` and `-INF` are supported despite being outside `Complex`'s always-finite range: a `Complex` is always less than
`INF` and greater than `-INF`.

PHP compares an object with `null` or a `bool` itself, without calling the handler, by treating the object as `true`. So
`$z == null` is `false` and `$z == true` is `true`, rather than throwing.

See [Comparison Operators](Comparison_Operators.md) for how comparison works across all four classes.

---

### <=> (spaceship)

```php
$result = $z1 <=> $z2;  // Complex <=> Complex → int
$result = $z <=> $x;    // Complex <=> int|float → int
$result = $x <=> $z;    // int|float <=> Complex → int
```

Returns `-1` for "less than", `0` for "equal", and `1` for "greater than": real parts are compared first, and imaginary
parts only if the real parts are equal.

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(1, 5);
$z2 = new Complex(2, 1);
$z1 <=> $z2;   // -1  (1 < 2, so the real parts alone decide it)

$z1 = new Complex(3, 1);
$z2 = new Complex(3, 5);
$z1 <=> $z2;   // -1  (the real parts are equal, so 1 < 5 decides it)

$z = new Complex(5, 0);
$z <=> 5;      // 0
$z <=> 6.0;    // -1
$z <=> INF;    // -1  (a Complex is always finite, so always less than +INF)
$z <=> NAN;    // throws DomainException
```

---

### < (less than)

```php
$result = $z1 < $z2;  // Complex < Complex → bool
$result = $z < $x;    // Complex < int|float → bool
$result = $x < $z;    // int|float < Complex → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(1, 0);
$z2 = new Complex(2, 0);
$z1 < $z2;   // true
$z1 < 1.5;   // true
1.5 < $z1;   // false
```

---

### <= (less than or equal)

```php
$result = $z1 <= $z2;  // Complex <= Complex → bool
$result = $z <= $x;    // Complex <= int|float → bool
$result = $x <= $z;    // int|float <= Complex → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(1, 0);
$z2 = new Complex(2, 0);
$z1 <= $z2;                // true
$z1 <= new Complex(1, 0);  // true
$z2 <= $z1;                // false
```

---

### > (greater than)

```php
$result = $z1 > $z2;  // Complex > Complex → bool
$result = $z > $x;    // Complex > int|float → bool
$result = $x > $z;    // int|float > Complex → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(3, 0);
$z2 = new Complex(2, 0);
$z1 > $z2;   // true
2.5 > $z2;   // true
```

---

### >= (greater than or equal)

```php
$result = $z1 >= $z2;  // Complex >= Complex → bool
$result = $z >= $x;    // Complex >= int|float → bool
$result = $x >= $z;    // int|float >= Complex → bool
```

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(3, 0);
$z2 = new Complex(2, 0);
$z1 >= $z2;                // true
$z1 >= new Complex(3, 0);  // true
$z2 >= $z1;                // false
```

---

### == (equal)

```php
$result = $z1 == $z2;  // Complex == Complex → bool
$result = $z == $x;    // Complex == int|float → bool
$result = $x == $z;    // int|float == Complex → bool
```

Two distinct instances representing the same value are `==`, matching [`equal()`](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md#equal). This is value equality, not identity.

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(3, 4);
$z1 == $z2;   // true  (same value, different instances)
$z1 == 5;     // false
```

**Equivalence table:**

| Operation    | Equivalent to     |
| ------------ | ----------------- |
| `$z1 == $z2` | `$z1->equal($z2)` |
| `$z == $x`   | `$z->equal($x)`   |
| `$x == $z`   | `$z->equal($x)`   |

---

### != (not equal)

```php
$result = $z1 != $z2;  // Complex != Complex → bool
$result = $z != $x;    // Complex != int|float → bool
$result = $x != $z;    // int|float != Complex → bool
```

The negation of `==`.

**Throws:**

- `DomainException` if the other operand is `NAN`.
- `InvalidArgumentException` if the other operand isn't a `Complex`, `int`, `float`, `bool` or `null`.

**Examples:**

```php
$z1 = new Complex(3, 4);
$z2 = new Complex(3, 5);
$z1 != $z2;   // true
```

**Equivalence table:**

| Operation    | Equivalent to      |
| ------------ | ------------------ |
| `$z1 != $z2` | `!$z1->equal($z2)` |
| `$z != $x`   | `!$z->equal($x)`   |
| `$x != $z`   | `!$z->equal($x)`   |

---

## See Also

- **[Complex](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md)** - the full class documentation in the Math package.
- **[Rational Operators](Rational.md)** - the other class with comparison operators, backed by its own `compare()` method (unlike
  `Complex`, which has no such method; see "Comparison Operators" above).
