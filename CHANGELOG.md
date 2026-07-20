# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added

- **`OceanMoon\Math\Complex` is now fully implemented natively**, matching the userland package
  method-for-method:
  - Construction, factory methods (`fromString()`, `fromPolar()`), conversion (`__toString()`),
    inspection (`isReal()`), and comparison (`equal()`, `approxEqual()`).
  - Unary and binary arithmetic (`neg()`, `inv()`, `conj()`, `add()`, `sub()`, `mul()`, `div()`).
  - Power and root methods (`pow()`, `sqr()`, `roots()`, `sqrt()`).
  - Transcendental methods (`exp()`, `ln()`, `log()`).
  - Trigonometric and inverse trigonometric methods (`sin()`, `cos()`, `tan()`, `asin()`, `acos()`,
    `atan()`).
  - Hyperbolic and inverse hyperbolic methods (`sinh()`, `cosh()`, `tanh()`, `asinh()`, `acosh()`,
    `atanh()`).
  - `round()`, including full `RoundingMode` support.
  - `ArrayAccess` (`$z[0]`/`$z[1]` for the real/imaginary parts; `offsetSet()`/`offsetUnset()`
    throw, since `Complex` is immutable).
- **Operator overloading for `Complex`** (not possible in the userland package): `+`, `-`, `*`,
  `/`, `**` each accept a `Complex` or `int|float` operand on either side, and `~` for the complex
  conjugate. See `docs/Complex.md`.

### Fixed

- A `read_property` object-handler bug where the engine's opcode-level inline property cache could
  bypass the lazy compute-and-cache logic for the `magnitude`/`phase` computed properties on the
  second and subsequent reads at the same bytecode location (e.g. inside a loop), returning a
  stale `null` instead of the computed value.
