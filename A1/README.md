# avondemh - CS 488 F 2024 - A1

## Compilation

program is compiled normally with `make` and `./A1`

## Manual

### Assumptions
- Block height increment is done in terms of cubes (as opposed to numerical growth)
- The player cannot move off the playable coloured floor
- The start is always on the left of the maze

### Code referenced / used

https://www.songho.ca/opengl/gl_sphere.html
- used in the function `initAvatar`
- modified to get rid of vector use / to fit naming convention I had of longitude / latitude / hours

### Additions

- horizontal scrolling will cause rotation