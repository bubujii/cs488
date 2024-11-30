mat1 = gr.material({0.7, 1.0, 0.7}, {0.1, 0.1, 0.1}, 25)
water = gr.material({1.0, 0.4, 0.4}, {1.0, 0.0, 0.0}, 25,{0.0, 0.0, 0.0}, 1.1, 0.9, 1.0, {0.05, 0.05, 0.05})

scene_root = gr.node('root')


cat = gr.mesh('cat', 'suzanne.obj')
-- cat:scale(0.5, 0.5, 0.5)
-- cat:rotate('X', -90)
cat:rotate('Y', 180)
-- cat:translate(2, 2, 0)
cat:set_material(mat1)
scene_root:add_child(cat)


test_light = gr.light({0, 100, -200}, {0.6, 0.6, 0.6}, {1, 0, 0})

gr.render(scene_root, 'cat.png', 800, 450,
	  {0, 0, -3}, {0, 0, 0}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {test_light})
