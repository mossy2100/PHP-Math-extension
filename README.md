# Mossy2100 PHP Math Extension

Provides classes for Complex numbers, Rational numbers, Vectors, and Matrices.

**[License](LICENSE)** | **[Changelog](CHANGELOG.md)** | **[Documentation](docs/)**

![PHP 8.4](docs/images/logo_php8_4-125w.png) ![PHP 8.5](docs/images/logo_php8_5-125w.png)

A native PHP extension that implements the `Complex`, `Rational`, `Vector`, and `Matrix` classes from the
[Mossy2100 PHP Math package](https://github.com/mossy2100/PHP-Math) in C, and adds operator overloading, which userland PHP classes can't offer on their own.

Each class has the same fully-qualified name and API as its package counterpart, so loading the extension replaces the
userland class with no code changes. The extension's version always matches the package version it mirrors.

Install it with [PIE](https://github.com/php/pie): `pie install mossy2100/math-ext`. See
[Installation](docs/Usage/Installation.md) for details.

---

## Description

**Key Features:**

- **Drop-in replacement** - same class names, methods and behavior as `mossy2100/math`.
- **Operator overloading** - `+`, `-`, `*`, `/`, `**` (where applicable) and their compound assignment forms (`+=`
  etc.), `~` (conjugate) for `Complex`, and `%` for `Rational`. `Complex` and `Rational` also get `++`, `--` and the
  full comparison operator set (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`). See [Classes](#classes).
- **Behavioral parity** - a PHPUnit conformance suite runs the package's own tests against the extension's classes, so
  the two stay identical in behavior, not just API shape.
- **Standalone** - no runtime dependency on `mossy2100/core`, `mossy2100/math` or any other package, despite sharing
  their namespaces.

### Why Keep the Package and Extension in Step?

- Some developers may prefer the package: its PHP source is a readable window into the algorithms, and some developers
  may be wary of installing extensions.
- An application built on the package can switch to the extension later without anything breaking.
- One set of class documentation (in the package) serves both.

### Advantages

- **Expressiveness** - mathematical expressions translate directly into code: `$z1 + $z2 * $z3` instead of
  `$z1->add($z2->mul($z3))`.
- **Either operand order** - `$x - $z` works as well as `$z - $x`. A method can only be called on the object, so the
  method form of `$x - $z` needs the scalar promoted first: `new Complex($x)->sub($z)`.
- **Operator precedence** - PHP applies the usual precedence rules automatically. With the fluent method API, the
  programmer must get the order of operations right by hand. See [Operator Precedence](#operator-precedence).
- **Operations with no method equivalent** - some operator forms have no matching method. For example,
  `Rational + float` widens to a `float` result, but `add()` only accepts a `Rational` or `int`. See the equivalence
  table on each class's operator page.
- **Performance** - all four classes run faster than the package. See the benchmarks below.

With opcache on, `scripts/bench` gives:

| Operation               | PHP 8.4         | PHP 8.5         |
| ----------------------- | --------------- | --------------- |
| `Complex::mul()`        | 7 times faster  | 5 times faster  |
| `Rational::add()`       | 17 times faster | 11 times faster |
| `Vector::dot()` (3D)    | 3 times faster  | 2 times faster  |
| `Matrix::mul()` (4×4)   | 23 times faster | 14 times faster |
| `Matrix::add()` (50×50) | 38 times faster | 21 times faster |
| `Matrix::mul()` (50×50) | 39 times faster | 21 times faster |
| `Matrix::inv()` (50×50) | 28 times faster | 17 times faster |

The extension runs at about the same speed on both versions, but the package runs faster on PHP 8.5, so the gap is
smaller there. Timings vary between machines and runs.

### Disadvantages

- **Operators tie your code to the extension** - code that uses the operators won't run with the package alone, so the
  "drop-in" fallback only holds for code that sticks to methods.
- **Static analysis** - PHPStan and similar tools don't know the operators exist, and report them as errors. This repo
  includes PHPStan extensions that teach it the operators, but they can't yet be used from another project. See
  [Static Analysis](docs/Usage/Static_Analysis.md).
- **IDE support** - IDEs such as PhpStorm and VS Code (Intelephense) flag the operators as invalid in their live
  inspections, and generally can't be taught otherwise.
- **Hosting** - shared hosting and many managed platforms don't allow custom PHP extensions. The package works
  everywhere.
- **Version-locked builds** - a compiled extension only loads in the PHP minor version it was built for. See
  [Supported PHP Versions](#supported-php-versions).
- **Debugging** - stepping through C needs `gdb`/`lldb` and a debug PHP build, whereas Xdebug works with the package out
  of the box.

### Argument Types

A parameter that takes either an object or a number, such as the one in `add()`, is always strict in the extension, as
if `strict_types=1` were on. Without `strict_types`, the package follows PHP's usual coercion rules for these
parameters:

| Call             | Package    | Extension   |
| ---------------- | ---------- | ----------- |
| `$z->add(' 2 ')` | Adds 2     | `TypeError` |
| `$r->add(2.0)`   | Adds 2     | `TypeError` |
| `$v->mul('2')`   | Multiplies | `TypeError` |

Other `int` and `float` parameters, such as the constructors', are coerced as usual by both. With `strict_types=1`, the
two behave identically.

---

## Development and Quality Assurance

This extension was written primarily by AI, using [Claude Code](https://claude.com/product/claude-code) and [DeepSeek](https://www.deepseek.com/):

- The public API mirrors the userland package (`mossy2100/math`), which the author designed.
- The author also specified which operators to implement for each type and how they should work.
- Claude Code and DeepSeek wrote the C implementation, including the object-handler architecture: the
  `do_operation`/`compare` overloads, lazily-computed properties, and a custom `clone_obj` handler.
- They also wrote the stub files, tests and documentation.

Correctness is validated by:

- A PHPUnit conformance suite (`tests/phpunit/`) that runs the package's own tests against the extension's classes.
- [PHPStan](https://phpstan.org/) at level 9, with custom operator-type-specifying extensions so static analysis
  understands the operator overloads that only exist here.

---

## Requirements

- **PHP 8.4.x or 8.5.x**, thread-safe (ZTS) or not (NTS).
- On Mac and Linux: a C compiler and the PHP development headers (`phpize`, `php-config`), since the extension is built
  from source for the PHP you have installed. On Windows, PIE downloads a prebuilt DLL instead. See
  [Installation](docs/Usage/Installation.md).

### Supported PHP Versions

A compiled PHP extension only loads in the PHP minor version it was built for, since each has its own Zend Extension API
number. This affects how you install it:

- **Mac and Linux:** [PIE](https://github.com/php/pie) builds the extension from source for the PHP it runs under, so
  you get a matching build automatically. After upgrading to a new PHP minor version, run `pie install` again.
- **Windows:** each GitHub release includes a prebuilt DLL for PHP 8.4 and 8.5, NTS and TS (64-bit), and PIE picks the
  one matching your PHP.
- **Building it yourself:** `composer build 8.4` or `composer build 8.5` builds into `build/8.4/` or `build/8.5/`, so
  both can sit side by side. See [Development](docs/Usage/Development.md#building).

Other PHP versions aren't supported. The extension uses the Zend engine's internal C APIs, which can change between
minor versions, so it likely won't build. `#if PHP_VERSION_ID` checks can adapt the code to another version (see
`complex.c` for an example).

---

## Usage

- [Installation](docs/Usage/Installation.md) - using the extension in your project
- [Development](docs/Usage/Development.md) - building, testing, and the project's C source layout
- [Static Analysis](docs/Usage/Static_Analysis.md) - extending PHPStan to support the operators

---

## Classes

Each class's full API (properties, factory methods, conversion, comparison, everything else) is documented in the Math
package itself. This extension is a drop-in replacement, not a different API. The table below covers what the extension
adds (operator overloading) and links to both sets of docs for each class.

| Class      | Arithmetic operators                      | Comparison operators                    | Class reference                                                              | Operator docs                                    |
| ---------- | ----------------------------------------- | --------------------------------------- | ---------------------------------------------------------------------------- | ------------------------------------------------ |
| `Complex`  | `+`, `-`, `*`, `/`, `**`, `++`, `--`, `~` | `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>` | [Complex](https://github.com/mossy2100/PHP-Math/blob/main/docs/Complex.md)   | [Complex operators](docs/Reference/Complex.md)   |
| `Rational` | `+`, `-`, `*`, `/`, `%`, `**`, `++`, `--` | `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>` | [Rational](https://github.com/mossy2100/PHP-Math/blob/main/docs/Rational.md) | [Rational operators](docs/Reference/Rational.md) |
| `Vector`   | `+`, `-`, `*`, `/`                        | None added                              | [Vector](https://github.com/mossy2100/PHP-Math/blob/main/docs/Vector.md)     | [Vector operators](docs/Reference/Vector.md)     |
| `Matrix`   | `+`, `-`, `*`, `/`, `**`                  | None added                              | [Matrix](https://github.com/mossy2100/PHP-Math/blob/main/docs/Matrix.md)     | [Matrix operators](docs/Reference/Matrix.md)     |

"None added" means `Vector` and `Matrix` keep PHP's default object comparison. See
[Comparison Operators](#comparison-operators).

---

## Comparison Operators

The extension only affects PHP's loose comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`); strict equality
operators (`===`, `!==`) can't be overloaded. See [Comparison Operators](docs/Reference/Comparison_Operators.md) for details.

---

## Operator Precedence

A chain of method calls evaluates in the order you nest it, so you have to work out the grouping yourself. Operators
follow PHP's precedence rules, so `$z1 + $z2 * $z3` reads like ordinary arithmetic.

An extension can change what an operator does, but not how tightly it binds. All four classes use PHP's
[operator precedence](https://www.php.net/manual/en/language.operators.precedence.php), the same as `int` and `float`.

The rules relevant to this extension, tightest-binding first:

| Precedence  | Operators                  | Associativity | Used by                                 | Notes                                              |
| ----------- | -------------------------- | ------------- | --------------------------------------- | -------------------------------------------------- |
| 1 (highest) | `**`                       | Right         | `Complex`, `Rational`, `Matrix`         | Binds tighter than a unary operator on its left    |
| 2           | `-` `+` `~` (unary prefix) | Right         | `-` `+` all four; `~` `Complex` only    | `-$z ** 2` is `-($z ** 2)`, not `(-$z) ** 2`       |
| 3           | `*` `/`                    | Left          | All four                                |                                                    |
| 4           | `-` `+` (binary)           | Left          | All four                                |                                                    |
| 5           | `<` `<=` `>` `>=`          | Non-assoc     | `Complex`, `Rational`                   | `$r1 + $r2 < $r3` is `($r1 + $r2) < $r3`           |
| 6 (lowest)  | `==` `!=` `<=>`            | Non-assoc     | `Complex`, `Rational`                   | `$r1 <=> $r2 == 1` is a parse error                |

> [!WARNING]
> `**` binds tighter than unary `-`, so `-$z ** 2` is `-($z ** 2)`. This is easy to miss, because unary operators are
> usually written without a space. It's standard PHP, not specific to this extension.

**Examples:**

```php
$z1 = new Complex(1, 1);
$z2 = new Complex(2, 0);
$z3 = new Complex(3, 0);

$z1 + $z2 * $z3;   // $z2 * $z3 is evaluated first: $z1 + ($z2 * $z3)
-$z1 ** 2;         // ** is evaluated first: -($z1 ** 2) = -2i
(-$z1) ** 2;       // parentheses override precedence: 2i (differs from the line above)
```

---

## License

MIT License - see [LICENSE](LICENSE) for details.

---

## Support

- **Issues and questions:** [open an issue](https://github.com/mossy2100/PHP-Math-extension/issues)
- **Class documentation:** in the [Math package](https://github.com/mossy2100/PHP-Math/tree/main/docs)
- **Operator documentation:** in this repo's [docs/Reference/](docs/Reference/) directory

---

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history and changes.
