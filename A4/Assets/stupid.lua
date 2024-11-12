-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
glass = gr.material({1.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, 25, 1.0, 0.9)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {10, -10, -120}, 50)
scene_root:add_child(s1)
s1:set_material(mat1)

s6 = gr.nh_sphere('s6', {0, 0, -10}, 50)
scene_root:add_child(s6)
s6:set_material(glass)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'simple.png', 800, 450,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 30,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
