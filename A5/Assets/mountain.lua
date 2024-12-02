mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

scene_root = gr.node('root')

mountain = gr.mountain('mountain', 25, 0.8)
mountain:scale(20, 20, 20)
mountain:set_material(mat3)
mountain:translate(0, -120, -400)
scene_root:add_child(mountain)

white_light = gr.light({-100.0, 1400, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'mountain.png', 400, 225,
	  {0, 20, 100}, {0, 0, -400}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})