Installing from Git master requires 2 preparatory steps before the typical ones.

First:

```sh
autoreconf -i
automake --foreign -Wall
```

Then:

```sh
./configure
make
make install
```
