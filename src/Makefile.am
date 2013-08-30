lib_LTLIBRARIES=libfwd.la
fwd_HEADERS=fwd/cgi.h fwd/cgi_headers.h fwd/cgi_params.h fwd/cgi_sess_file.h fwd/cgi_session.h fwd/cgi_utils.h fwd/cgi_fcgx_wrapper.h fwd/cgi_ob_stream.h fwd/cgi_parser.h fwd/cgi_sess_stdtype.h fwd/cgi_template.h
libfwd_la_LDFLAGS=
libfwd_la_SOURCES=cgi.c cgi_ob_stream.c cgi_sess_file.c cgi_template.c cgi_fcgx_wrapper.c cgi_params.c cgi_session.c cgi_utils.c cgi_headers.c cgi_parser.c cgi_sess_stdtype.c

libfwd_la_CFLAGS=-O2 -Wall -std=c99 -D_GNU_SOURCE
libfwd_la_LIBADD=-lfcgi -lghthash -ljson

fwddir=$(prefix)/include/fwd
