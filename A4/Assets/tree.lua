mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)

scene_root = gr.node('root')


tree = gr.ltree('tree', {{"F", "F[C/C+A\\][C\\C+B/]"}, {"A", "A[C/C+A\\][C\\B/]"}, {"B", "B[C/A\\][C\\C+B/]"} }, 4, 90.0, 200, 10)
tree:set_material(mat1)
tree:scale(0.2, 0.2, 0.2)
tree:rotate('X', 90)
tree:translate(0, -100, 0)
scene_root:add_child(tree)

test_light = gr.light({0, 100, -200}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'tree.png', 400, 225,
	  {0, 0, -200}, {0, 0, 50}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {test_light})
