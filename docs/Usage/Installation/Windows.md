# Installation: Windows

## Installation via PIE (recommended)

[PIE](https://github.com/php/pie) installs extensions on Windows too. Unlike on Mac and Linux, it doesn't build from
source there: it downloads a prebuilt DLL from the extension's GitHub release, matching your PHP's version, thread
safety and architecture.

```bash
pie install oceanmoon/math-ext
```

Each release includes DLLs for PHP 8.4 and 8.5, thread-safe (TS) or not (NTS), for 64-bit Windows. They're built by
this repository's release workflow (`.github/workflows/release.yml`), using PHP's own
[php-windows-builder](https://github.com/php/php-windows-builder) actions.

> [!NOTE]
> The extension hasn't been released yet, so there are no DLLs to download. Until there are, build it yourself (below).

---

## Manual installation

### Get the DLL

Either download the zip matching your PHP from a [release](https://github.com/mossy2100/PHP-Math-extension/releases),
or build it yourself.

Building needs a Windows machine with Visual Studio 2022 (MSVC), PHP's
[SDK binary tools](https://github.com/php/php-sdk-binary-tools) and the PHP development pack for your PHP version.
[PHP's Windows build guide](https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2) covers the setup. Then, from
this repository:

```bash
phpize
configure --enable-oceanmoon_math --with-prefix=<path to your PHP>
nmake
```

The build produces `php_oceanmoon_math.dll`, in `x64\Release` (NTS) or `x64\Release_TS` (TS). The CI workflow
(`.github/workflows/ci.yml`) runs exactly these steps, and the full test suite, on every push.

### Enable the extension

Place `php_oceanmoon_math.dll` in PHP's extension directory (`extension_dir` in `php -i`), then add this line to
`php.ini`:

```ini
extension=oceanmoon_math
```

If PHP is running as a persistent process, e.g. under IIS via FastCGI, restart it to pick up the change.

### Confirm it loaded

```bash
php -m
```

Look for `oceanmoon_math` in the list.
