-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
glass = gr.material({0.0, 0.2, 0.2}, {0.5, 0.7, 0.5}, 60, 1.2, 0.9)
blue_glass = gr.material({1.0, 1.0, 1.0}, {0.5, 0.7, 0.5}, 60, 1.2, 0.996)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {-200, 0, -620}, 200)
scene_root:add_child(s1)
s1:set_material(mat2)

s2 = gr.nh_sphere('s2', {200, 0, -620}, 200)
scene_root:add_child(s2)
s2:set_material(mat3)


-- test = gr.node('test')
-- -- test:rotate('y', 180)
-- s8 = gr.nh_box('c1', {-50, -50, -400}, 100)
-- -- s8:scale(40, 40, 40)

-- -- s8:translate(0, 0, -400)
-- s8:set_material(mat2)
-- -- test:rotate('y', 180)
-- test:add_child(s8)
-- scene_root:add_child(test)

s9 = gr.nh_sphere('s9', {0, 0, 200}, 50)
scene_root:add_child(s9)
s9:set_material(glass)

-- s6 = gr.mesh('cat', 'cat.obj')
-- s6:scale(20, 20, 20)
-- s6:rotate('z', 90)
-- s6:rotate('x', -90)
-- s6:translate(-20, 30, -200)
-- scene_root:add_child(s6)
-- s6:set_material(blue_glass)

s7 = gr.nh_sphere('s7', {-10, -20, -400}, 50)
scene_root:add_child(s7)
s7:set_material(glass)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'stupid.png', 800, 450,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 30,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
