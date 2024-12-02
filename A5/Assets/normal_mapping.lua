grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 0, {0.15, 0.15, 0.3}, 0.0, 0.0, 0.0, {0.05, 0.05, 0.05}, 0.0, 0.0, '', 'water.png')
scene_root = gr.node('root')


plane = gr.mesh('plane', 'plane.obj')
plane:set_material(grey)
plane:scale(100, 100, 100)
plane:rotate('X', -90)
scene_root:add_child(plane)

test_light = gr.light({0, 0, -200}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'normal_mapping.png', 400, 225,
	  {0, 20, -200}, {0, 0, 0}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {test_light})