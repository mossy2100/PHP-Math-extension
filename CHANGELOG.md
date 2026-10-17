# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

Matches the `oceanmoon/math` `[Unreleased]` changes.

### Added

- **`Rational::pow()` accepts a `Rational` exponent**, matching the package. `p/q` means the q-th root, raised to the
  power p. The result is exact, or it throws `DomainException`.
- **`**` supports a `Rational` exponent:**
  - `Rational ** Rational` and `int ** Rational` are exact, like `pow()`. For example, `4 ** new Rational(1, 2)` is `2`.
  - `float ** Rational` gives a float, with a real result for an odd root of a negative base:
    `(-8.0) ** new Rational(1, 3)` is `-2.0`, not `NAN`.
- **`Vector * Vector` and `Vector / Vector`** are element-wise (Hadamard), matching `mul()` and `div()` with a vector.
  Both previously threw `TypeError`.
- **`Rational::trunc()`**, matching the package: the integer part, truncating toward zero.
- **`(float) $r` and `(int) $r`** give `$r->toFloat()` and `$r->trunc()`, as do `floatval()`, `intval()` and
  `settype()`. `(int)` truncates, as it does for a float. Without the extension, both casts warn and give `1.0` or `1`.
  PHPStan reports them as errors, since casts have no extension point.
- **`Vector` and `Matrix` implement `IteratorAggregate`**, matching the package, so `foreach` yields a Vector's
  elements and a Matrix's live rows, instead of their public properties.
- **`Rational::mod()` and `%`.** `mod()` matches the package. `%` on a `Rational` is `mod()`, with a `Rational`, `int`
  or `float` on either side, widening to `float` like the other operators. `%=` works too.

### Changed

- **`Complex::atan()`/`atanh()`** name the function in their singularity messages, matching the package.
- **`Matrix::count()` returns the number of rows (breaking)**, matching the package, `$m[$i]` and `foreach`.
- **`Matrix::mul()` accepts a `Vector`, replacing `Matrix::mulVector()` (breaking).** `$m->mul($v)` is _Ax_ and returns
  a `Vector`. PHPStan infers the return type from the argument type.
- **`Vector::mul()` and `Vector::div()` accept a `Vector`, replacing `hadamardMul()` and `hadamardDiv()` (breaking),**
  matching the package. `div()`'s parameter is renamed from `$scalar` to `$other`.
- **`Vector::fromArray()`/`Matrix::fromArray()` (breaking):** a non-numeric element, or a `Matrix` row that isn't an
  array, throws `InvalidArgumentException` instead of `DomainException`. A non-finite element, or a row that isn't a
  list, still throws `DomainException`. The messages match the package.
- **`Vector::normalize()`** throws "Cannot normalize zero Vector." for a zero or empty vector, instead of "Cannot divide
  by zero."
- **Overflow throws `OverflowException` everywhere (breaking),** matching the package. This covers the methods and the
  operators. A result, or an intermediate value, too large to represent throws `OverflowException`, with a message
  naming the operation, instead of `DomainException` or a return value of `INF` or `NAN`. A non-finite operand still
  throws `DomainException`.
- **`new Complex()`** throws `OverflowException` when both parts are finite but the magnitude isn't, matching the
  package.
- **`Vector::mul()`, `Matrix::mul()` and `*`** throw `DomainException` for a non-finite scalar, like `div()` and `/`.
- **`Rational` operators with a `float` operand (breaking)** never accept or return `±INF` or `NAN`, like the rest of
  the library:
  - A non-finite `float` operand throws `DomainException`, e.g. "Invalid operand: NAN. Must be finite."
  - A result too large to represent throws `OverflowException`, e.g. "Cannot multiply the given Rational and float. The
    result would overflow."
  - Where PHP's `**` would give `NAN` from finite operands, an even root of a negative base throws `DomainException`,
    and zero to a negative power throws `ArithmeticException`.
- **`Rational` overflow messages name the operation,** for the methods and the operators, matching the package, e.g.
  "Cannot add the given Rational numbers. The result would overflow." Previously they were Core's, e.g. "Overflow in
  integer addition." The exception class (`OverflowException`) is unchanged.
- **`Complex::fromString()` and `Rational::fromString()`** include the string in an invalid-format message, quoted and
  abbreviated to 32 characters, matching the package. `Complex` previously showed it in full, and `Rational` not at all.
- **`scripts/enable` and `scripts/disable`** default to the PHP on `PATH`, rather than 8.4.
- **`Matrix::inv()`, `det()` and `pow()` use LU decomposition for matrices larger than 3×3 (breaking),** instead of
  cofactor expansion, which was O(n!): a 10×10 inverse took about 30 seconds, and now takes well under a millisecond.
  Matrices up to 3×3 still use the closed-form formulas, which give exact results where the arithmetic allows. Matches
  the package.
- **`Matrix::inv()` detects near-singular matrices (breaking).** It throws `ArithmeticException` "Cannot invert singular
  Matrix." if a pivot is negligible relative to its row, instead of only for an exactly zero determinant. Previously, a
  matrix such as `[[0.1, 0.3], [0.7, 2.1]]`, singular but with a determinant of about 3e-17 due to rounding, gave an
  inverse with elements around 1e16. The message was "Cannot invert Matrix with zero determinant."
- **`%` on `Complex`, `Vector` and `Matrix` throws `TypeError`** "Unsupported operand types", instead of converting
  the object to an int with a warning.

### Fixed

- **`pow()` parameter names** didn't match the package, which broke named arguments. `Complex::pow()`, `Rational::pow()`
  and `Matrix::pow()` now all take `$exp`.
- **Ported the package's float range fixes:**
  - `Complex::div()`, `inv()` and `/` use Smith's algorithm, so a very small or large divisor gives the right result.
    They throw `OverflowException` when the result is too large to represent.
  - `Vector::div()`, `Matrix::div()` and `/` throw `DomainException` for a non-finite scalar, instead of silently giving
    zeros.
  - `Vector::magnitude`, `Matrix::norm()` and `Vector::normalize()` scale the elements by a power of 2 first, so
    squaring them can't overflow or underflow.
  - `Vector::cross()` gives the right result whenever it fits, including for huge parallel vectors, and throws
    `OverflowException` otherwise.
- **`Complex` matches the package's results and messages exactly:**
  - A non-finite result now gives the package's message, e.g. "Cannot create Complex with non-finite real part: INF.",
    instead of a generic one. `fromPolar()` and `exp()` give the magnitude and phase messages.
  - `tan()`, `tanh()`, `atan()`, `atanh()` and `log()` divide with Smith's algorithm, like `div()`.
  - `pow()` with an integer exponent uses exponentiation by squaring, like the package, and an exponent of -1 uses
    `inv()`.
  - `asin()`, `acos()`, `atan()` and `atanh()` give the same sign for a zero part, and `log()` with base 2 or 10 uses
    `log2()` or `log10()`, as PHP's `log()` does.
- **`Matrix::inv()` and `**` with a negative exponent** work for a matrix of uniformly tiny or huge elements, matching
  the package.
- **`Complex::sqrt()` and `roots()` are exact on the axes**, matching the package. `pow(0.5)` and `** 0.5` use
  `sqrt()`, as do `asin()`, `acos()`, `asinh()` and `acosh()`. `roots()` refines the root magnitude, like the package.
- **`Matrix::inv()` and `**` with a negative exponent give no negative zeros**, matching the package.
- **`Complex::tan()`/`tanh()` never overflow**, matching the package's new algorithm.
- **`Rational` comparison is exact**, matching the package, for the comparison methods and operators. Comparing with a
  `float`, or two Rationals whose cross products overflow, no longer converts to floats.
- **`Rational::add()`, `sub()`, `+` and `-` only overflow when the result does**, except in rare cases, matching the
  package.
- **More `Complex` results match the package exactly:**
  - `fromPolar()` and `roots()` calculate the parts from the phase as given, instead of wrapping it first. The
    magnitude and phase are computed from the parts, as for any other `Complex`, instead of keeping the input values.
    For example, `fromPolar(0, 1)` had a phase of 1.
  - `sqr()`, `pow(2)` and the inverse functions calculate z² as `mul()` does, which differed when a product of the
    parts was subnormal.
  - The inverse functions wrap the phase of their logarithm step, as `ln()` does, so an underflowed phase gives `+0`.
- **Builds with `-ffp-contract=off`.** The compiler could fuse a multiply and an add into one instruction, which rounds
  differently from PHP. That broke exact parity with the package, e.g. in `Vector::cross()`.
- **Builds with `-fno-builtin-sin -fno-builtin-cos`.** The compiler combined `sin(x)` and `cos(x)` into one `sincos`
  call, which on macOS can round differently from PHP's `sin()`. That broke exact parity in every method that uses
  both, e.g. `fromPolar()`, `exp()`, `sin()` and `cos()`.
- **`~` on a `Vector` or `Matrix` crashed PHP.** It now throws `TypeError`, as for `Rational`.
- **Compound assignment (`+=`, `-=`, `*=`, `/=`, `**=`) and `++`/`--` leaked memory**, in all four classes. Each one
  leaked the object it replaced, so an accumulating loop grew without bound.
- **A clone lost the extension's behaviour.** `clone` on any of the four classes gave an object without its operators,
  comparison or `Vector::$magnitude`, and a clone of a cloned `Matrix` shared its rows. This also affected results built
  from a clone, such as `$v * $m`, `$v->mul($m)` and `$m->getRow()`.
- **An operator leaked a `Rational` when the result couldn't be represented**, e.g. `new Rational(PHP_INT_MIN + 1) - 1`.
- **`approxEqual()` and `approxCompare()` accepted invalid tolerances.** A negative or non-finite tolerance now throws
  `DomainException`, with the package's message.
- **`new Rational(0)->div()` returned zero for any argument**, including a string or `NAN`. It now throws `TypeError`
  for an argument of the wrong type, like the package.
- **The extension couldn't be built on Windows.** `config.w32` was missing a source file and called a function the
  Windows build system doesn't have, and the integer overflow checks used GCC/Clang builtins that MSVC lacks. It now
  builds with MSVC for PHP 8.4 and 8.5, thread-safe or not, and passes the full test suite.
- **A thread-safe Windows build crashed** on its first access to an engine global, such as when throwing an exception.
  The module never filled its per-thread cache, which Windows builds use.
- **`Vector::$magnitude` behaves like the package's property.** `isset()` and `empty()` compute it, and
  `json_encode()`, `get_object_vars()` and `var_export()` include it. Previously `isset()` was `false`, those functions
  left it out, and `var_dump()` showed it as uninitialized.
- **`TypeError` messages match the package** for parameters taking an object or a number, e.g. "must be of type
  OceanMoon\Math\Vector|OceanMoon\Math\Matrix|float, true given". Previously they used short class names, a different
  order, and the type rather than the value ("bool given").
- **`Complex::round()` and `Rational::round()` accepted `null` for `$mode`**, using the default. They now throw
  `TypeError`, like the package.
- **`Vector::mul()` and `*` with a Matrix of the wrong height** said "Must equal this Matrix's column count". The
  message now names the Vector's count, like the package.

### Documentation

- `docs/Reference/Rational.md` documents `**` with a `Rational` exponent, and has a **Throws:** block for each operator.

---
- Each operator page documents compound assignment, and `Complex` and `Rational` document `++` and `--`.
- The README's new "Argument Types" section explains that parameters taking an object or a number, such as `add()`'s,
  are always strict in the extension.

## [4.0.1] - 2026-10-10

First release. The version matches `oceanmoon/math` 4.0.1, whose API this release mirrors. The two stay in step from now
on.

### Added

- **Native `Complex`, `Rational`, `Vector` and `Matrix` classes**, matching `oceanmoon/math` 4.0.1 method for method,
  under the same fully-qualified names. Loading the extension replaces the package's classes with no code changes.
- **The `OceanMoon\Math\M_I` constant**, and native registrations of Core's `ArithmeticException` and `FormatException`,
  so the extension has no runtime dependency on any package.
- **Operator overloading:**

  | Class      | Arithmetic                                | Comparison                              |
  | ---------- | ----------------------------------------- | --------------------------------------- |
  | `Complex`  | `+`, `-`, `*`, `/`, `**`, `~` (conjugate) | `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>` |
  | `Rational` | `+`, `-`, `*`, `/`, `**`                  | `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>` |
  | `Vector`   | `+`, `-`, `*`, `/`                        | None added                              |
  | `Matrix`   | `+`, `-`, `*`, `/`, `**` (`int` exponent) | None added                              |

  - `Complex` and `Rational` accept an `int` or `float` operand on either side.
  - A `float` operand widens a `Rational` operation to a `float` result. An `int` keeps it exact.
  - `Matrix * Vector` is equivalent to `mul()` with a Vector, and `Vector * Matrix` to `Vector::mul()`.
  - `int|float / Vector` and `int|float / Matrix` divide the scalar by each element, like `$x * $v->reciprocal()`.
  - Unsupported forms, such as `Vector * Vector` or `Matrix / Matrix`, throw `TypeError`.
  - `Complex` ordering is lexicographic (real part, then imaginary). It's useful for sorting, but not mathematically
    meaningful.
- **Cloning a `Matrix`** deep-clones its row vectors, matching the package's `Matrix::__clone()`.
- **PHP 8.4 and 8.5 support**, NTS and ZTS. The published build targets 8.4.
- **PHPStan operator extensions** (`phpstan/`), so static analysis understands the operators. They're registered in this
  repo's own `phpstan.neon` and can't yet be used from another project. See `docs/Usage/Static_Analysis.md`.
- **Conformance test suite** - runs `oceanmoon/math`'s own PHPUnit tests against the extension's classes, plus the
  extension's own operator tests.
- **Scripts** - `composer build`, `test`, `enable` and `disable`, each taking an optional PHP version.
- **Documentation** - an operator reference page for each class, a comparison operators page, and installation,
  development and static analysis guides.
