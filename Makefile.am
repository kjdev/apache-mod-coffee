
ACLOCAL_AMFLAGS = -I m4

moddir = @APACHE_MODULEDIR@
mod_LTLIBRARIES = mod_coffee.la

mod_coffee_la_SOURCES = mod_coffee.cpp
mod_coffee_la_CFLAGS = @APACHE_CFLAGS@ @APACHE_INCLUDES@ @V8_INCLUDES@
mod_coffee_la_CPPFLAGS = @APACHE_CPPFLAGS@ @APACHE_INCLUDES@ @V8_INCLUDES@
mod_coffee_la_LDFLAGS = -avoid-version -module @APACHE_LDFLAGS@ @V8_LDFLAGS@
mod_coffee_la_LIBS = @APACHE_LIBS@ @V8_LIBS@
