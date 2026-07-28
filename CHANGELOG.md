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
- **Comparison operators for `Complex`** (`<`, `<=`, `>`, `>=`, `<=>`, and, as an unavoidable side effect of PHP wiring
  all six to the same `compare` handler, `==`/`!=`) -- not backed by any named method in the userland package, since
  comparing complex numbers isn't mathematically meaningful (no total order is compatible with the field operations).
  The ordering is plain lexicographic (real part first, then imaginary), matching what PHP's own default object
  comparison already gives two `Complex` instances for free (`$real` is declared before `$imaginary`); the operators
  add accepting an `int|float` operand on either side, promoted the same way `equal()` promotes one, and throwing
  `DomainException` for a `NAN` operand. `Complex` is now the second of the four classes (after `Rational`) with
  comparison operators; `Vector`/`Matrix` still have none. See `docs/Complex.md`.
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
    `approxEqual()`). `$magnitude` is a computed property, implemented via custom `read_property`/`write_property`
    object handlers (internal classes can't use PHP 8.4 property hooks directly) -- like the userland package's own
    `get`-only property hook, it recomputes fresh from the current elements on every read rather than caching (since
    `Vector` is mutable and a cache would need every mutating method to remember to invalidate it), and rejects
    writes with the identical "Property OceanMoon\Math\Vector::$magnitude is read-only" `Error` PHP's own hook
    machinery produces for the package's version. `vector.stub.php` also declares the property `readonly` (rather
    than mirroring the package's `get`-only property hook syntax, which `gen_stub.php` can't parse) so
    `ReflectionProperty::isReadOnly()` and PHPStan both see it as read-only too, not just the runtime
    `write_property` handler.
  - `Vector` unary and binary arithmetic (`neg()`, `reciprocal()`, `add()`, `sub()`, `mul()`, `div()`, `hadamardMul()`,
    `hadamardDiv()`), linear algebra (`dot()`, `cross()`, `outer()`, `normalized()`), and aggregation (`sum()`,
    `prod()`, `count()`).
  - `Matrix`: construction, factory methods (`fromArray()`, `identity()`), conversion (`toArray()`, `__toString()`),
    inspection (`isSquare()`, `get()`, `getRow()`, `getColumn()`, `copy()`), modification (`set()`, `setRow()`,
    `setColumn()`, `paste()`), comparison (`equal()`, `approxEqual()`), and transformation (`resize()`).
  - `Matrix` unary and binary arithmetic (`neg()`, `reciprocal()`, `inv()`, `add()`, `sub()`, `mul()`, `div()`,
    `hadamardMul()`, `hadamardDiv()`), power methods (`pow()`, `sqr()`), linear algebra (`mulVector()`, `t()`, `det()`,
    `trace()`), and norm methods (`norm()`, `p1Norm()`, `pInfNorm()`).
  - Both implement `Countable`/`ArrayAccess` (mutable, unlike `Complex`); `Matrix::offsetGet()` returns a live row
    `Vector` reference, matching the userland package's semantics for `$m[$row][$col] = $x`.
  - Cloning a `Matrix` deep-clones its row `Vector`s via a custom `clone_obj` object handler -- the one genuinely new C
    pattern this port required -- matching `Matrix::__clone()`'s fix for the same shared-row-identity bug.
- **Operator overloading for `Vector` and `Matrix`** (not possible in the userland package): `Vector` gets `+`/`-`
  (`Vector` operand only), `*` (`int|float` on either side, or `Vector * Matrix`), and `/` (`int|float` divisor, or
  `int|float` dividend -- see below). `Matrix` gets `+`/`-` (`Matrix` operand only), `*` (`int|float` on either side,
  `Matrix * Matrix`, or `Matrix * Vector` -- equivalent to `mulVector()`, resulting in a `Vector` rather than a
  `Matrix`), `/` (`int|float` divisor, or `int|float` dividend), and `**` (`int` exponent only, via `pow()`). Every
  deliberately-unsupported form documented in `docs/Vector.md`/`docs/Matrix.md` (e.g. `Vector * Vector`,
  `Matrix / Matrix`, `int ** Matrix`) throws `TypeError`, matching PHP's own "unsupported operand types" error for
  ordinary types.
- **`int|float / Vector` and `int|float / Matrix`** -- element-wise: divides the scalar by each element in turn,
  equivalent to `$x * $vector->reciprocal()`/`$x * $matrix->reciprocal()`, and throwing `ArithmeticException` for a
  zero element, matching `reciprocal()`. Not commutative with `Vector / int|float`/`Matrix / int|float` (`div()`),
  and has no equivalent named method in either direction -- `div()` only ever divides the `Vector`/`Matrix` by a
  scalar, never the reverse. On `Matrix`, this is distinct from `$x * $matA->inv()` (inverse scaling); the ambiguity
  between the two readings of `x / A` that made this form unsupported until now is resolved in favor of the
  element-wise reading, matching `reciprocal()`'s existing meaning.
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
- **PHPCS set up for `tests/`**, matching the setup already used by `Math` and the other packages: `phpcs.xml` using
  the `oceanmoon/coding-standard` `OceanMoon` ruleset, plus `composer fix`/`composer check` scripts. Fixing the
  resulting violations (mostly array-literal formatting) also turned up two real issues: `MatrixBinaryOperatorsTest.php`
  and `VectorBinaryOperatorsTest.php` used `$A`/`$B` for matrix variables, inconsistent with the project's lowerCamelCase
  convention and with `packages/Math`'s own tests (which use `$a`/`$b`); and `RationalComparisonOperatorsTest.php`'s
  `==`/`!=` operator tests were auto-"fixed" by `phpcbf` to `===`/`!==`, which silently broke them, since the whole
  point of those tests is exercising `Rational`'s custom loose-equality operator overload — reverted. (The underlying
  `SlevomatCodingStandard.Operators.DisallowEqualOperators` sniff was subsequently removed from `oceanmoon/coding-standard`
  itself, so no per-line suppression was needed.)
- **`composer quality`**, matching the composite script other `oceanmoon/*` packages already have: runs `fix`, `check`,
  `analyze`, and `test` in sequence. The `make` script was also renamed to `build`, matching `scripts/build`'s actual
  filename (it never matched the script name before).
- **New "Comparison Operators" section in `README.md`**, covering both loose (`<=>`-backed) and strict (`===`/`!==`)
  comparison across all four classes: which classes get loose operators and why (`Complex`/`Rational` do, `Vector`/
  `Matrix` don't, since neither has a natural total order over a whole element list), and that strict comparison
  needs no extension support at all -- PHP's own `===` already does the right thing for value types with only
  scalar/typed properties.

### Changed

- **`Vector::$size` renamed to `$count`**, matching the userland package's own rename (same motivation: consistency
  with `Matrix`'s `rowCount`/`columnCount`, and less risk of confusion with `$magnitude`). Purely a rename with no
  behavior change; the property is still a plain stored value (not a computed one like `$magnitude`), so no
  `read_property` handler changes were needed here -- just the property name itself, the constructor parameter, and
  every internal reference (`vector_read_size()` → `vector_read_count()`, and equivalent renames throughout
  `Vector`'s and `Matrix`'s C source). Update `$v->size` to `$v->count`.
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

### Documentation

- **The unary `+` sections in `docs/Complex.md`/`docs/Rational.md`/`docs/Vector.md`/`docs/Matrix.md` now document it as
  `clone $x`** rather than describing the underlying mechanism (no dedicated opcode, lowers to `$x * 1`, etc.) — the
  `clone` framing is both simpler and, for `Vector`/`Matrix`, directly verifiable: confirmed at runtime that `+$x` and
  `clone $x` produce value-equal, instance-distinct results, and for `Matrix` specifically, that both independently
  deep-clone row `Vector`s (via the custom `clone_obj` handler), so mutating a row of either copy never affects the
  original or the other copy.
