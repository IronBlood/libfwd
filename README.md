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

Other
=====
