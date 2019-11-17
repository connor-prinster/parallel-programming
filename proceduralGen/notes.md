# Notes on Procedural Generation

## Links:

[How Minecraft Generates Such HUGE Worlds | Game/Show | PBS Digital Studios](https://www.youtube.com/watch?v=8Ex_eHJ29iQ)

[Minecraft Procedural Generation Links](http://pcg.wikidot.com/pcg-games:minecraft)

[Terrain Generation: Part I](https://notch.tumblr.com/post/3746989361/terrain-generation-part-1)

[Minecraft Map Generator Summary?](https://github.com/UnknownShadow200/ClassiCube/wiki/Minecraft-Classic-map-generation-algorithm)

## Perlin Noise
Makes sure that there isn't total randomness (like cats in water or whatever).

There is randomness, but it's ordered

### Noise Map
flat surface covered in different shades of dots that are grouped together in semi-random pattern. 

Sort of topographical

## Map Generation:
Computer creates a noise map

computer smooths it into a more realistic map
* starts with the biggest chunks
* goes to finer later on

### Steps:
1. Decides what is ground and what is sky

2. another noise map is overlapped and it generates a distinction between valleys, hills, and craggy areas

3. generates lakes, trees, clouds

## Emergent Behavior
* sand dunes vs winds
* drivers avoid traffic

### Video games
* thousands of tiny rules
* where and where not to put stuff
* rules can interact in weird ways
    * when Notch added wolves, he discovered they chased sheep
* Also known as *Engines of Perpetual Novelty*

## Small Notes

* Like any technology, the software multiplies human effort ~ *Daniel Cook*
