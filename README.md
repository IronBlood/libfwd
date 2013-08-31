This is a forked project from [Vladimir V. Pavluk](http://www.pavluk.org/)'s project [libcgic](http://www.pavluk.org/projects/libcgic/). The original project supports both CGI and FastCGI, made up of three parts:

* [libcgic](http://www.pavluk.org/projects/libcgic/libcgic/) - The core part handles HTTP request and response.
* [libdata](http://www.pavluk.org/projects/libcgic/libdata/) - A data processing library only working with MySQL database.
* [libhtmltpl](http://www.pavluk.org/projects/libcgic/libhtmltpl) - A template engine library.

Well, as my demand is quite simple: a framework ONLY supporting **FastCGI** would works for me, and would be better if works with **JSON**. So I start to migrate the core part to this new project, [libfwd](https://github.com/IronBlood/libfwd), which means library for FCGI Web Daemon.

Dependence
==========

libfwd requires the following libraries:
* [libghthash](https://github.com/SimonKagstrom/libghthash)
* [json-c](https://github.com/json-c/json-c)
* [fcgi](http://www.fastcgi.com/drupal/)

Installation
============

libfwd should be easy to install as other GNU programs.

    $ ./configure
    $ make
    # make install

Usage
=====
### Project Structure ###
The **helloword** example is quite simple. The structure of a typical project is list here:

```
examples/helloworld/
├── Makefile
├── helloworld.nginx.conf
├── main.c
└── run.sh
```

The `Makefile` and `main.c` is the main part. You may copy the `main.c` to your project and add new function prototype at the beginning, just like this:

```
int helloworld_main(FCGX_Request *request);
int error_501_main(FCGX_Request *request);
```

Then, name your function with a **script name**, and add it to `cgi_applet_t applets[]`, shown below:

```
cgi_applet_t applets[] = {
	{ helloworld_main, "/helloworld.json" },
	{ NULL, NULL },
};
```

When your application is up and running, you may visit the path `http://your.ip.address/your.app.path/helloworld.json` and it will call the `helloworld_main()` for you.

When you finish coding, use `make(1)` utility to compile. You may check out `Makefile` to view more.

### Up and Running ###
The `run.sh` script will call `cgi-fcgi(1)` utility for you. It **SHOULD** work with most web servers which support fastcgi, such as Nginx, Apache, Lighttpd, etc.

The `helloworld.nginx.conf` file is an example for Nginx. You may paste it to your configuration file, and reload.

Other
=====
