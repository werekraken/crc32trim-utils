# crc32trim-utils

`crc32combine` wraps _[zlib's](https://github.com/madler/zlib)_ `crc32combine()` providing functionality to combine two or more crcs of streams when the lengths of all streams, but the first, are known.

`crc32trim-trailing` wraps _[libcrc32trim's](https://github.com/werekraken/libcrc32trim)_ `crc32_trim_trailing()` providing functionality to trim one or more crcs of streams from the end of a combined crc when the lengths of all streams, but the first, are known.

`crc32trim-leading` wraps _[libcrc32trim's](https://github.com/werekraken/libcrc32trim)_ `crc32_trim_leading()` providing functionality to trim one or more crcs of streams from the front of a combined crc when the lengths of all streams, but the first, are known.

## Usage

### Basic usage

```
$ crc32 <(echo -n "this")
feee8227
```
```
$ crc32 <(echo -n "other")
d9583520
```
```
$ crc32 <(echo -n "thisother")
c4aa891d
```
```
$ ./crc32combine feee8227 d9583520 5
c4aa891d
```
```
$ ./crc32trim-trailing c4aa891d d9583520 5
feee8227
```
```
$ ./crc32trim-leading c4aa891d feee8227 5
d9583520
```

### Recursive usage

```
$ for i in this that other another; do echo "`crc32 <(echo -n "$i")` $i"; done
feee8227 this
a8539d8c that
d9583520 other
9d293438 another
```
```
$ crc32 <(echo -n "thisthatotheranother")
0787c864
```
```
$ ./crc32combine feee8227 a8539d8c 4 d9583520 5 9d293438 7
0787c864
```
```
$ ./crc32trim-trailing 0787c864 9d293438 7 d9583520 5 a8539d8c 4
feee8227
```
```
$ ./crc32trim-leading 0787c864 feee8227 16 a8539d8c 12 d9583520 7
9d293438
```
```
$ ./crc32trim-leading --split-lengths 0787c864 feee8227 4 a8539d8c 5 d9583520 7
9d293438
```

### Cooperative usage

```
$ for i in this that other another; do echo "`crc32 <(echo -n "$i")` $i"; done
feee8227 this
a8539d8c that
d9583520 other
9d293438 another
```
```
$ crc32 <(echo -n "thisthatotheranother")
0787c864
```
```
$ ./crc32trim-leading `./crc32trim-trailing 0787c864 9d293438 7 d9583520 5` feee8227 4
a8539d8c
```
```
$ ./crc32trim-leading `./crc32trim-trailing 0787c864 9d293438 7` feee8227 9 a8539d8c 5
d9583520
```
```
$ ./crc32trim-leading --split-lengths `./crc32trim-trailing 0787c864 9d293438 7` feee8227 4 a8539d8c 5
d9583520
```

## Versioning

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## License

_crc32trim-utils_ is available as open source under the terms of the [zlib license](LICENSE).
