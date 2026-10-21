<?php

/**
 * The benchmarks behind scripts/bench, which passes the module path, the package's autoloader and the opcache options.
 *
 * Runs itself twice, once with the extension and once with the package, then prints the time per call for each and
 * the speed-up. Each timing is the best of several batches of about 50 ms, which filters out most noise.
 */

declare(strict_types=1);

use OceanMoon\Math\Complex;
use OceanMoon\Math\Matrix;
use OceanMoon\Math\Rational;
use OceanMoon\Math\Vector;

/**
 * Build an n×n Matrix of values between -1 and 1, plus n on the diagonal, so it's well-conditioned for inv().
 */
function benchMatrix(int $n, int $offset): Matrix
{
    $data = [];
    for ($i = 0; $i < $n; $i++) {
        for ($j = 0; $j < $n; $j++) {
            $data[$i][$j] = sin($i * $i * 0.37 + $j * 1.3 + $offset) + ($i === $j ? $n : 0);
        }
    }
    return Matrix::fromArray($data);
}

/**
 * Get the benchmarks, as labels mapped to closures.
 *
 * @return array<string, Closure(): mixed>
 */
function benchmarks(): array
{
    $z1 = new Complex(1.5, -2.5);
    $z2 = new Complex(-0.5, 3.25);
    $r1 = new Rational(3, 7);
    $r2 = new Rational(-5, 11);
    $v1 = Vector::fromArray([1.0, 2.0, 3.0]);
    $v2 = Vector::fromArray([4.0, -5.0, 6.0]);
    $a4 = benchMatrix(4, 0);
    $b4 = benchMatrix(4, 1);
    $a50 = benchMatrix(50, 0);
    $b50 = benchMatrix(50, 1);

    return [
        '`Complex::mul()`'        => static fn () => $z1->mul($z2),
        '`Rational::add()`'       => static fn () => $r1->add($r2),
        '`Vector::dot()` (3D)'    => static fn () => $v1->dot($v2),
        '`Matrix::mul()` (4×4)'   => static fn () => $a4->mul($b4),
        '`Matrix::add()` (50×50)' => static fn () => $a50->add($b50),
        '`Matrix::mul()` (50×50)' => static fn () => $a50->mul($b50),
        '`Matrix::inv()` (50×50)' => static fn () => $a50->inv(),
    ];
}

/**
 * Time one benchmark, in microseconds per call.
 *
 * @param Closure(): mixed $fn
 */
function timeBenchmark(Closure $fn): float
{
    // Find a batch size that takes about 50 ms.
    $iterations = 1;
    while (true) {
        $start = hrtime(true);
        for ($i = 0; $i < $iterations; $i++) {
            $fn();
        }
        $elapsed = hrtime(true) - $start;
        if ($elapsed >= 50_000_000) {
            break;
        }
        $iterations *= 2;
    }

    // Take the best of five batches.
    $best = $elapsed / $iterations;
    for ($batch = 0; $batch < 5; $batch++) {
        $start = hrtime(true);
        for ($i = 0; $i < $iterations; $i++) {
            $fn();
        }
        $best = min($best, (hrtime(true) - $start) / $iterations);
    }

    return $best / 1000;
}

/**
 * Format a time in microseconds with a sensible unit.
 */
function formatTime(float $us): string
{
    return $us >= 1000 ? sprintf('%.2f ms', $us / 1000) : sprintf('%.2f µs', $us);
}

// Child: run the benchmarks and print the timings as JSON.
if (($argv[1] ?? '') === '--child') {
    if ($argv[2] !== '') {
        require $argv[2];
    }
    $times = array_map(timeBenchmark(...), benchmarks());
    echo json_encode($times, JSON_THROW_ON_ERROR);
    exit;
}

// Parent: run a child with the extension, then one with the package, and print the table.
[
    ,
    $module,
    $autoload,
] = $argv;
$opcache = array_slice($argv, 3);

/**
 * Run a child process and decode its timings.
 *
 * @param list<string> $phpOptions
 * @return array<string, float>
 */
function runChild(array $phpOptions, string $autoload): array
{
    $command = array_merge([PHP_BINARY, '-n'], $phpOptions, [
        __FILE__,
        '--child',
        $autoload,
    ]);
    $output = shell_exec(implode(' ', array_map(escapeshellarg(...), $command)));
    if (!is_string($output)) {
        fwrite(STDERR, "The benchmark process failed.\n");
        exit(1);
    }
    /** @var array<string, float> */
    return json_decode($output, true, flags: JSON_THROW_ON_ERROR);
}

$extension = runChild(array_merge($opcache, [
    '-d',
    "extension=$module",
]), '');
$package = runChild($opcache, $autoload);

$rows = [
    ['Operation', 'Extension', 'Package', 'Speed-up'],
];
foreach ($extension as $label => $time) {
    $rows[] = [
        $label,
        formatTime($time),
        formatTime($package[$label]),
        sprintf('%.1f×', $package[$label] / $time),
    ];
}

// Print a Markdown table, padded so the columns line up.
$widths = [];
foreach ($rows as $row) {
    foreach ($row as $i => $cell) {
        $widths[$i] = max($widths[$i] ?? 0, mb_strlen($cell));
    }
}
$line = static fn (array $cells): string => '| ' . implode(' | ', array_map(
    static fn (string $cell, int $i): string => $cell . str_repeat(' ', $widths[$i] - mb_strlen($cell)),
    $cells,
    array_keys($cells)
)) . ' |';
echo $line($rows[0]), "\n";
echo $line(array_map(static fn (int $width): string => str_repeat('-', $width), $widths)), "\n";
foreach (array_slice($rows, 1) as $row) {
    echo $line($row), "\n";
}
