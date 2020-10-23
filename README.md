# Fano JWT

This example project demonstrates how to use [JSON Web Token (JWT) signing and verification](https://fanoframework.github.io/security/jwt/) with Fano Framework.

This project is generated using [Fano CLI](https://github.com/fanoframework/fano-cli)
command line tools to help scaffolding web application using Fano Framework.

## Requirement

- Linux or FreeBSD
- [Free Pascal](https://www.freepascal.org/) >= 3.0
- Web Server ([Apache with mod_proxy_scgi](https://httpd.apache.org/docs/current/mod/mod_proxy_scgi.html), nginx)
- [Fano CLI](https://github.com/fanoframework/fano-cli)
- Web Server (Apache, nginx)
- MySQL 5.7
- Administrative privilege for setting up virtual host

## Installation

### TLDR
Make sure all requirements are met. Run
```
$ git clone https://github.com/fanoframework/fano-jwt.git --recursive
$ cd fano-jwt
$ ./tools/config.setup.sh
$ ./build.sh
$ sudo fanocli --deploy-scgi=jwt.fano
```
### Setup database

Run data seeder utility to setup users table and seed its data. It will
create a database and a user, for example username `fano_jwt` with password `f4n0_Jwt`
and database name `fano_jwt` and setup table. Type MySQL `root` password when you are asked.

```
$ DB_ADMIN=root DB_USER=fano_jwt DB_PASSW=f4n0_Jwt ./tools/data.seeder.sh
```
Put database username, password and database name in `config/config.json`.
```
    "db" : {
        "mysql" : {
            "version" : "mysql 5.7",
            "host" : "localhost",
            "port" : 3306,
            "username" : "fano_jwt",
            "password" : "f4n0_Jwt",
            "database" : "fano_jwt"
        }
    },

```
### Create secret key

Edit `secretKey` key in `config/config.json` with your secret key. To generate random
key, you can use Fano CLI.

```
$ fanocli --key
```

### Setup Authorization header in Apache
By default, Apache will not pass `Authorization` header to our reverse-proxied application.
Edit `/etc/apache2/sites-available/jwt.fano.conf` (if in Debian-based Linux) and add
following line inside `VirtualHost` tag.
```
<IfModule mod_rewrite.c>
    RewriteEngine on
    RewriteRule .* - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization},L]
</IfModule>
```

Reload Apache so that configuration is read.

```
$ sudo systemctl reload apache2
```
### Run application

```
$ ./bin/app.cgi
```

Open HTTP client, for example Postman or curl, and go to `http://jwt.fano`. Without `Authorization` header you should see application rejects with HTTP error 401.

Authenticate using curl to get token.

```
$ curl -XPOST -d "user=joko@widowo.com&passw=joko@widowo.com" http://jwt.fano/auth
```
It will returns
```
{"status":"OK","token" : "very_long_jwt_token"}
```
After that, use token to get access

```
$ curl -H "Authorization: Bearer {replace_with_token}" http://jwt.fano
```
If token is verified, you should see "Hello joko@widowo.com"

### Free Pascal installation

Make sure [Free Pascal](https://www.freepascal.org/) is installed. Run

    $ fpc -i

If you see something like `Free Pascal Compiler version 3.0.4`,  you are good to go.

Clone this repository

    $ git clone https://your-repo-hostname/fano-app.git --recursive

`--recursive` is needed so git also pull [Fano](https://github.com/fanoframework/fano) repository.

If you are missing `--recursive` when you clone, you may find that `vendor/fano` directory is empty. In this case run

    $ git submodule update --init

To update Fano to its latest commit, run

    $ git checkout master && git submodule foreach --recursive git pull origin master

Above command will checkout to `master` branch of this repository and pull latest update from `master` branch of [Fano](https://github.com/fanoframework/fano) repository.

Copy `*.cfg.sample` to `*.cfg`.
Make adjustment as you need in `build.cfg`, `build.prod.cfg`, `build.dev.cfg`
and run `build.sh` shell script (if you are on Windows, then `build.cmd`).

These `*.cfg` files contain some Free Pascal compiler switches that you can turn on/off to change how executable is compiled and generated. For complete
explanation on available compiler switches, consult Free Pascal documentation.

Also copy `src/config/config.json.sample` to `src/config/config.json` and edit
configuration as needed. For example, you may need to change `baseUrl` to match your own base url so JavaScript or CSS stylesheets point to correct URL.

    $ cp config/config.json.sample config/config.json
    $ cp build.prod.cfg.sample build.prod.cfg
    $ cp build.dev.cfg.sample build.dev.cfg
    $ cp build.cfg.sample build.cfg
    $ ./build.sh

`tools/config.setup.sh` shell script is provided to simplify copying those
configuration files. Following shell command is similar to command above.

    $ ./tools/config.setup.sh
    $ ./build.sh

By default, it will output binary executable in `public` directory.

### Build for different environment

To build for different environment, set `BUILD_TYPE` environment variable.

#### Build for production environment

    $ BUILD_TYPE=prod ./build.sh

Build process will use compiler configuration defined in `vendor/fano/fano.cfg`, `build.cfg` and `build.prod.cfg`. By default, `build.prod.cfg` contains some compiler switches that will aggressively optimize executable both in speed and size.

#### Build for development environment

    $ BUILD_TYPE=dev ./build.sh

Build process will use compiler configuration defined in `vendor/fano/fano.cfg`, `build.cfg` and `build.dev.cfg`.

If `BUILD_TYPE` environment variable is not set, production environment will be assumed.

## Change executable output directory

Compilation will output executable to directory defined in `EXEC_OUTPUT_DIR`
environment variable. By default is `public` directory.

    $ EXEC_OUTPUT_DIR=/path/to/public/dir ./build.sh

## Change executable name

Compilation will use executable filename as defined in `EXEC_OUTPUT_NAME`
environment variable. By default is `app.cgi` filename.

    $ EXEC_OUTPUT_NAME=index.cgi ./build.sh

## Run

### Run with a webserver

Setup a virtual host. Please consult documentation of web server you use.

#### Apache

You need to have `mod_proxy_scgi` installed and loaded. This module is Apache's built-in module, so it is very likely that you will have it with your Apache installation. You just need to make sure it is loaded. For example, on Debian,

```
$ sudo a2enmod proxy_scgi
$ sudo systemctl restart apache2
```

Create virtual host config and add `ProxyPassMatch`, for example

```
<VirtualHost *:80>
     ServerName www.example.com
     DocumentRoot /home/example/public

     <Directory "/home/example/public">
         Options +ExecCGI
         AllowOverride FileInfo
         Require all granted
     </Directory>

    ProxyRequests Off
    ProxyPass /css !
    ProxyPass /images !
    ProxyPass /js !
    ProxyPassMatch ^/(.*)$ scgi://127.0.0.1:20477
</VirtualHost>
```
Last four line of virtual host configurations basically tell Apache to serve any
files inside `css`, `images`, `js` directly otherwise pass it to our application.

On Debian, save it to `/etc/apache2/sites-available` for example as `fano-scgi.conf`
Enable this site and restart Apache

```
$ sudo a2ensite fano-scgi.conf
$ sudo systemctl restart apache2
```
## Deployment

You need to deploy only executable binary and any supporting files such as HTML templates, images, css stylesheets, application config.
Any `pas` or `inc` files or shell scripts is not needed in deployment machine in order application to run.

So for this repository, you will need to copy `public`, `Templates`, `config`
and `storages` directories to your deployment machine. make sure that
`storages` directory is writable by web server.

## Known Issues

### Issue with GNU Linker

When running `build.sh` script, you may encounter following warning:

```
/usr/bin/ld: warning: public/link.res contains output sections; did you forget -T?
```

This is known issue between Free Pascal and GNU Linker. See
[FAQ: link.res syntax error, or "did you forget -T?"](https://www.freepascal.org/faq.var#unix-ld219)

However, this warning is minor and can be ignored. It does not affect output executable.

### Issue with unsynchronized compiled unit with unit source

Sometime Free Pascal can not compile your code because, for example, you deleted a
unit source code (.pas) but old generated unit (.ppu, .o, .a files) still there
or when you switch between git branches. Solution is to remove those files.

By default, generated compiled units are in `bin/unit` directory.
But do not delete `README.md` file inside this directory, as it is not being ignored by git.

```
$ rm bin/unit/*.ppu
$ rm bin/unit/*.o
$ rm bin/unit/*.rsj
$ rm bin/unit/*.a
```

Following shell command will remove all files inside `bin/unit` directory except
`README.md` file.

    $ find bin/unit ! -name 'README.md' -type f -exec rm -f {} +

`tools/clean.sh` script is provided to simplify this task.

### Windows user

Free Pascal supports Windows as target operating system, however, this repository is not yet tested on Windows. To target Windows, in `build.cfg` replace
compiler switch `-Tlinux` with `-Twin64` and uncomment line `#-WC` to
become `-WC`.

### Lazarus user

While you can use Lazarus IDE, it is not mandatory tool. Any text editor for code editing (Atom, Visual Studio Code, Sublime, Vim etc) should suffice.
