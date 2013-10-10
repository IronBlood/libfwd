#!/bin/sh

cgi-fcgi -start -connect 127.0.0.1:20000 multiple-threaded.fcgi
