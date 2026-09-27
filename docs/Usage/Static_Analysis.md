# Static Analysis with PHPStan

Operator overloading only exists at the C level, via the `do_operation`/`compare` object handlers - there's no PHP source text anywhere that declares "`Vector` supports `*`".

Static analysis tools work from PHP source (plus, for classes like these, the `.stub.php` files that
document their method signatures), so without help they have no way to know these operators exist. Left alone, a tool
will treat `$v * 2` exactly like `$anyOtherObject * 2`: an error, because plain PHP objects don't support arithmetic
operators at all.

This page explains how to teach PHPStan about the operators this extension adds. Other tools will need their own solution.

## The problem, concretely

Without any help, PHPStan reports every operator use on `Complex`/`Rational`/`Vector`/`Matrix` as an error:

There is no error using comparison operators and objects of the same type. Errors will only occur when comparing objects with scalars, or using arithmetic operators with objects.

Example:

```
Binary operation "*" between OceanMoon\Math\Vector and int results in an error.
```

The tempting-but-wrong fix is scattering `@phpstan-ignore binaryOp.invalid` over every call site - that silences the
error without telling PHPStan what the _result type_ actually is, so `($v * 2)->get(0)` still doesn't type-check
(PHPStan doesn't know `$v * 2` is a `Vector`, so it doesn't know `get()` exists on it).

## The real fix: `OperatorTypeSpecifyingExtension`

PHPStan has a first-class extension point for exactly this situation -
[`OperatorTypeSpecifyingExtension`](https://phpstan.org/developing-extensions/operator-type-specifying-extension) (for
binary operators) and `UnaryOperatorTypeSpecifyingExtension` (for unary `+`/`-`/`~`). This isn't a workaround specific
to this extension - it's the same mechanism PHPStan's own bundled extensions use for GMP and BCMath, which have the
identical problem (operators implemented in C, invisible to static analysis otherwise).

Each interface has two methods:

- `isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool` (binary) /
  `(string $operatorSigil, Type $operand): bool` (unary) - given the operator (`'+'`, `'*'`, etc.) and the inferred
  type(s) of the operand(s), return whether _this_ extension knows how to handle that specific combination. Returning
  `false` lets PHPStan fall through to another registered extension, or its own default "invalid operand" error if
  nothing claims it.
- `specifyType(...): Type` - given the same inputs (once `isOperatorSupported()` has already said yes), return the
  PHPStan `Type` the expression evaluates to.

This repo already has a complete, working set of these - one binary + one unary extension per class, plus a separate
comparison extension for `Complex` and `Rational` (the two classes with comparison operators), since those are backed
by a distinct `compare` object handler, not `do_operation`:

| File                                              | Backs                                   |
| ------------------------------------------------- | --------------------------------------- |
| `phpstan/ComplexBinaryOperatorExtension.php`      | `+`, `-`, `*`, `/`, `**`                |
| `phpstan/ComplexUnaryOperatorExtension.php`       | unary `+`, `-`, `~`                     |
| `phpstan/ComplexComparisonOperatorExtension.php`  | `<`, `<=`, `>`, `>=`, `<=>`, `==`, `!=` |
| `phpstan/RationalBinaryOperatorExtension.php`     | `+`, `-`, `*`, `/`, `**`                |
| `phpstan/RationalUnaryOperatorExtension.php`      | unary `+`, `-`                          |
| `phpstan/RationalComparisonOperatorExtension.php` | `<`, `<=`, `>`, `>=`, `<=>`, `==`, `!=` |
| `phpstan/VectorBinaryOperatorExtension.php`       | `+`, `-`, `*`, `/`                      |
| `phpstan/VectorUnaryOperatorExtension.php`        | unary `+`, `-`                          |
| `phpstan/MatrixBinaryOperatorExtension.php`       | `+`, `-`, `*`, `/`, `**`                |
| `phpstan/MatrixUnaryOperatorExtension.php`        | unary `+`, `-`                          |

## Anatomy of one extension

`VectorBinaryOperatorExtension` is a good one to read first - it's short, and it shows the pattern of narrowing
`isOperatorSupported()` per-operator to match exactly what `vector_do_operation()` (`src/Vector/vector_operators.c`)
actually accepts at runtime (no more, no less):

```php
final class VectorBinaryOperatorExtension implements OperatorTypeSpecifyingExtension
{
    private const OPERATORS = ['+', '-', '*', '/'];

    public function isOperatorSupported(string $operatorSigil, Type $leftSide, Type $rightSide): bool
    {
        if (!in_array($operatorSigil, self::OPERATORS, true)) {
            return false;
        }

        $vectorType = new ObjectType(Vector::class);
        $leftIsVector = $vectorType->isSuperTypeOf($leftSide)->yes();
        $rightIsVector = $vectorType->isSuperTypeOf($rightSide)->yes();

        // + and - only accept Vector + Vector - no scalar form exists.
        if ($operatorSigil === '+' || $operatorSigil === '-') {
            return $leftIsVector && $rightIsVector;
        }

        // ...
    }

    public function specifyType(string $operatorSigil, Type $leftSide, Type $rightSide): Type
    {
        return new ObjectType(Vector::class);
    }
}
```

Two things worth noting from the fuller set of extensions:

- **The result type isn't always a constant.** `RationalBinaryOperatorExtension::specifyType()` has to compute the
  return type from the operand types, because a genuine `float` operand widens the whole expression to `float` (matching
  this extension's numeric-widening behavior for `Rational` - see `docs/Rational.md`), while an `int` operand widens to
  `Rational`. `MatrixBinaryOperatorExtension::specifyType()` similarly special-cases `Matrix * Vector` to return
  `Vector` rather than `Matrix` (equivalent to `mulVector()`'s old behavior).
- **Cross-class operators are split by which side is "self".** `Vector * Matrix` is handled entirely by
  `VectorBinaryOperatorExtension` (since `Vector` is always the left operand there), while `Matrix * Vector` is handled
  entirely by `MatrixBinaryOperatorExtension` - each extension only claims the forms its own class's `do_operation`
  handler actually receives as `op1`. See the doc comments on `vector_do_operation()`/ `matrix_do_operation()` in the C
  source for why the engine dispatches that way.

## Registering the extensions

Once the class exists, it needs registering as a service in `phpstan.neon`, tagged with
`phpstan.broker.operatorTypeSpecifyingExtension` (binary) or `phpstan.broker.unaryOperatorTypeSpecifyingExtension`
(unary):

```neon
services:
    -
        class: OceanMoon\Math\PHPStan\VectorBinaryOperatorExtension
        tags:
            - phpstan.broker.operatorTypeSpecifyingExtension
    -
        class: OceanMoon\Math\PHPStan\VectorUnaryOperatorExtension
        tags:
            - phpstan.broker.unaryOperatorTypeSpecifyingExtension
```

See `phpstan.neon` in this repo for the complete, currently-working set of ten registrations (one per row in the table
above).

## The classes also have to be visible to PHPStan as class types at all

Registering the operator extensions solves "does PHPStan understand what `*` means", but PHPStan separately needs to
know `Complex`/`Rational`/`Vector`/`Matrix` _exist_ as classes with the methods/properties `oceanmoon_math.stub.php`
declares - the compiled `.so` obviously isn't something PHPStan can read. That's what `scanFiles` in `phpstan.neon` is
for:

```neon
parameters:
    scanFiles:
        - oceanmoon_math.stub.php
```

`oceanmoon_math.stub.php` is the exact source of truth already used to generate the C arginfo (`gen_stub.php` - see the
`Building` section of [Development](Development.md#building)), so this is the same signatures PHP itself uses at
runtime, not a separate hand-maintained copy that could drift.

## Constants registered at runtime

`OceanMoon\Math\M_I` (the `Complex(0, 1)` constant) has the same fundamental problem as the operators, for a different
reason: it's registered purely at request start via `zend_register_constant()` in `complex_rinit()`
(`src/Complex/complex.c`), fresh on every request, so there's no PHP source text anywhere for PHPStan to discover it in.
`gen_stub.php` can't generate a declaration for it either (constant expressions have to be a "global, non-magic
constant" - object instantiation isn't allowed). The fix is the same shape as the class-visibility one above: a
PHPStan-only file (`phpstan/constants.php`) that declares `const M_I = new Complex(0, 1);` purely for PHPStan to see,
listed in `scanFiles` alongside the stubs. It's never processed by `gen_stub.php` and never executed at runtime - it
exists solely for this purpose.

## Known limitation: not yet usable from a consuming project

Everything above documents how PHPStan is made to understand **this repo's own code** (`tests/phpunit/`, per
`phpstan.neon`'s `paths:`). The `phpstan/` extension classes are currently registered under `autoload-dev` in
`composer.json`:

```json
"autoload-dev": {
    "psr-4": {
        "OceanMoon\\Math\\PHPStan\\": "phpstan/"
    }
}
```

`autoload-dev` is Composer's mechanism for autoloading needed _to develop this package itself_ - it's never included
when another project does `composer require oceanmoon/math-ext`. That means, as things stand today, a project consuming
this extension has no way to `require` these extension classes into their own `phpstan.neon` at all - the files simply
aren't shipped to them. Before this can be documented as "how to set PHPStan up in _your_ project using this extension",
the extension classes need to move to a real (non-dev) autoload section, or be split into a separate, always-shipped
location. **TODO**, tracked here rather than fixed yet.
