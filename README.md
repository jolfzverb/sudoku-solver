# sudoku-solver
Console semi-interactive toy sudoku solver. Does not use brute-force approach. Implements several simple heuristics to try to reduce possible values.

Algorithm:
1) Fill all empty squares with all possible pencil marks (from 1 to 9)
2) Reduce values in pencil marks from directly visible squares (row, column and box)
3) Fill whatever is fillable
4) Repeat 2 and 3 until it does not work
5) Try to find 2-, 3- and 4-pair restrictions in rows, columns and boxes.
6) Repeat 2..5 until it does not work
7) Give up. =)

Future plans:
1) Implement reverse pair search (squares that constitute pair can't contain other digits)
2) Implement extra restrictions (king's move, knight's move, etc)
