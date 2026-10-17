# OceanMoon PHP Math extension

Provides classes for Complex numbers, Rational numbers, Vectors, and Matrices.

**[License](LICENSE)** | **[Changelog](CHANGELOG.md)** | **[Documentation](docs/)**

![PHP 8.4](docs/images/logo_php8_4-125w.png) ![PHP 8.5](docs/images/logo_php8_5-125w.png)

A native PHP extension that implements the `Complex`, `Rational`, `Vector`, and `Matrix` classes from the
[OceanMoon PHP Math package](https://github.com/mossy2100/PHP-Math) in C, and adds operator overloading, which userland PHP classes can't offer on their own.

Each class has the same fully-qualified name and API as its package counterpart, so loading the extension replaces the
userland class with no code changes. The extension's version always matches the package version it mirrors.

The extension isn't published on Packagist or [PIE](https://github.com/php/pie) yet. See [Installation](docs/Usage/Installation.md) to build and install it from source.

---

## Description

**Key Features:**

- **Drop-in replacement** - same class names, methods and behaviour as `oceanmoon/math`.
- **Operator overloading** - `+`, `-`, `*`, `/`, `**` (where applicable) and their compound assignment forms (`+=`
  etc.), `~` (conjugate) for `Complex`, and `%` for `Rational`. `Complex` and `Rational` also get `++`, `--` and the
  full comparison operator set (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`). See [Classes](#classes).
- **Behavioural parity** - a PHPUnit conformance suite runs the package's own tests against the extension's classes, so
  the two stay identical in behaviour, not just API shape.
- **Standalone** - no runtime dependency on `oceanmoon/core`, `oceanmoon/math` or any other package, despite sharing
  their namespaces.

### Why Keep the Package and Extension in Step?

- Some developers prefer the package: its PHP source is a readable window into the algorithms, and some are wary of
  installing extensions.
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
- **Performance** - all four classes run faster than the package. In a simple benchmark:

| Operation             | PHP 8.4         | PHP 8.5        |
| --------------------- | --------------- | -------------- |
| `Complex::mul()`      | 5 times faster  | 3 times faster |
| `Rational::add()`     | 9 times faster  | 7 times faster |
| `Vector::dot()` (3D)  | 3 times faster  | 2 times faster |
| `Matrix::mul()` (4×4) | 15 times faster | 7 times faster |
#claude I would have expected PHP 8.5 to be faster.

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
  [Why PHP 8.4?](#why-php-84).
- **Debugging** - stepping through C needs `gdb`/`lldb` and a debug PHP build, whereas Xdebug works with the package out
  of the box.

---

## Development and Quality Assurance

[Claude Chat](https://claude.ai) and [Claude Code](https://www.claude.com/product/claude-code) were used extensively in developing this extension:

- Designing the C-level object-handler architecture: the `do_operation`/`compare` overloads, lazily-computed properties,
  and the custom `clone_obj` handler `Matrix` needs to deep-clone its row `Vector`s.
- Writing and reviewing the C implementation, stub files and documentation.

All code was reviewed by the author. Correctness is validated by:

- A PHPUnit conformance suite (`tests/phpunit/`), which runs the userland package's own tests (`oceanmoon/math`) against
  this extension's native classes. So behavior stays identical between the two, not just the method signatures.
- [PHPStan](https://phpstan.org/) at level 9, with custom operator-type-specifying extensions so static analysis understands the operator
  overloads that only exist here.

---

## Requirements

- **PHP 8.4.x or 8.5.x** (NTS or ZTS). The published build targets 8.4, matching the rest of the OceanMoon PHP package
  family; see "Why PHP 8.4?" below for how to build for 8.5 (or another version) yourself instead.
- To build from source: a C compiler and the PHP development headers (`phpize`, `php-config`). See Installation, below.

#### Why PHP 8.4?

A compiled PHP extension is ABI-locked to the exact PHP minor version it was built against (its Zend Extension API
number). The same `.so` can't just be loaded into a different version, and in the worst case a mismatched build can
compile "successfully" against the wrong version's headers while silently reading/writing the wrong struct layout at
runtime.

The published, prebuilt module is compiled against PHP 8.4, to match the minimum PHP version required by
`oceanmoon/core`, `oceanmoon/math`, and the other OceanMoon PHP packages. It's still ABI-locked to 8.4 specifically,
though: it won't load under 8.5 or any other version.

**If you're on PHP 8.5, or want a build for some other reason,** you can easily [build it yourself](docs/Usage/Development.md#building):

- `composer build 8.5` builds against 8.5, and `composer build` defaults to 8.4.
- The C source supports both, so you just build against the PHP version you have installed.
- Each version builds into its own tree (`build/8.4/`, `build/8.5/`, each with its module in `modules/`), so you can
  keep both side by side.

**If you're on another version of PHP,** in all likelihood the extension won't build due to changes in the internal Zend
engine APIs and structures that PHP's C extension interface depends on. These aren't part of PHP's stable,
backward-compatible userland API, and can change from one minor version to the next without notice. You can use
`#if PHP_VERSION_ID...` macros to make version-specific changes (see `complex.c` for an example), or perhaps get an AI
to help you.

---

## Usage

See: [Installation](docs/Usage/Installation.md) for using the extension in your project.

See: [Development](docs/Usage/Development.md) for building, testing, and the project's C source layout.

See: [Static Analysis](docs/Usage/Static_Analysis.md) for how to extend PHPStan to support the operators provided by this extension.

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

`Vector` and `Matrix` get no extension-provided comparison operators: "None added" above means no `compare` handler is
installed for them, so PHP's own default object comparison still applies, as it does to any plain PHP object (see
[Comparison Operators](#comparison-operators) below).

---

## Comparison Operators

The extension only affects PHP's loose comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`); strict equality
operators (`===`, `!==`) can't be overloaded. See [Comparison Operators](docs/Reference/Comparison_Operators.md) for details.

---

## Operator Precedence

A fluent chain of method calls has no notion of precedence at all. It only ever evaluates in the order you nest the
calls, so `$z1->add($z2->mul($z3))` requires you to have already worked out the correct grouping yourself before writing
a single method call. Operators come with precedence and associativity rules that PHP itself resolves instead, so
`$z1 + $z2 * $z3` reads the same as ordinary arithmetic and PHP evaluates `$z2 * $z3` first automatically.

That precedence isn't something any of these classes control, though. A PHP extension can overload what an operator
_does_ (via the `do_operation`/`compare` object handlers, which is how every operator on this page exists at all) but
not how tightly it binds relative to other operators. Precedence and associativity are fixed by the language grammar
itself, entirely outside any extension's control. So none of `Complex`/`Rational`/`Vector`/`Matrix` get their own
precedence rules; they all just inherit PHP's, same as `int` and `float`.

This is actually a blessing. Most PHP developers already know the most important precedence rules (multiplication before
addition, etc.), so keeping them fixed should reduce bugs. Refer to the [full PHP operator precedence table](https://www.php.net/manual/en/language.operators.precedence.php) to revise.

The table below shows just the rules relevant to this extension, tightest-binding first:

| Precedence  | Operators                  | Associativity | Used by                                 | Notes                                                                                                                                         |
| ----------- | -------------------------- | ------------- | --------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| 1 (highest) | `**`                       | Right         | `Complex`, `Rational`, `Matrix`         | Binds _tighter_ than unary `-`/`+`/`~` on its left operand - see below.                                                                       |
| 2           | `-` `+` `~` (unary prefix) | Right         | `-` `+` All four; `~` is `Complex`-only | `-$z ** 2` is `-($z ** 2)`, not `(-$z) ** 2`. This behaviour for the  `**` operator is a special case in PHP, not specific to this extension. |
| 3           | `*` `/`                    | Left          | All four                                |                                                                                                                                               |
| 4           | `-` `+` (binary)           | Left          | All four                                |                                                                                                                                               |
| 5           | `<` `<=` `>` `>=`          | Non-assoc     | `Complex`, `Rational`                   | Looser than arithmetic, so `$r1 + $r2 < $r3` is `($r1 + $r2) < $r3`.                                                                          |
| 6 (lowest)  | `==` `!=` `<=>`            | Non-assoc     | `Complex`, `Rational`                   | Looser again than `<`/`<=`/`>`/`>=` - `$r1 <=> $r2 == 1` is a parse error, matching PHP's own non-associativity here.                         |

The most tricky thing to remember is that the exponentiation operator `**` has the highest precedence of all, which
means, for example, `-$z ** 2` is evaluated as `-($z ** 2)` rather than `(-$z) ** 2`. That precedence can be unobvious
because typical code formatting omits spaces around unary operators, but includes them around binary ones, suggesting
unary always trumps binary. This is usually true, but not in the case of `**`.

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

MIT License - see [LICENSE](LICENSE) for details

---

## Support

- **Issues**: https://github.com/mossy2100/PHP-Math-extension/issues
- **Documentation**: See [docs/](docs/) directory for detailed class documentation
- **Examples**: See test files for comprehensive usage examples

For questions or suggestions, please [open an issue](https://github.com/mossy2100/PHP-Math-extension/issues).

---

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history and changes.
