mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
emissive = gr.material({0.0, 0.0, 0.0}, {0.3, 0.3, 0.7}, 25, {0.0, 0.0, 0.0}, 1.0, 1.0, 1.0, {-0.00, -0.02, -0.02}, 0.0, 0.0, '', '')

scene_root = gr.node('root')


cylinder = gr.cylinder('cylinder')
cylinder:scale(50, 100, 50)
cylinder:set_material(mat1)
scene_root:add_child(cylinder)

cylinder1 = gr.cylinder('cylinder1')
cylinder1:scale(70, 100, 70)
cylinder1:set_material(emissive)
scene_root:add_child(cylinder1)

test_light = gr.light({0, 100, -200}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'cylinder.png', 400, 225,
	  {0, 0, -200}, {0, 0, 50}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {test_light})
