rtrack - track `require()`/`include()`
=======================================


functions
---------

```
string rtrack_count();
```
Returns total count of required/included files.


ini settings
------------

`rtrack.hook_func` will be called when a file is required/included.

`rtrack.log_file` is the log file which is used for rtrack to record required/included files.


How to build
============

In docker
---------

Just run `./build-docker.sh`.


Otherwise
---------

Just like other php extensions: `phpize && ./configure && make`. Please refer to the `Dockerfile`.
