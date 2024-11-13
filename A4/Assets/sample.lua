-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({0.4, 0.4, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.4, 0.2, 0.6}, {0.5, 0.7, 0.8}, 0)
glass = gr.material({0.0, 0.0, 1.0}, {0.3, 0.3, 0.7}, 25, 1.2, 0.906)
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 0)
blue_thing = gr.material({0.1, 0.4, 0.6}, {0.2, 0.5, 0.7}, 25)
ball_top_col = gr.material({0.2, 0.6, 0.6}, {0.5, 0.7, 0.8}, 0)

orange_ish = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

scene_root = gr.node('root')
-- scene_root:rotate('X', -0.)

plane = gr.mesh('plane', 'plane.obj' )
scene_root:add_child(plane)
plane:set_material(grey)
plane:scale(1000, 1000, 1000)

pillar = gr.cube('pillar')
pillar:scale(8, 100, 8)
pillar:set_material(stone)

for i = 1, 6 do
    a_pillar = gr.node('pillar' .. tostring(i))
    a_pillar:translate(-50, 0, -250 + 50 * i)
    scene_root:add_child(a_pillar)
    a_pillar:add_child(pillar)
 end

 for i = 1, 6 do
    a_pillar = gr.node('pillar' .. tostring(i))
    a_pillar:translate(50, 0, -250 + 50 * i)
    scene_root:add_child(a_pillar)
    a_pillar:add_child(pillar)
 end
platform_node = gr.node('platform_node')

back_ball = gr.sphere('back_ball')
back_ball:scale(10, 10, 10)
back_ball:translate(25, 10, -20)
back_ball:set_material(mat4)
platform_node:add_child(back_ball)

back_ball2 = gr.sphere('back_ball2')
back_ball2:scale(10, 10, 10)
back_ball2:translate(-25, 10, -20)
back_ball2:set_material(mat4)
platform_node:add_child(back_ball2)

back_ball3 = gr.sphere('back_ball3')
back_ball3:scale(10, 10, 10)
back_ball3:translate(25, 10, 20)
back_ball3:set_material(mat4)
platform_node:add_child(back_ball3)

back_ball4 = gr.sphere('back_ball4')
back_ball4:scale(10, 10, 10)
back_ball4:translate(-25, 10, 20)
back_ball4:set_material(mat4)
platform_node:add_child(back_ball4)

platform = gr.cube('platform')
platform:scale(54, 2, 54)
platform:translate(-27, 20, -27)
platform:set_material(mat2)
platform_node:add_child(platform)

platform_node:translate(5, 0, -50)
scene_root:add_child(platform_node)

weird_pillars = gr.node('weird_pillars')

arch1 = gr.sphere('arch1')
arch1:scale(10, 50, 1)
arch1:translate(-15, 0, 0)
arch1:set_material(mat3)
weird_pillars:add_child(arch1)

arch2 = gr.sphere('arch2')
arch2:scale(10, 50, 1)
arch2:translate(15, 0, 0)
arch2:set_material(mat3)
weird_pillars:add_child(arch2)


ball_top = gr.sphere('ball_top')
ball_top:scale(10, 10, 10)
ball_top:translate(-15, 50, 0)
ball_top:set_material(orange_ish)

ball_top1 = gr.sphere('ball_top1')
ball_top1:scale(10, 10, 10)
ball_top1:translate(15, 50, 0)
ball_top1:set_material(orange_ish)


weird_pillars:add_child(ball_top)
weird_pillars:add_child(ball_top1)

weird_pillars:translate(5, 0, -25)
scene_root:add_child(weird_pillars)

cat = gr.mesh('cat', 'cat.obj')
cat:scale(5, 5, 5)
cat:rotate('z', 90)
cat:rotate('x', -90)
cat:rotate('y', 180)
cat:translate(15, 75, -50)
cat:set_material(glass)

scene_root:add_child(cat)



white_light_cam = gr.light({5, 14, -100}, {0.9, 0.4, 0.3}, {1, 0, 0})
white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.2}, {1, 0, 0})

gr.render(scene_root, 'screenshot.png', 1600, 900,
	  {5, 6, -200}, {0, 50, -50}, {0, 1, 0}, 60,
	  {0.5, 0.3, 0.0}, {white_light_cam, white_light, magenta_light})
