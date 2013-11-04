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

Caveats
-------

1. If you set the screen size to be too large or too small, the game won't run
correctly. You can solve this with `rm preferences.txt`.

2. The game might have strange glitches on certain terminals (`iTerm 2` is
known to cause problems; `Terminal.app` works).

Technical
---------

We used a linked list to keep track of enemies (we'd pop them off the list 
whenever one died), and an array to hold the explosions.

We used structs to hold enemies, the player, (along with sprites, bullets, 
and almost everything else) and the game itself. This let us be able to 
more easily create lots of them, which keeps the game simple even if there's
a lot of enemies on the screen.

To do the preferences (height and width) of the game, we would write and 
read info from a file, preferences.txt. 

To do all the graphics, we basically had strings/chars that we would printf()
or putchar() onto the screen, and everything displayed on the screen at 20fps.

To adjust for screen text char dimensions, we implemented a integer by-1 velocity
scheme for horizontal movement, and floating-point velocity (that we rounded off
for coordinates) for the vertical movements, and it would be at about half of the
horizontal velocity. 
