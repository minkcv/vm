# descent design document
*** SPOILERS ***

### main descent
functional way to determine specifics of each 256 height slice and 64 height sub-slices
render 4-5 sub-slices at a time.  use transparent sprite data start to scroll

#### subslice
contains data about 2 platforms and 2 passages
platforms:
* none: no opening, no platform
* closed: no opening, yes platform
* blocked: open behind block, yes platform
* open: yes opening, yes platform
layout:
```
|                   |
|                   ]
|             ------+
|                   |
[                   |
+------             |
```

store player slice, sub-slice, feet y
player moves left and right, deploys glider when moving off edge of platform
calculate world around player SPRITE feet y = 96 in main descent
4 platform sprites per side, move 2 or 3 at once and keep one at bottom for y > sub-slice 60
3 or 4 wall sprites on sides. 2 static at top and bottom, 1 or 2 moving
  opening to side passages, empty lower (y+) 32px
some platforms have blocks on them
can change direction in air
slide down walls and relaunch wings 

sprites with alpha before and after

```
AAAAAAAA
--------
AAAAAAAA
```
```
A
A
A
|
|
|
A
A
A
```

### world map
note that the map of sub-slices is grid aligned
where as the vertical "descent" sections are staggered when drawn in detail
such that exiting via the right passage results in the player
being at the lower left platform of the vertical section to the right.
continually moving right results in the player descending 1 subslice.
continually moving left results in the player remaining at the same subslice.

passage between vertical sections
```
|        ]====[      ---
[      ---    ---      |
---      |    |        ]
```
map of sub-slices
```
          0 1 2 3
0         |
1        s+
2  c+b+c+b+b+  
3   | | +b+b+c+  
4   | | +b+b+ +b+
5   | | +b+b+ | +c
6   | | | | | | |
7   | | | | | | |
8  c|x|c|x|x| |x|   <-- start repeating pattern
9   | | | | |c| |c
10  +e| | | | |e+   <-- entered from far side only
11  | | | | | | |
12 c|x|c|x|x| |x|
13  | | | | |c| |c
14  +e| | | | |e+
15  | | | | | | |
16 c|x|c|x|x| |x|
17  | | | | |c| |c
18  +e| | | | |e+
19  | | | | | | |
20 c|x|c|x|x| |x|
21  | | | | |c| |c
22  +e| | | | |e+
23  | | | | | | |
24 c|x|c|x|x| |x|
25  | | | | |c| |c
26  +e| | | | |e+
27  | | | | | | |
28 c|x|c|x|x| |x|
29  | | | | |c| |c
30  +e| | | | |e+
31  | | | | | | |
```
repeat the pattern if check is not blocked and not passable

key:

|: no platforms or passages

s: player start

b: block type depending on checks passed and h-dist

c: block check

x: blocked platform (hint but player can't decline block)

e: empty room (secret in challenge mode)

slices 0-127 normal

after slice 127 graphics start to change, writing random tiles over platforms, walls, hint blocks

### side passages
 
#### single block
walk past the block and it falls and blocks you in

#### block check passage
3 columns

3 depth first, 5 depth next

find out what blocks you've been passing have fallen into the pit

the pattern is somewhat random for challenge mode
```
=======|   |======
        036
        147
        258
```

### challenge mode

UP UP DN DN LEFT RIGHT at title

"biohazard detected" after title

oxygen counter that ticks down

can find crew members in secret empty rooms. one before slice 127, one after slice 127

find both crew to sedate evil red crew member and carry red

### bad ending
player has seen the blocked check passage (piled too high)

return to descent and no more platforms/passages

evil red crew member flys in and kills the player

### good ending
player ascends via elevator, show ending report with survived status

### best ending
player ascends via elevator with 4/4 crew, show ship leaving planet, show ending report with escaped status

### ending report screens
status: deceased/survived/escaped

mode: standard/challenge (only shown on escape: complete)

crew: 1-4 (only shown on mode: challenge)

