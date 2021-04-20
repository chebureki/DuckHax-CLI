![asdf](meta/banner.png)
# DuckHax CLI
The CLI-tool for: dumping, building and inspecting game-files for _Sitting Ducks_ by Asobo Studios

## Running Sitting Ducks on Linux
The PC release is a mess, even on Windows music and intro-videos are non-functional.
Wine signals a page fault on Linux, which renders the game unusable!
To get the game to **even run** simply hide them from the game.
Reencoding doesn't help, sadly :(
```sh
cd pathToGame
mkdir videos/backup sounds/MUSIC/backup/
mv videos/* videos/backup
mv sounds/MUSIC/* sound/music/backup
```

## Building
### Requirements
You will need **libpng**, that's about it!

Windows builds haven't been tested yet
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage
**YOU WILL NEED THE ORIGINAL GAME FILES**

### Dumping:
```sh
$ ./DuckHax dump -i pathToGameDir/ -o dump/
```

### Inspecting:
```sh
$ ./DuckHax inspect -i pathToFile
```