-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
glass = gr.material({1.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, 25, 1.2, 0.9)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {-200, 0, -120}, 200)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 0, -120}, 200)
scene_root:add_child(s2)
s2:set_material(mat2)

s6 = gr.nh_sphere('s6', {0, 0, 500}, 50)
scene_root:add_child(s6)
s6:set_material(glass)

s7 = gr.nh_sphere('s7', {-10, -20, 100}, 50)
scene_root:add_child(s7)
s7:set_material(glass)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'stupid.png', 800, 450,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 30,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
