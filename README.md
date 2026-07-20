# OceanMoon PHP Math extension

**Work in progress.** A native PHP extension that replicates the
[OceanMoon PHP Math package](https://github.com/mossy2100/PHP-Math) - the `Complex`, `Rational`, `Vector`, and `Matrix`
classes - as a drop-in, faster substitute that also adds operator overloading, something userland PHP classes can't
offer on their own.

Since each class uses the same fully-qualified name as its PHP package counterpart, loading this extension transparently
replaces the userland class - no code changes required. Without the extension loaded, the plain PHP package classes are
used instead, so this is a purely additive, opt-in performance and ergonomics upgrade.

Implementation of `Complex` is currently in progress; it's the first class being built, with `Rational`, `Vector`, and
`Matrix` to follow the same pattern.

---

## Classes

Each class's full API (properties, factory methods, conversion, comparison, everything else) is documented in the Math
package itself - this extension is a drop-in replacement, not a different API. The pages below cover only what the
extension adds: operator overloading.

### [Complex](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md)

Adds `+`, `-`, `*`, `/`, `**`, and `~` (conjugate). See [Complex operators](docs/Complex.md).

### [Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md)

Adds `+`, `-`, `*`, `/`, `**`, and the full set of comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`). See
[Rational operators](docs/Rational.md).

### [Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)

Adds `+`, `-`, `*`, `/`. See [Vector operators](docs/Vector.md).

### [Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)

Adds `+`, `-`, `*`, `/`, `**`. See [Matrix operators](docs/Matrix.md).

---

## Operator Precedence

A fluent chain of method calls has no notion of precedence at all - it only ever evaluates in the order you nest the
calls, so `$z1->add($z2->mul($z3))` requires you to have already worked out the correct grouping yourself before writing
a single method call. Operators come with precedence and associativity rules that PHP itself resolves instead, so
`$z1 + $z2 * $z3` reads the same as ordinary arithmetic and PHP evaluates `$z2 * $z3` first automatically.

That precedence isn't something any of these classes control, though. A PHP extension can overload what an operator
_does_ (via the `do_operation`/`compare` object handlers, which is how every operator on this page exists at all) but
not how tightly it binds relative to other operators - precedence and associativity are fixed by the language grammar
itself, entirely outside any extension's control. So none of `Complex`/`Rational`/`Vector`/`Matrix` get their own
precedence rules; they all just inherit PHP's, same as `int`/`float`. See the
[full PHP operator precedence table](https://www.php.net/manual/en/language.operators.precedence.php) for every operator
PHP has - below is just the subset relevant to this extension, tightest-binding first:

| Precedence  | Operators                  | Associativity | Used by                                 | Notes                                                                                                                 |
| ----------- | -------------------------- | ------------- | --------------------------------------- | --------------------------------------------------------------------------------------------------------------------- |
| 1 (highest) | `**`                       | Right         | `Complex`, `Rational`, `Matrix`         | Binds _tighter_ than unary `-`/`+`/`~` on its left operand - see below.                                               |
| 2           | `-` `+` `~` (unary prefix) | Right         | `-` `+` All four; `~` is `Complex`-only | `-$z ** 2` is `-($z ** 2)`, not `(-$z) ** 2` - PHP's well-known `**` special case, not specific to this extension.    |
| 3           | `*` `/`                    | Left          | All four                                |                                                                                                                       |
| 4           | `-` `+` (binary)           | Left          | All four                                |                                                                                                                       |
| 5           | `<` `<=` `>` `>=`          | Non-assoc     | `Rational` only                         | Looser than arithmetic, so `$r1 + $r2 < $r3` is `($r1 + $r2) < $r3`.                                                  |
| 6 (lowest)  | `==` `!=` `<=>`            | Non-assoc     | `Rational` only                         | Looser again than `<`/`<=`/`>`/`>=` - `$r1 <=> $r2 == 1` is a parse error, matching PHP's own non-associativity here. |

```php
$z1 = new Complex(1, 1);
$z2 = new Complex(2, 0);
$z3 = new Complex(3, 0);

$z1 + $z2 * $z3;   // $z2 * $z3 is evaluated first: $z1 + ($z2 * $z3)
-$z1 ** 2;         // ** is evaluated first: -($z1 ** 2) = -2i
(-$z1) ** 2;       // parentheses override precedence: 2i (differs from the line above)
```

---

## Requirements

- PHP 8.4+
- A C compiler and the PHP development headers (`phpize`, `php-config`)

---

## Building

```bash
phpize
./configure --enable-oceanmoon_math
make
php -d extension="$PWD/modules/oceanmoon_math.so" -m | grep oceanmoon_math   # confirm it loads
```

---

## Testing

```bash
make test               # .phpt tests (tests/phpt/)
scripts/test-phpunit    # PHPUnit conformance tests against the Math package's own tests (tests/phpunit/)
```

---

## Project Structure

- Top level: module-wide files only (`math.c` - MINIT/RINIT/MINFO/module entry; `floats.c`/`floats.h` - shared helpers
  with no class affinity).
- One subfolder per class (`Complex/`, and eventually `Rational/`, `Vector/`, `Matrix/`): everything specific to that
  class - its `.c` implementation files, its `_internal.h`, its stub, and its generated arginfo header.

---

## License

MIT
