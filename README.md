terminvaders
============

Ben Kurtovic and Victor Jiao

Story
-----

It seems like aliens are once again trying to invade Earth in their fancy
spaceships! Oh no! Who will save us from the evil about to take over?

You (the green spaceship on the bottom) will be attempting to fend off the
alien spaceships (those on the top) for as long as possible. Press Spacebar to
shoot, and the WASD keys to use your engines!

Remember, due to being in space, you must toggle an engine to start moving, and
toggle the reverse one to stop!

Unfortunately, your spaceship isn't the best one the US Military has (better
ones are currently on a mission to repopulate Mars) so whenever you toggle any
engine, your shooting mechanism locks shut, and must be re-initiated (press and
hold the spacebar again) to be resumed.

Good luck!

FAQ
---

- Q: How do I change the screen size? Can I change the screen size?
  - A: Sure you can! On the main menu, go down to Settings and press Enter.
Then, using the W/S keys or Up/Down arrow keys, highlight the parameter you
want to change, press Enter, type in the parameter size and press Enter again.
To go back to the main menu, highlight Back, and press Enter. Your settings are
saved automatically in `preferences.txt`, so there's no need to constantly
resize the screen to yo!

- Q: How do I exit the game?
  - A: :( . You can either highlight 'Exit' on the main menu and press enter,
or keep pressing Q. That will eventually get you to the main menu. You can then
press Q to exit.

- Q: dis gaem sux0rs
  - A: That's not really a question, but don't worry, for updates shall be
forthcoming!

Technical
---------

We use a linked list to keep track of enemies, bullets, and explosions. When
one spawns, we make it the first element in the list and have its `next`
pointer point to the previous first element. When one despawns, we deallocate
it and adjust the previous entity's `next` to point to the dying entity's
`next`. This ensures the list is efficient for adding, iterating, and removal.

We use structs to store data on single enemies, the player, bullets,
explosions, sprites, points, and the game itself. A point is stored within all
of the "entity" structs, and the game struct contains pointers to various
entity structs.

To store game preferences (window height and width), we write and read from a
file, `preferences.txt`. The file format is `<int>:<int>`, with the first int
being the number of rows and the second the number of columns.

To handle graphics, we have an array of sprite structs, with each one storing
an array of strings, a width, a height, and color data. The index of a sprite
in the array corresponds to the type, so `sprite_table[ENEMY]` is the enemy
ship sprite. The `draw` function takes a sprite struct and a point, rendering
the sprite centered around that point using the width/height data.

To adjust for the width of a printed character being less than its height, we
scale the player ship's vertical velocity to be half its horizontal velocity.
Coordinates are stored as doubles which are truncated when drawing.

Caveats
-------

1. If you set the screen size to be too large or too small, the game won't run
correctly. You can solve this with `rm preferences.txt`.

2. The game might have strange glitches on certain terminals (`iTerm 2` is
known to cause problems; `Terminal.app` works).

3. `game.c` contains most of the important code, and it can be a bit confusing
to read as a result.
