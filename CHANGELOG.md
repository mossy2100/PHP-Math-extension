# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

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
  - `Matrix * Vector` is equivalent to `mulVector()`, and `Vector * Matrix` to `Vector::mul()`.
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
