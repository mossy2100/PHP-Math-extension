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

Each class's full API (properties, factory methods, conversion, comparison, everything else) is documented in the
Math package itself - this extension is a drop-in replacement, not a different API. The pages below cover only what
the extension adds: operator overloading.

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
