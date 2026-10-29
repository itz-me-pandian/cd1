# Simple-assignment question

## Build
```sh
make
```

## Run the integrated lexer + parser + TAC generator
```sh
./compiler < sample.in
cat tac.txt
```

## Optimize and generate target code
```sh
./optimizer tac.txt optimized.txt
./backend optimized.txt target.txt
cat optimized.txt
cat target.txt
```

## Important
- Edit `sample.in` or redirect your own source program into `./compiler`.
- Every assignment must end with `;`.
- TAC is written to `tac.txt`; the parser status message is printed into that file as well in this basic lab version.
- Optimizer/backend are separate modules to demonstrate questions (d) and (e).
