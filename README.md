![asdf](meta/banner.png)
# DuckHax CLI

The CLI-tool for: dumping, building and inspecting game-files for Sitting Ducks
## Building
### Requirements
You need will **libpng**, that's about it!

Windows builds haven't been tested yet
```sh
$ mkdir build
$ cd build
$ cmake ..
```

## Usage
**YOU WILL NEED THE ORIGINAL GAME FILES**

### Dumping:
```sh
$ ./DuckHax dump -i pathToGameDir/ -o dump/
```