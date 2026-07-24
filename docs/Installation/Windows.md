# Installation: Windows

**There is no working installation path for Windows yet.** This is a known gap, tracked as a TODO for this project
(setting up CI to build and publish a Windows DLL on every release) - until that's done, Windows support is
build-it-yourself only, and even that requires a real Windows machine.

## Why PIE doesn't help here (yet)

[PIE](https://github.com/php/pie) is the recommended installation method on Mac/Linux (see the Mac/Linux installation
doc), where it builds the extension from source itself. Windows is different: per
[PIE's own documentation for extension maintainers](https://github.com/php/pie/blob/1.4.x/docs/extension-maintainers.md),
**"For Windows systems, extension maintainers must provide pre-built binaries."** PIE has no fallback that builds from
source on Windows - if no prebuilt `.dll` has been published for this package, `pie install` simply won't work on
Windows at all, regardless of whether the package itself is published to Packagist.

No prebuilt Windows binary exists for this extension yet.

## Building it yourself

PHP's Windows build tooling ([PHP SDK Binary Tools](https://github.com/php/php-sdk-binary-tools)) requires an actual
Windows host with Visual Studio (VC++ 2017/2019 or later) installed - it's driven by batch scripts and `nmake`, not
`phpize`/`configure`/`make`, and it can't be cross-compiled from Mac or Linux. If you want to build the extension
yourself today:

1. Set up the PHP SDK Binary Tools on a Windows machine, following
   [PHP's own Windows build documentation](https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2) (the PHP 7.2+
   version of the guide - this extension targets PHP 8.4+).
2. This repository already includes `config.w32`, the Windows equivalent of `config.m4`, describing how to build this
   extension - point the SDK's build process at this repository as you would any other PHP extension source.
3. The build produces `php_oceanmoon_math.dll`.

## Enabling the extension

Once you have `php_oceanmoon_math.dll`, place it in PHP's extension directory (check `php_ini`/`extension_dir` under
`phpinfo()` or `php -i`), then add this line to `php.ini`:

```ini
extension=php_oceanmoon_math.dll
```

As with any platform, if PHP is running as a persistent process (e.g. under IIS via FastCGI, rather than invoked fresh
from the command line each time), that process needs restarting before it picks up the change.

Confirm it loaded:

```
php -m
```

Look for `oceanmoon_math` in the list.
