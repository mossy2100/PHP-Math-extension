# Installation: Docker

Add a build stage that compiles the extension from source, using the helper scripts the official `php` images provide:

```dockerfile
FROM php:8.4-cli

# Copy the extension source in (or `git clone` it) and build it.
COPY . /tmp/oceanmoon-math-ext
RUN cd /tmp/oceanmoon-math-ext \
    && phpize \
    && ./configure --enable-oceanmoon_math \
    && make -j"$(nproc)" \
    && make install \
    && docker-php-ext-enable oceanmoon_math \
    && rm -rf /tmp/oceanmoon-math-ext
```

`make install` (a standard target phpize's generated `Makefile` always provides) copies the built module into the
image's extension directory; `docker-php-ext-enable` (provided by the official `php` images) then writes the `php.ini`
entry for you.

No restart step is needed here, unlike a long-lived server install (see the Mac/Linux and Remote installation docs) -
the extension is baked into the image itself, so every container started from it already has it enabled from the moment
PHP starts.

## Confirm it built correctly

```bash
docker build -t your-image .
docker run --rm your-image php -m | grep oceanmoon_math
```
