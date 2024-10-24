
local root = gr.node('root')
local light_brown = gr.material({0.6, 0.4, 0.2}, {1, 1, 1}, 10)
local dark_brown = gr.material({0.4, 0.3, 0.2}, {1, 1, 1}, 10)
local pinkish = gr.material({1.0, 0.8, 0.6}, {1, 1, 1}, 10)

local torso_joint = gr.joint('torso_joint', {0, 0, 0}, {0, 0, 0})
root:add_child(torso_joint)

local upper_torso_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
local upper_torso_joint = gr.joint('upper_torso_joint', {0, 0, 0}, {0, 0, 0})
upper_torso_joint_translate:translate(0, 0.75, 0) 
torso_joint:add_child(upper_torso_joint_translate)
upper_torso_joint_translate:add_child(upper_torso_joint)

local torso = gr.mesh('sphere', '')
torso:set_material(light_brown)
torso:scale(0.8, 0.75, 0.5)
torso_joint:translate(0, 1.5, 0)
torso_joint:add_child(torso)

local upper_torso = gr.mesh('sphere', '')
upper_torso:set_material(dark_brown)
upper_torso:scale(1.0, 0.75, 0.5)
upper_torso_joint:add_child(upper_torso)

local lower_torso_joint = gr.joint('lower_torso_joint', {0, 0, 0}, {0, 0, 0})
lower_torso_joint:translate(0, -0.75, 0)
torso_joint:add_child(lower_torso_joint)

local lower_torso = gr.mesh('sphere', '')
lower_torso:set_material(dark_brown)
lower_torso:scale(1.0, 0.75, 0.5)
lower_torso_joint:add_child(lower_torso)

local upper_tail_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
local upper_tail_joint = gr.joint('upper_tail_joint', {-20, 0, 20}, {0, 0, 0})
lower_torso_joint:add_child(upper_tail_joint_translate)
upper_tail_joint_translate:translate(0, -0.2, -0.05)
upper_tail_joint_translate:add_child(upper_tail_joint)
local tail = gr.mesh('sphere', '')
tail:set_material(dark_brown)
local tail_joint = gr.joint('tail_joint', {0, 0, 0}, {0, 0, 0})
tail:translate(0, 0, -0.5)
tail:scale(0.1, 0.1, 1);
upper_tail_joint:add_child(tail_joint)
tail_joint:add_child(tail)

local lower_tail_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
lower_tail_joint_translate:rotate('x', -30)
local lower_tail_joint = gr.joint('lower_tail_joint', {0, 0, 0}, {0, 0, 0})
local lower_tail = gr.mesh('sphere', '')
lower_tail:set_material(dark_brown)
lower_tail:scale(0.1, 0.1, 1);
lower_tail:translate(0, 0.7, -2.2)

lower_tail_joint_translate:add_child(lower_tail_joint)
lower_tail_joint:add_child(lower_tail)
upper_tail_joint:add_child(lower_tail_joint_translate)

local neck_joint_lower = gr.joint('neck_joint_lower', {-30, 0, 30}, {0, 0, 0})
neck_joint_lower:translate(0, 1.0, 0) 
upper_torso_joint:add_child(neck_joint_lower)

local neck_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
neck_joint_lower:add_child(neck_joint)

local neck = gr.mesh('sphere', '')
neck:set_material(dark_brown)
neck:scale(0.3, 0.5, 0.3)
neck_joint:add_child(neck)

local neck_joint_upper = gr.joint('neck_joint_upper', {0, 0, 0}, {-30, 0, 30})
neck_joint:add_child(neck_joint_upper)

local head_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
head_joint:translate(0, 0.3, 0)
neck_joint_upper:add_child(head_joint)

local head = gr.mesh('suzanne', 'monke_head')
head:set_material(pinkish)
head:scale(0.6, 0.6, 0.6)
head_joint:add_child(head)

function generate_arm(direction, connecting_joint)
    local shoulder_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
    local shoulder_joint = gr.joint('shoulder_joint', {-60, 0, 120}, {0, 0, 0})
    shoulder_joint_translate:rotate('y', 180)
    shoulder_joint_translate:rotate('z', 20 * direction)
    
    shoulder_joint_translate:translate(0.8 * direction, 0.5, 0)
    connecting_joint:add_child(shoulder_joint_translate)
    shoulder_joint_translate:add_child(shoulder_joint)

    local upper_arm_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
    upper_arm_joint:translate(0, -0.8, 0)
    shoulder_joint:add_child(upper_arm_joint)

    local upper_arm = gr.mesh('sphere', 'upper_arm')
    upper_arm:set_material(dark_brown)
    upper_arm:scale(0.4, 1.0, 0.4) 
    upper_arm_joint:add_child(upper_arm)

    local elbow_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
    local elbow_joint = gr.joint('elbow_jiont', {0, 0, 90}, {0, 0, 0})
    elbow_joint_translate:translate(0, -0.8, 0)
    upper_arm_joint:add_child(elbow_joint_translate)
    elbow_joint_translate:add_child(elbow_joint)

    local lower_arm_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
    lower_arm_joint:translate(0, -0.8, 0)
    elbow_joint:add_child(lower_arm_joint)

    local lower_arm = gr.mesh('sphere', 'lower_arm')
    lower_arm:set_material(dark_brown)
    lower_arm:scale(0.3, 0.8, 0.3)
    lower_arm_joint:add_child(lower_arm)

    local wrist_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
    local bounds = {-90, 0, 0}
    if direction == 1 then 
        bounds = {0, 0, 90}
    end
    local wrist_joint = gr.joint('wrist_joint', {0, 0, 0}, bounds)

    -- wrist_joint_translate:rotate('x', 60)
    wrist_joint_translate:translate(0, -0.6, 0)
    -- wrist_joint_translate:rotate('x', -30)

    lower_arm_joint:add_child(wrist_joint_translate)
    wrist_joint_translate:add_child(wrist_joint)

    local hand_joint = gr.joint('', {0, 0, 0}, {0, 0, 0});

    local hand = gr.mesh('sphere', 'hand')
    hand:set_material(pinkish)
    hand:scale(0.2, 0.4, 0.4)
    hand:translate(0, -0.3, 0)
    wrist_joint:add_child(hand_joint)
    hand_joint:add_child(hand)
end

function generate_leg(direction, connecting_joint)
    local hip_joint = gr.joint('hip_joint', {-45, 0, 45}, {0, 0, 0})
    hip_joint:translate(0.5 * direction, -1.0, 0)
    connecting_joint:add_child(hip_joint)

    local upper_leg_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
    upper_leg_joint:translate(0, -0.4, 0)
    hip_joint:add_child(upper_leg_joint)

    local upper_leg = gr.mesh('sphere', 'upper_leg')
    upper_leg:set_material(dark_brown)
    upper_leg:scale(0.4, 1.0, 0.4)
    upper_leg_joint:add_child(upper_leg)

    local knee_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
    local knee_joint = gr.joint('knee_joint', {-20, 0,90}, {0, 0, 0})
    knee_joint_translate:translate(0, -0.8, 0)
    upper_leg_joint:add_child(knee_joint_translate)
    knee_joint_translate:add_child(knee_joint)

    local lower_leg_joint = gr.joint('', {0, 0, 0}, {0, 0, 0})
    lower_leg_joint:translate(0, -0.8, 0)
    knee_joint:add_child(lower_leg_joint)

    local lower_leg = gr.mesh('sphere', 'lower_leg')
    lower_leg:set_material(dark_brown)
    lower_leg:scale(0.35, 0.8, 0.35)
    lower_leg_joint:add_child(lower_leg)

    local ankle_joint_translate = gr.joint('', {0, 0, 0}, {0, 0, 0})
    local ankle_joint = gr.joint('ankle_jiont', {-20, 0, 90}, {0, 0, 0})
    ankle_joint_translate:translate(0, -0.8, 0)
    lower_leg_joint:add_child(ankle_joint_translate)
    ankle_joint_translate:add_child(ankle_joint)

    local foot_joint = gr.joint('', {0,0,0}, {0,0,0})
    local foot = gr.mesh('sphere', 'foot')
    foot:set_material(pinkish)
    foot:scale(0.4, 0.2, 0.5)
    foot:translate(0, 0, 0.25)
    ankle_joint:add_child(foot_joint)
    foot_joint:add_child(foot)
end

for direction=-1, 1, 2 do

    generate_arm(direction, upper_torso_joint)
    generate_leg(direction, lower_torso_joint)
    
end

return root