-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
purple = gr.material({0.6, 0.0, 0.8}, {0.9, 0.9, 0.9}, 10)
mat2 = gr.material({0.02, 0.85, 1.0}, {1.0, 1.0, 1.0}, 25)
mat3 = gr.material({0.4, 0.4, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.4, 0.2, 0.6}, {0.5, 0.7, 0.8}, 0)
glass = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.7}, 25, {0.0, 0.0, 0.0}, 1.2, 0.996, 1.0, {0.03, 0.01, 0.01}, 0.0, 0.0, '', '')
emissive = gr.material({0.0, 0.0, 0.0}, {0.3, 0.3, 0.7}, 25, {0.0, 0.0, 0.0}, 1.0, 1.0, 1.0, {-0.00, -0.2, -0.2}, 0.0, 0.0, '', '')
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 0, {0.15, 0.15, 0.3}, 0.0, 0.0, 0.0, {0.05, 0.05, 0.05}, 0.0, 0.0, 'texture.png', '')
blue_thing = gr.material({0.1, 0.4, 0.6}, {0.2, 0.5, 0.7}, 25)
ball_top_col = gr.material({0.2, 0.6, 0.6}, {0.5, 0.7, 0.8}, 0)

orange_ish = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

water = gr.material({1.0, 0.4, 0.4}, {1.0, 0.0, 0.0}, 25,{0.0, 0.0, 0.0}, 1.3, 0.9, 1.0, {0.05, 0.05, 0.05}, 0.0, 0.0, '', 'ripples.png')
mirror = gr.material({1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, 25,{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, {0.0, 0.0, 0.0}, 0.0, 0.0, '', 'ripples.png')
rust = gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 25,{0.0, 0.0, 0.0}, 0.0, 0.0, 0.5, {0.0, 0.0, 0.0}, 0.0, 0.0, '', '')

column_mat = gr.material({0.85, 0.85, 0.9}, {1.0, 1.0, 1.0}, 25, {0.0, 0.0, 0.0}, 0.0, 0.0, 0.0)
column_mat_2 = gr.material({0.00, 1.0, 1.0}, {1.0, 1.0, 1.0}, 25, {0.0,0.0, 0.0}, 1.0, 0.8, 0.0)
gloss = gr.material({0.35, 0.85, 0.9}, {0.0, 0.0, 0.0}, 0, {0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, {0.0, 0.0, 0.0}, 0.0, 3.0, '', '')

scene_root = gr.node('root')

floor = gr.node('floor')
scene_root:add_child(floor)

plane1 = gr.mesh('plane', 'plane.obj' )
floor:add_child(plane1)
plane1:set_material(grey)
plane1:scale(100, 100, 100)
plane1:set_texture_tiling(2.5, 2.5)
plane1:translate(-150, 0, 0)

plane2 = gr.mesh('plane', 'plane.obj' )
floor:add_child(plane2)
plane2:set_material(grey)
plane2:scale(100, 100, 100)
plane2:set_texture_tiling(2.5, 2.5)
plane2:translate(150, 0, 0)

plane3 = gr.mesh('plane', 'plane.obj' )
floor:add_child(plane3)
plane3:set_material(grey)
plane3:scale(20, 100, 100)
plane3:set_texture_tiling(0.5, 2.5)

pool1 = gr.cube('pool1')
pool1:scale(40, 20, 200)
pool1:translate(-40, -10.001, 0)
pool1:set_material(mirror)
pool1:set_texture_tiling(2, 10)

scene_root:add_child(pool1)

pool2 = gr.cube('pool2')
pool2:scale(40, 40, 200)
pool2:translate(40, -20.001, 0)
pool2:set_material(water)
pool2:set_texture_tiling(2, 10)

scene_root:add_child(pool2)

mountain = gr.mountain('mountain', 25, 0.9)
mountain:set_material(purple)
mountain:scale(20, 5, 3)
mountain:translate(30, -25, 150)
scene_root:add_child(mountain)

tree = gr.ltree('tree', {{"F", "F[C/C|A\\][C\\C|B/]"}, {"A", "A[C/C|A\\][C\\B/]"}, {"B", "B[C/A\\][C\\C|B/]"} }, 4, 90.0, 200, 10)
tree:set_material(rust)
tree:scale(0.1, 0.15, 0.1)
tree:rotate('X', 90)
tree:translate(0, 1, -60)
scene_root:add_child(tree)

cat = gr.mesh('cat', 'catN.obj')
cat:scale(5, 5, 5)
cat:rotate('z', 90)
cat:rotate('x', -90)
cat:rotate('y', 180)
cat:translate(15, 50, 85)
cat:set_material(glass)

scene_root:add_child(cat)

monkey_head_left = gr.mesh('monkey_head_left', 'suzanne.obj')
monkey_head_left:scale(4, 4, 4)
monkey_head_left:rotate('y', 200)
monkey_head_left:translate(49, 12, 35)
monkey_head_left:set_material(mat1)
scene_root:add_child(monkey_head_left)

column_left = gr.cylinder('column')
column_left:scale(3, 60, 3)
column_left:translate(50, -20, 37)
column_left:set_material(gloss)
scene_root:add_child(column_left)


monkey_head_right = gr.mesh('monkey_head_right', 'suzanne.obj')
monkey_head_right:scale(4, 4, 4)
monkey_head_right:rotate('y', 160)
monkey_head_right:translate(-49, 12, 35)
monkey_head_right:set_material(column_mat)
scene_root:add_child(monkey_head_right)

column_right = gr.cylinder('column_right')
column_right:scale(3, 60, 3)
column_right:translate(-50, -20, 37)
column_right:set_material(gloss)
scene_root:add_child(column_right)




white_light = gr.light({-100.0, 300.0, 200.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({-400.0, 100.0, -150.0}, {0.6, 0.0, 0.9}, {1, 0, 0})
cyan_light = gr.light({400.0, 100.0, -150.0}, {0.0, 0.6, 0.9}, {1, 0, 0})

test_light = gr.light({0, 300, -100}, {0.6, 0.6, 0.9}, {1, 0, 0})

gr.render(scene_root, 'screenshot.png', 1600, 900,
	  {0, 15, -110}, {0, 40, 50}, {0, 1, 0}, 45,
	  {0.5, 0.3, 0.0}, {magenta_light, cyan_light, white_light}, false)
