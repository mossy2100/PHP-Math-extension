# Installation

Install the extension with [PIE](https://github.com/php/pie):

```bash
pie install mossy2100/math-ext
```

On Mac and Linux, PIE builds it from source for the PHP it runs under. On Windows, it downloads a prebuilt DLL.

For details, manual builds and server setups, pick the page matching your situation:

- **[Mac, Linux](Installation/Mac,%20Linux.md)** - via [PIE](https://github.com/php/pie), or manually.
- **[Windows](Installation/Windows.md)** - via [PIE](https://github.com/php/pie), which downloads a prebuilt DLL, or
  manually.
- **[Docker](Installation/Docker.md)** - a `Dockerfile` build stage.
- **[Remote](Installation/Remote.md)** - VPS/cloud servers (DigitalOcean, EC2, etc.), including the production-server specifics (`phpenmod`,
  restarting `php-fpm`/Apache) that don't apply to a local dev install.
