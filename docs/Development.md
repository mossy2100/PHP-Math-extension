# Development

Building, testing, and the project layout, for anyone working on the extension's C source itself. If you just want to
*install* the extension to use it in a project, see [Installation](Installation.md) instead - manual installation
links back to the Building section below, but that's the only overlap.

---

## Building

Start in the root of the project. This builds against whichever PHP is first on your `PATH` (`phpize` and
`./configure` both resolve their toolchain from it) - check with `php -v` before building. To build for a different
installed version (see the main `README.md`'s "Why PHP 8.4?"), point `PHP_CONFIG` at that version's `php-config`
before running `phpize`.

```bash
phpize
./configure --enable-oceanmoon_math
make
```

Or, more simply:

```bash
composer build
```

`composer build` (`scripts/build`) refuses to run against anything other than PHP 8.4.x/8.5.x, rather than risk
silently producing an ABI-mismatched build against an unsupported version. The raw `phpize`/`configure`/`make`
commands above have no such check.

Confirm the extension loads:

```bash
php -d extension="$PWD/modules/oceanmoon_math.so" -m | grep oceanmoon_math
```

You should see simply `oceanmoon_math`.

---

## Testing

```bash
make test               # .phpt tests (tests/phpt/)
scripts/test-phpunit    # PHPUnit conformance tests against the Math package's own tests (tests/phpunit/)
```

---

## Project Structure

- Top level: `oceanmoon_math.c` (MINIT/RINIT/MINFO/module entry), `php_oceanmoon_math.h`, and `oceanmoon_math.stub.php`
  (plus its generated `oceanmoon_math_arginfo.h`) - one monolithic stub for the whole extension, following the
  convention used by most PHP core extensions (`php_dom.stub.php`, `php_reflection.stub.php`, `random.stub.php`, etc.)
  rather than one stub per class.
- `src/`: everything else.
  - `floats.c`/`.h`, `integers.c`/`.h`, `types.c`/`.h`, `exceptions.c`/`.h` - shared helpers with no class affinity.
  - One subfolder per class (`Complex/`, `Rational/`, `Vector/`, `Matrix/`): everything specific to that class - its
    `.c` implementation files and its `_internal.h`.
