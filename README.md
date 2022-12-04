# Chase

Like `find` but using threads.

## Build from source

### Dependencies
You have to met the following dependencies: `gcc`, `make`, `git`.

- Clone repository
```bash
git clone https://github.com/BetulioMay/chase.git
```
- Build project:
```bash
cd chase
make
```

## Usage

- Search by filename:
```bash
./bin/chase <dir> -n <filename>
```
- Search by filetype:
```bash
./bin/chase <dir> -t [f | d]
```
`f` for regular files and `d` for directory file.

