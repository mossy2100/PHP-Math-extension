# Installation: Remote (VPS / Cloud Servers)

A remote Linux server (a DigitalOcean Droplet, an EC2 instance, a plain VPS, etc.) is really just Linux - if the
hosting provider gives you a shell with root/sudo access and lets you install PHP extensions at all (many managed/
shared hosts explicitly don't - see the main `README.md`'s Advantages/Disadvantages notes), you have two options:

1. **[PIE](https://github.com/php/pie)**, exactly as described in the Mac/Linux installation doc, if the server has
   a C compiler and the PHP development headers installed (or you're willing to install them).
2. **Build locally, upload the compiled `.so`**, if you'd rather not install build tools on the server itself, or
   the server's PHP version/architecture matches something you've already built elsewhere.

The manual walkthrough below (option 2) is one concrete example of the process, using a DigitalOcean Droplet running
Ubuntu as the worked example. Different hosts and distros will differ in the exact paths and service-manager
commands, but the overall shape - find the extension directory, place the `.so` there, register it in an `.ini`
file, restart the service that actually serves your requests - is the same everywhere.

---

## Step 1: Find your PHP extensions directory

```bash
php -i | grep extension_dir
```

Expected output looks like: `extension_dir => /usr/lib/php/20230831 => /usr/lib/php/20230831`

## Step 2: Upload or move the `.so` file

Move your built extension file into the exact folder discovered in Step 1.

- **From your local machine** (using `scp`):
  ```bash
  scp modules/oceanmoon_math.so root@your_server_ip:/usr/lib/php/20230831/
  ```
- **If the file is already on the server** (e.g. you built it there):
  ```bash
  sudo mv oceanmoon_math.so /usr/lib/php/20230831/
  ```

Set the correct file permissions so PHP has read access:

```bash
sudo chmod 644 /usr/lib/php/20230831/oceanmoon_math.so
```

## Step 3: Find your PHP configuration folder

```bash
php -i | grep "Scan this dir for additional .ini files"
```

Expected output looks like: `Scan this dir for additional .ini files => /etc/php/8.3/cli/conf.d`

## Step 4: Create the `.ini` configuration file

On Debian/Ubuntu-family systems, create the configuration file in the central `mods-available` folder rather than
directly in the scan directory - this is what lets a single `.ini` be shared between the CLI, FPM, and Apache SAPIs
via symlinks, rather than needing a separate copy configured per SAPI:

```bash
sudo nano /etc/php/8.3/mods-available/oceanmoon_math.ini
```

(Replace `8.3` with your active PHP version.)

Paste the following line into the file, save, and exit (`Ctrl+O`, `Enter`, `Ctrl+X`):

```ini
extension=oceanmoon_math.so
```

## Step 5: Enable the extension

Link the configuration file so both the command line (CLI) and your web server's process manager (FPM or Apache)
can read it:

```bash
sudo phpenmod oceanmoon_math
```

## Step 6: Restart the service that actually serves your requests

This is the step it's easiest to forget: enabling the module doesn't take effect for a *running* `php-fpm`/Apache
process until it's restarted - only the next fresh CLI invocation picks it up automatically.

- **Nginx (LEMP)**:
  ```bash
  sudo systemctl restart php8.3-fpm
  ```
- **Apache (LAMP)**:
  ```bash
  sudo systemctl restart apache2
  ```

## Step 7: Verify the installation

```bash
php -m | grep oceanmoon_math
```

This confirms the **CLI SAPI** loaded it. It does *not* confirm `php-fpm`/Apache did - that's a separate process
with its own independently-loaded extension list. To actually confirm the SAPI serving your web requests has it,
either:

- Check via a script executed through the web server itself, e.g. a route/page that runs
  `var_dump(extension_loaded('oceanmoon_math'));`, or
- For FPM specifically: `sudo php-fpm8.3 -m | grep oceanmoon_math` (running the FPM binary's own `-m` flag, not the
  CLI binary's), or check its status page if you have one configured.
