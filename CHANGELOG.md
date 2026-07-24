# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added

- **`OceanMoon\Math\Complex` is now fully implemented natively**, matching the userland package method-for-method:
  - Construction, factory methods (`fromString()`, `fromPolar()`), conversion (`__toString()`), inspection (`isReal()`),
    and comparison (`equal()`, `approxEqual()`).
  - Unary and binary arithmetic (`neg()`, `inv()`, `conj()`, `add()`, `sub()`, `mul()`, `div()`).
  - Power and root methods (`pow()`, `sqr()`, `roots()`, `sqrt()`).
  - Transcendental methods (`exp()`, `ln()`, `log()`).
  - Trigonometric and inverse trigonometric methods (`sin()`, `cos()`, `tan()`, `asin()`, `acos()`, `atan()`).
  - Hyperbolic and inverse hyperbolic methods (`sinh()`, `cosh()`, `tanh()`, `asinh()`, `acosh()`, `atanh()`).
  - `round()`, including full `RoundingMode` support.
  - `ArrayAccess` (`$z[0]`/`$z[1]` for the real/imaginary parts; `offsetSet()`/`offsetUnset()` throw, since `Complex` is
    immutable).
- **Operator overloading for `Complex`** (not possible in the userland package): `+`, `-`, `*`, `/`, `**` each accept a
  `Complex` or `int|float` operand on either side, and `~` for the complex conjugate. See `docs/Complex.md`.
- **`OceanMoon\Math\Rational` is now fully implemented natively**, matching the userland package method-for-method:
  - Construction, factory methods (`fromFloat()`, `fromString()`), conversion (`toFloat()`, `toMixedNumber()`,
    `__toString()`), and comparison (`compare()`, `equal()`, `approxEqual()`, `lessThan()`, etc.).
  - Unary and binary arithmetic (`neg()`, `inv()`, `add()`, `sub()`, `mul()`, `div()`).
  - `pow()` and `round()`, including full `RoundingMode` support.
- **Operator overloading for `Rational`** (not possible in the userland package): `+`, `-`, `*`, `/` each accept a
  `Rational`, `int`, or `float` operand on either side, and `**` accepts `Rational ** int`, `Rational ** float`, or
  `float ** Rational`. A genuine `float` operand widens the whole operation to `float` -- something none of the
  equivalent methods support -- while an `int` operand widens to an exact `Rational`. `Rational` also gets comparison
  operators (`<`, `<=`, `>`, `>=`, `<=>`, `==`, `!=`), the only one of the four classes to do so, since it has a genuine
  natural ordering. See `docs/Rational.md`.
- **`OceanMoon\Math\Vector` and `OceanMoon\Math\Matrix` are now fully implemented natively**, ported together since
  they're mutually dependent in the userland package, matching it method-for-method:
  - `Vector`: construction, `fromArray()`, conversion (`toArray()`, `toRowMatrix()`, `toColumnMatrix()`,
    `__toString()`), inspection (`get()`), modification (`set()`, `normalize()`), and comparison (`equal()`,
    `approxEqual()`).
  - `Vector` unary and binary arithmetic (`neg()`, `reciprocal()`, `add()`, `sub()`, `mul()`, `div()`, `hadamardMul()`,
    `hadamardDiv()`), linear algebra (`dot()`, `cross()`, `outer()`, `normalized()`), and aggregation (`sum()`,
    `prod()`, `count()`).
  - `Matrix`: construction, factory methods (`fromArray()`, `identity()`), conversion (`toArray()`, `__toString()`),
    inspection (`isSquare()`, `get()`, `getRow()`, `getColumn()`, `copy()`), modification (`set()`, `setRow()`,
    `setColumn()`, `paste()`), comparison (`equal()`, `approxEqual()`), and transformation (`resize()`).
  - `Matrix` unary and binary arithmetic (`neg()`, `reciprocal()`, `inv()`, `add()`, `sub()`, `mul()`, `div()`,
    `hadamardMul()`, `hadamardDiv()`), power methods (`pow()`, `sqr()`), linear algebra (`t()`, `det()`, `trace()`), and
    norm methods (`norm()`, `p1Norm()`, `pInfNorm()`).
  - Both implement `Countable`/`ArrayAccess` (mutable, unlike `Complex`); `Matrix::offsetGet()` returns a live row
    `Vector` reference, matching the userland package's semantics for `$m[$row][$col] = $x`.
  - Cloning a `Matrix` deep-clones its row `Vector`s via a custom `clone_obj` object handler -- the one genuinely new C
    pattern this port required -- matching `Matrix::__clone()`'s fix for the same shared-row-identity bug.
- **Operator overloading for `Vector` and `Matrix`** (not possible in the userland package): `Vector` gets `+`/`-`
  (`Vector` operand only), `*` (`int|float` on either side, or `Vector * Matrix`), and `/` (`int|float` divisor only).
  `Matrix` gets `+`/`-` (`Matrix` operand only), `*` (`int|float` on either side, `Matrix * Matrix`, or
  `Matrix * Vector` -- resulting in a `Vector` rather than a `Matrix`, with no equivalent named method; see
  `Vector::mul()`/`Matrix::mul()`), `/` (`int|float` divisor only), and `**` (`int` exponent only, via `pow()`). Every
  deliberately-unsupported form documented in `docs/Vector.md`/`docs/Matrix.md` (e.g. `Vector * Vector`, `int / Vector`,
  `Matrix / Matrix`, `int ** Matrix`) throws `TypeError`, matching PHP's own "unsupported operand types" error for
  ordinary types.
- **`Matrix::mulVector()`** — added earlier in this Unreleased cycle, removed again for the same reason `Vector::mul()`
  dropped its `Matrix` operand (see the PHP package's CHANGELOG): a dedicated method for "matrix times vector" (_Ax_)
  forces an unsatisfying choice between a `self|Vector` union return type (which PHPStan can't narrow without
  `assert()`, and which breaks the fluent API) or a single-column `Matrix` (not what callers actually want from _Ax_).
  Use the `*` operator instead (`$A * $v`), which isn't constrained by a declared return type; see `docs/Matrix.md` and
  the PHP package's `Matrix::mul()` docblock for the named-method alternative.
- **`docs/Static Analysis.md`**, explaining how PHPStan is taught to understand this extension's operator overloads
  (PHPStan's `OperatorTypeSpecifyingExtension`/`UnaryOperatorTypeSpecifyingExtension` mechanism — the same one used for
  GMP/BCMath), using the existing `phpstan/*OperatorExtension.php` classes and `phpstan.neon` registrations as the
  worked example. Also documents a real gap found while writing it: those classes currently live under `autoload-dev`,
  so a project that just `composer require`s this extension has no way to actually use them in its own `phpstan.neon`
  yet — tracked there as a known limitation, not yet fixed.
- **`docs/Installation/`**, splitting installation instructions into dedicated Mac/Linux, Windows, Docker, and Remote
  (VPS/cloud) pages, replacing a single flat section. The Windows page in particular is now explicit that no prebuilt
  binary exists yet and why PIE can't build one for you there (it only builds from source on non-Windows platforms —
  extension maintainers must supply prebuilt Windows binaries).

### Changed

- **The planned `Vector`/`Matrix` operator surface (`docs/Vector.md`, `docs/Matrix.md`) drops two forms** ahead of
  implementation, mirroring the userland `Math` package's own `Matrix::div()` narrowing this release:
  - `Vector * Vector` isn't mapped to an operator - with `dot()`, `cross()`, `hadamardMul()`, and now `outer()` all
    valid readings of "multiply two vectors", the operator itself would be ambiguous about which one it means. Call the
    specific method instead.
  - `Matrix / Matrix` and `int|float / Matrix` are dropped for the same order-ambiguity reason: `A × B⁻¹` and `B⁻¹ × A`
    differ in general (matrix multiplication isn't commutative), so `/` between matrices doesn't have one obvious
    meaning. Use `$a->mul($b->inv())` directly to express the specific order you want.
- **`docs/Rational.md`'s comparison operators (`<`, `<=`, `>`, `>=`, `==`, `!=`) each get their own section**, matching
  the existing per-operator treatment `+`/`-`/`*`/`/`/`**` already had, instead of being covered together in one
  combined section. Each now has its own equivalence table mapping every operand-order form to the userland package's
  `Comparable`-trait methods (`lessThan()`, `greaterThan()`, `equal()`, etc.).
- **Equivalence tables across `docs/Complex.md`, `docs/Rational.md`, and `docs/Matrix.md` reviewed for a consistent
  rule**: the first "Equivalent to" column always keeps the operands in the order written; a second "Also equivalent"
  column, when present, is a genuinely shorter alternative — not just another way to write the same thing. Added missing
  concise alternatives where one exists (e.g. `$x + $z` → also `$z->add($x)`; `$i - $r` → also `-$r->sub($i)`, verified
  against the identity `i - r = -(r - i)`), removed several `<=>`/`<`/`<=`/`>`/`>=` cells that showed a `->toFloat()`
  form no shorter than the primary, and split `Matrix.md`'s `$A * $v` row (previously both forms crammed into one cell
  with "or") into proper primary/concise columns.
- **`docs/Rational.md`'s `!=` equivalence table expanded** from combined `$x` (`int|float`) rows to separate `$i`/`$f`
  rows, matching `==`'s structure, gaining its own "Also equivalent" column in the process.
- **README.md restructured** to match the sibling packages' layout: `Description` and
  `Development and Quality Assurance` sections added, `Requirements` moved up to directly follow them, and
  `Installation` (now linking out to `docs/Installation/`) inserted after that.

### Fixed

- A `read_property` object-handler bug where the engine's opcode-level inline property cache could bypass the lazy
  compute-and-cache logic for the `magnitude`/`phase` computed properties on the second and subsequent reads at the same
  bytecode location (e.g. inside a loop), returning a stale `null` instead of the computed value.
- **Several correctness bugs in the planned `Vector`/`Matrix` operator documentation**, caught before implementation: a
  fabricated `ZEND_UNARY_PLUS` handler claim in `Matrix.md` (PHP has no such opcode; unary `+`/`-` actually lowers to
  the scalar `*` form, as `Complex.md`/`Rational.md`/`Vector.md` already correctly documented), backwards row/column
  dimension rules for `Matrix * Matrix` and `Vector * Matrix`, a `Matrix.md` `*` section that cited `$a->mul($other)`
  for the `Matrix * Vector` form even though that's actually `mulVector()`, and a "reversed operands" formula in both
  files that called a nonexistent `Vector::t()` method (verified numerically that dropping the trailing `->t()` entirely
  is both correct and all that's needed, since a `Vector` has no row/column orientation to transpose).
- **`docs/Rational.md`'s `==` equivalence table referenced an undefined `$x` variable** in two cells (leftover from an
  earlier combined-`$x` version of the table, never updated when it was split into separate `$i`/`$f` rows) - corrected
  to `$r->equal($i)` in both places.

### Removed

- **`composer deploy` (`scripts/deploy`)** — it copied the built module into PHP's real `extension_dir` under its bare
  filename, but nothing in the normal `composer enable` workflow (`scripts/global-install`) ever reads that copy:
  `enable` points PHP directly at this repo's own build via a full path, so the file `deploy` produced was dead weight,
  not a safety mechanism for keeping a stable global build isolated from local rebuilds.
