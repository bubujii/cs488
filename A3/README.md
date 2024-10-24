# avondemh - CS 488 F 2024 - A3

## Compilation

program is compiled normally with `make` and `./A3 Assets/puppet.lua`

## Manual

### Assumptions
Rotation (with the trackball) is done in relation to the origin of the scene. The "scene" being the model normally. 

### Code referenced / used

Used code from the picking example to implement picking

### Additions

- Head rotates only one way, but there is a tail :)))
- undo / redo counter

### Clarification of hierarchy

To avoid the issue of a translated node messing with rotation the way the model is connected is that there are "translation joints", "rotation joints" and "connector joints". When manipulating a body part, you manipulate a "rotation joint" which is connected to a "connector joint" which itself is connected to the actual geometry that was clicked.