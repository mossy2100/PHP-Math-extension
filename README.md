# complex

**Work in progress.** A native PHP extension providing `OceanMoon\Math\Complex` — intended as a
drop-in, faster counterpart to the `Complex` class in
[oceanmoon/math](https://github.com/mossy2100/PHP-Math).

Since both use the same fully-qualified class name, loading this extension transparently replaces
the userland class (Composer's autoloader never runs); without it, the plain PHP class is used.
The goal is to eventually let both share one PHPUnit conformance suite, and to add capabilities
userland PHP can't provide on its own — operator overloading, chiefly.

## Status

Early skeleton, built incrementally against the Math package's own tests (see `tests/phpunit/`).
Implemented so far:

- Class registration, `real` / `imaginary` properties, `__toString`
- Constructor, including the same non-finite-value validation as the PHP package
- The `OceanMoon\Math\I` constant

Not yet implemented: factory methods, arithmetic, operators, and most of the rest of `Complex`'s
API. See [packages/Math](https://github.com/mossy2100/PHP-Math) for the reference implementation
this is being built to match.

## Building

```bash
phpize
./configure --enable-complex
make
php -d extension="$PWD/modules/complex.so" -m | grep complex   # confirm it loads
```

## Testing

```bash
make test               # .phpt tests (tests/phpt/)
scripts/test-phpunit    # PHPUnit conformance tests against the Math package's own tests (tests/phpunit/)
```

## License

MIT
