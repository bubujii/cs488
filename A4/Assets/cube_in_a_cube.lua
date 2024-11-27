-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({0.4, 0.4, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.4, 0.2, 0.6}, {0.5, 0.7, 0.8}, 0)
glass = gr.material({0.0, 0.0, 0.1}, {0.3, 0.3, 0.7}, 25, 1.2, 0.906, 0.1)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 0)
blue_thing = gr.material({0.1, 0.4, 0.6}, {0.2, 0.5, 0.7}, 25)
ball_top_col = gr.material({0.2, 0.6, 0.6}, {0.5, 0.7, 0.8}, 0)

orange_ish = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

pure_red = gr.material({1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, 25, 1.2, 0.9, 0.0)

scene_root = gr.node('root')
-- scene_root:rotate('X', -0.)

big_cube = gr.cube('big_cube')
big_cube:scale(100, 100, 100)
big_cube:set_material(pure_red)

scene_root:add_child(big_cube)

small_cube = gr.cylinder('small_cube')
small_cube:scale(50, 50, 50)
small_cube:translate(25, 25, 25)
small_cube:set_material(mat1)

scene_root:add_child(small_cube)

test_light = gr.light({0, 100, -200}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'screenshot.png', 400, 225,
	  {0, 0, -200}, {0, 0, 50}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {test_light})
