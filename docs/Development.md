# Development

Building, testing, and the project layout, for anyone working on the extension's C source itself. If you just want to
*install* the extension to use it in a project, see [Installation](Installation.md) instead - manual installation
links back to the Building section below, but that's the only overlap.

---

## Building

```bash
composer build          # PHP 8.4 (default)
composer build 8.5      # PHP 8.5
PHP_VERSION=8.5 composer build
```

`composer build` (`scripts/build`) resolves its own toolchain via `scripts/php-env`, rather than inheriting whichever
`php`/`phpize` happen to be first on your `PATH`. That matters on macOS: the 8.4 Homebrew formula is keg-only, so
`/opt/homebrew/bin/php` is 8.5, and a PATH-based build silently targets the wrong version. Only 8.4 and 8.5 are
supported, and anything else is refused rather than producing an ABI-mismatched build.

`PHP_VERSION` selects the version for the other scripts too: `composer test` (`scripts/test-phpunit`) and
`composer enable` (`scripts/enable`) both honour it, and each uses the module built for that version.

### Out-of-tree builds

Each version is built in its own tree under `build/`, which `scripts/build` recreates from a fresh copy of the compile
inputs each time:

```text
build/8.4/     # phpize/configure/make run here; the module ends up in build/8.4/modules/
build/8.5/
```

Building out of tree isn't just tidiness - it's required. The generated `Makefile`, and the `build/Makefile.global`
that `phpize` copies verbatim from whichever PHP it was run against, hard-code `modules/` for the test, install and
clean targets. Only the build step honours a configurable output directory (`phplibdir`), so a single tree can't serve
two PHP versions however you shuffle the `.so` files: as soon as it isn't in `modules/`, `make test` and `make install`
break. One tree per version gives each its own `modules/`, and both work unmodified.

Recreating the tree from scratch each time also means stale artifacts can't accumulate. That matters here because the
Makefile's `-include *.dep` dependency files record the *absolute* PHP include paths of whichever PHP generated them,
so a `.dep` left by another version's build (or made stale by a Homebrew PHP upgrade) makes `make` fail outright.

Confirm the extension loads (adjust the version to match):

```bash
php8.4 -d extension="$PWD/build/8.4/modules/oceanmoon_math.so" -m | grep oceanmoon_math
```

You should see simply `oceanmoon_math`.

### Building by hand

`scripts/build` is a thin wrapper you can replicate. Source `php-env` first to get the toolchain for the version you
want onto `PATH`, then run the standard pipeline in a tree - and use its variables rather than hardcoding keg paths
(`/opt/homebrew/opt/php@8.4/bin`), which vary by formula name and PHP patch version. Run this from the repo root:

```bash
source scripts/php-env        # PHP_VERSION=8.5 source scripts/php-env, to build for 8.5 instead
mkdir -p "$PHP_BUILD_DIR" && cp -R config.m4 config.w32 oceanmoon_math.c php_oceanmoon_math.h \
    oceanmoon_math_arginfo.h oceanmoon_math.stub.php src "$PHP_BUILD_DIR"/
mkdir -p "$PHP_BUILD_DIR/tests" && cp -R tests/phpt "$PHP_BUILD_DIR/tests/"
cd "$PHP_BUILD_DIR"
phpize
./configure --enable-oceanmoon_math --with-php-config="$PHP_CONFIG"
make
```

Sourcing `php-env` first is the important part: `phpize` is version-specific (each PHP install ships its own, with
that version's prefix baked in), so without it `phpize` resolves to whatever is on your `PATH` - on macOS that's
`/opt/homebrew/bin/phpize`, which is 8.5.

---

## Testing

```bash
scripts/test-phpunit                           # PHPUnit conformance tests (tests/phpunit/) against the built module
(cd build/8.4 && make test TESTS=tests/phpt/)  # .phpt tests, run inside that version's build tree
```

`make test` has to run from inside a build tree, because that's what its relative `modules/` reference resolves
against. Note that `phpize` doesn't define `TESTS`, so `make test` on its own finds nothing - point it at the tests as
above, or it just reports "No tests were run".

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
