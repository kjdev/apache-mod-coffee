# mod_coffee #

mod_coffee is CoffeeScript handler module for Apache HTTPD Server.

## Dependencies ##

* [coffee-script](http://coffeescript.org/)
* [V8](http://code.google.com/p/v8)

## Build ##

    % ./autogen.sh (or autoreconf -i)
    % ./configure [OPTION]
    % make
    % make install

### Build Options ###

V8 path.

* --with-v8=PATH  [default=/usr/include]
* --with-v8-lib=PATH  [default=no]

V8 isolate.

* --enable-v8-isolate  [default=no]

apache path.

* --with-apxs=PATH  [default=yes]
* --with-apr=PATH  [default=yes]
* --with-apreq2=PATH  [default=yes]

## Configration ##

httpd.conf:

    LoadModule coffee_module modules/mod_coffee.so
    AddHandler coffee-script .coffee

## CoffeeScript Compile Options ##

bare: true

  Compile the JavaScript without the top-level function safety wrapper.

## Example ##

test.coffee:

    # Assignment:
    number   = 42
    opposite = true

    # Conditions:
    number = -42 if opposite

    # Functions:
    square = (x) -> x * x

    # Arrays:
    list = [1, 2, 3, 4, 5]

    # Objects:
    math =
      root:   Math.sqrt
      square: square
      cube:   (x) -> x * square x

    # Splats:
    race = (winner, runners...) ->
      print winner, runners

    # Existence:
    alert "I knew it!" if elvis?

    # Array comprehensions:
    cubes = (math.cube num for num in list)
