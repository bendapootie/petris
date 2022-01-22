## "O" Piece
Nothing to be done here. The O piece just rotates in place.

## "I" Piece
```Tetrimino
North   East    South   West
....    .0-.    ....    .-3.
----    .1-.    3210    .-2.
0123    .2-.    ----    .-1.
....    .3-.    ....    .-0.
```
### Clockwise Rotations
| Direction     | 0      | 1 | 2 | 3 | 4 |
| ------------- | ------ | - | - | - | - |
| North -> East | (0, 0) |  | - | - | - |
| East -> South | - | - | - | - | - |
| South -> West | - | - | - | - | - |
| West -> North | - | - | - | - | - |
### Counter-Clockwise Rotations
#### North -> West
#### West -> South
#### South -> East
#### East -> North

## "T" Piece
```Tetrimino
North   East    South   West
....    ....    ....    ....
-5--    .0-.    ....    -2..
012-    .15.    210.    51..
....    .2-.    -5-.    -0..
```
### Clockwise Rotations
| Direction     | 1      | 2       | 3        | 4       | 5        |
| ------------- | ------ | ------- | -------- | ------- | -------- |
| North -> East | (0, 0) | (-1, 0) | (-1,  1) | n/a     | (-1, -2) |
| East -> South | (0, 0) | ( 1, 0) | ( 1,  1) | ( 0, 2) | ( 1,  2) |
| South -> West | (0, 0) | ( 1, 0) | n/a      | ( 0, 2) | ( 1,  2) |
| West -> North | (0, 0) | (-1, 0) | (-1, -1) | ( 0, 2) | (-1,  2) |
### Counter-Clockwise Rotations
| Direction     | 1      | 2       | 3        | 4       | 5        |
| ------------- | ------ | ------- | -------- | ------- | -------- |
| North -> West | (0, 0) | ( 1, 0) | ( 1,  1) | n/a     | ( 1, -2) |
| East -> North | (0, 0) | ( 1, 0) | ( 1, -1) | ( 0, 2) | ( 1,  2) |
| South -> East | (0, 0) | (-1, 0) | n/a      | ( 0, 2) | (-1,  2) |
| West -> South | (0, 0) | (-1, 0) | (-1, -1) | ( 0, 2) | (-1,  2) |


## "L" Piece
## "J" Piece
## "S" Piece
## "Z" Piece