# Installation: Mac, Linux

## Installation via PIE (recommended)

[PIE](https://github.com/php/pie) (PHP Installer for Extensions) is the official successor to PECL for installing
native PHP extensions, and is what this package's `composer.json` `"type": "php-ext"` configuration
(`extension-name`, `support-zts`, `support-nts`, `configure-options`) is designed for - plain `composer require`
can't compile and install a native extension, since Composer only manages PHP-space autoloading, not native builds.

```bash
pie install oceanmoon/math-ext
```

Until it's published, you can point PIE directly at this repository instead:

```bash
pie repository:add vcs https://github.com/mossy2100/PHP-Math-extension.git
pie install oceanmoon/math-ext
```

PIE requires the same build tools as a manual build (below) - a C compiler and the PHP development headers - since,
on Mac/Linux, it runs the standard `phpize`/`configure`/`make`/`make install` pipeline itself rather than fetching a
prebuilt binary.

---

## Manual installation

### Build from source

See the main `README.md`'s Building section for the full `phpize`/`configure`/`make` workflow. Once built,
`modules/oceanmoon_math.so` is a standard PHP extension module.

### Enable the extension

#### Option 1 (recommended) - Use the provided composer command

Go to the local folder containing the git repository for the extension. Let's call this `extension-dir`.

Run `composer enable`.

This will set up a custom `oceanmoon_math.ini` file in the PHP scan directory (as described below), telling PHP to
load the compiled `oceanmoon_math.so` extension directly from the `extension-dir/modules` subfolder.

The advantage of this approach is that PHP is linked to the repo's own build rather than a copied file, so every
subsequent rebuild is picked up automatically without redeploying.

Run `composer disable` to disable the extension. This doesn't affect the `.so` file but will remove
`oceanmoon_math.ini` so the extension isn't loaded by PHP.

#### Option 2 - Manual setup

Decide whether you want to copy `oceanmoon_math.so` to PHP's extensions folder and load it from there, or just link
directly to the build version (i.e. the one in `extension-dir/modules`).

To copy the extension to PHP's own extensions folder, first go to `extension-dir`. Then:

```bash
cp modules/oceanmoon_math.so "$(php -r 'echo ini_get("extension_dir");')/"
```

Tell PHP to load the extension (from wherever you decided) either by adding a small custom `.ini` file
(`oceanmoon_math.ini`) to the configuration scan directory, or editing `php.ini`.

To add an `oceanmoon_math.ini` file, go to PHP's configuration scan directory:

```bash
cd $(php -r 'echo PHP_CONFIG_FILE_SCAN_DIR;')
```

Create the new file:

```bash
sudo nano oceanmoon_math.ini
```

With this content:

```ini
extension={path to oceanmoon_math.so}
```

Otherwise, to modify `php.ini` directly:

```bash
cd $(php -r 'echo PHP_CONFIG_FILE_PATH;')
sudo nano php.ini
```

Look for the "Dynamic Extensions" section and a bunch of lines that look like `extension=bz2`.

Add this line:

```ini
extension={path to oceanmoon_math.so}
```

### Restart, if PHP is running as a persistent daemon

If you're only using PHP from the command line (CLI SAPI), a new `.ini` file takes effect on the very next `php`
invocation - nothing to restart. But if PHP is also running as a long-lived process - `php-fpm`, or `mod_php` under
Apache - that process already loaded its extensions when it started, and won't notice a new `.ini` file until it's
restarted. Common local setups that fall into this category: Laravel Valet, Herd, or `brew services start php`.

```bash
# php-fpm, e.g. via Homebrew
brew services restart php
# or, if managed by your OS's service manager
sudo systemctl restart php-fpm

# Apache
sudo apachectl restart
```

### Confirm it loaded

```bash
php -m | grep oceanmoon_math
```

The response should just be `oceanmoon_math`.

**Note:** this only confirms the CLI SAPI has it loaded. If your actual use case is a web request handled by
`php-fpm`/Apache (not the CLI), that's a separate PHP process with its own separately-loaded extension list - `php -m`
here says nothing about whether *that* process has it. Confirm via a script executed through the web server itself
(e.g. `<?php var_dump(extension_loaded('oceanmoon_math'));`), not just the command line.
