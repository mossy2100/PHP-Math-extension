# math

**Work in progress.** A native PHP extension intended to eventually replicate all of
[oceanmoon/math](https://github.com/mossy2100/PHP-Math) — `Complex`, `Rational`, `Vector`, and
`Matrix` — as a drop-in, faster counterpart to the pure-PHP package. `Complex` is the first (and so
far only) class implemented.

Since each class uses the same fully-qualified name as its PHP package counterpart, loading this
extension transparently replaces the userland class (Composer's autoloader never runs); without it,
the plain PHP class is used. The goal is to eventually let both share one PHPUnit conformance suite,
and to add capabilities userland PHP can't provide on its own — operator overloading, chiefly.

## Status

`OceanMoon\Math\Complex`, built incrementally against the Math package's own tests (see
`tests/phpunit/`). Implemented so far:

- Class registration, `real` / `imaginary` properties
- Constructor, including the same non-finite-value validation as the PHP package
- Factory methods: `fromArray()` (list and associative), `fromObject()`, `toComplex()`
- Conversion methods: `toArray()`, `toObject()`, `__toString()` (byte-for-byte parity with the PHP
  package's native `(string)`-cast-based formatting)
- Inspection: `isReal()`
- Comparison methods: `equal()`, `approxEqual()`
- The `OceanMoon\Math\I` constant

In progress: `==`/`!=` operator overloading (needs a custom object handler; see
`tests/phpunit/Complex/extension-only/ComplexComparisonOperatorsTest.php`).

Not yet implemented: the rest of the arithmetic/comparison operators, `parse()`, `fromPolar()`,
trigonometric/hyperbolic methods, `ArrayAccess`, serialization, and the rest of `Complex`'s API —
nor any of `Rational`, `Vector`, or `Matrix`. See [packages/Math](https://github.com/mossy2100/PHP-Math)
for the reference implementation this is being built to match.

## Layout

- Top level: module-wide files only (`math.c` — MINIT/RINIT/MINFO/module entry; `floats.c`/`floats.h`
  — shared helpers with no class affinity, e.g. the `Floats::approxEqual()` port used by comparison
  methods).
- One subfolder per class (`Complex/` so far): everything specific to that class — its `.c`
  implementation files split by region (construction/factories, conversion, inspection, comparison,
  ...), its `_internal.h`, its stub, and its generated arginfo header. `Rational/`, `Vector/`, and
  `Matrix/` will follow the same pattern.

## Building

```bash
phpize
./configure --enable-math
make
php -d extension="$PWD/modules/math.so" -m | grep math   # confirm it loads
```

## Testing

```bash
make test               # .phpt tests (tests/phpt/)
scripts/test-phpunit    # PHPUnit conformance tests against the Math package's own tests (tests/phpunit/)
```

## License

MIT
