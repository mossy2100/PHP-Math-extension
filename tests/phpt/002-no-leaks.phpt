--TEST--
Operators, compound assignment, ++/-- and clone don't leak, on success or failure
--DESCRIPTION--
Exercises every operator and compound assignment, ++ and --, on all four classes, on local variables, array elements,
and untyped and typed properties, including the failure paths. Under a debug PHP build, any leaked allocation is
reported at shutdown, which makes the output differ from --EXPECT-- and fails the test. CI runs this in its debug job.
--EXTENSIONS--
oceanmoon_math
--FILE--
<?php
declare(strict_types=1);

use OceanMoon\Math\Complex;
use OceanMoon\Math\Matrix;
use OceanMoon\Math\Rational;
use OceanMoon\Math\Vector;

final class Holder
{
    public mixed $value;
    public Complex $complex;
    public Rational $rational;
    public Vector $vector;
    public Matrix $matrix;
}

function attempt(callable $f): void
{
    try {
        $f();
    } catch (Throwable) {
    }
}

$m = fn() => Matrix::fromArray([[1, 2], [3, 4]]);
$v = fn() => Vector::fromArray([1, 2]);

for ($i = 0; $i < 3; $i++) {
    // Local variables, every compound operator, success.
    $z = new Complex(1, 2);
    $z += new Complex(1, 1); $z -= 1; $z *= 2; $z /= 2; $z **= 2; $z++; $z--; ++$z; --$z;
    $keep = $z; $z += 1; $post = $z++;

    $r = new Rational(1, 2);
    $r += new Rational(1, 3); $r -= 1; $r *= 2; $r /= 3; $r **= 2; $r++; $r--; ++$r; --$r;
    $keep = $r; $r += 1; $post = $r--;
    $rf = new Rational(1, 2); $rf += 0.25;

    $vec = $v();
    $vec += $v(); $vec -= $v(); $vec *= 2; $vec /= 2; $vec *= $v(); $vec /= $v(); $vec *= $m();
    $vec = $v(); $keep = $vec; $vec *= 3;

    $mat = $m();
    $mat += $m(); $mat -= $m(); $mat *= 2; $mat /= 2; $mat *= $m(); $mat **= 3; $mat **= -1;
    $keep = $mat; $mat *= 3;

    // Scalar on the left, and the same variable on both sides.
    $x = 2; $x *= new Complex(1, 2);
    $x = 2; $x *= new Rational(1, 3);
    $x = 2.0; $x *= $v();
    $x = 2; $x *= $m();
    $z = new Complex(1, 2); $z += $z;
    $mat = $m(); $mat *= $mat;

    // Failures: exception from the calculation, and unsupported operands.
    attempt(function () { $z = new Complex(1, 2); $z /= 0; });
    attempt(function () { $r = new Rational(1, 2); $r /= 0; });
    attempt(function () { $r = new Rational(PHP_INT_MAX); $r += 1; });
    attempt(function () { $r = new Rational(PHP_INT_MAX); $r++; });
    attempt(function () { $r = new Rational(PHP_INT_MIN + 1); $r--; });
    attempt(function () use ($v) { $vec = $v(); $vec += 1; });
    attempt(function () use ($v) { $vec = $v(); $vec++; });
    attempt(function () use ($v) { $vec = $v(); $vec += Vector::fromArray([1, 2, 3]); });
    attempt(function () use ($m) { $mat = $m(); $mat **= 0.5; });
    attempt(function () use ($m) { $mat = $m(); $mat--; });
    attempt(function () use ($m) { $mat = Matrix::fromArray([[1, 2], [2, 4]]); $mat **= -1; });
    attempt(function () use ($v) { $vec = $v(); ~$vec; });
    attempt(function () use ($m) { $mat = $m(); ~$mat; });

    // Array elements.
    $arr = ['z' => new Complex(1, 2), 'r' => new Rational(1, 2), 'v' => $v(), 'm' => $m()];
    $arr['z'] += 1; $arr['z']++; $arr['r'] *= 2; $arr['r']--; $arr['v'] *= 2; $arr['m'] **= 2;
    attempt(function () use (&$arr) { $arr['r'] /= 0; });

    // Untyped and typed properties.
    $h = new Holder();
    $h->value = new Complex(1, 2); $h->value += 1; $h->value++;
    $h->complex = new Complex(1, 2); $h->complex *= 2; $h->complex++;
    $h->rational = new Rational(1, 2); $h->rational += 1; ++$h->rational;
    $h->vector = $v(); $h->vector *= 2;
    $h->matrix = $m(); $h->matrix **= 2;
    attempt(function () use ($h) { $h->rational /= 0; });
    attempt(function () use ($h) { $h->rational += 0.5; }); // float result rejected by the typed property
    attempt(function () use ($h) { $h->vector++; });
}

echo "done\n";
?>
--EXPECT--
done
