
local root = gr.node('root')
local light_brown = gr.material({0.6, 0.4, 0.2}, {1, 1, 1}, 10)
local dark_brown = gr.material({0.4, 0.3, 0.2}, {1, 1, 1}, 10)
local pinkish = gr.material({1.0, 0.8, 0.6}, {1, 1, 1}, 10)

local torso_joint = gr.joint('torso_joint', {0, 0, 0}, {0, 0, 0})
root:add_child(torso_joint)

local upper_torso_joint_translate = gr.joint('upper_torso_joint_translate', {0, 0, 0}, {0, 0, 0})
local upper_torso_joint = gr.joint('upper_torso_joint', {0, 0, 0}, {0, 0, 0})
upper_torso_joint_translate:translate(0, 0.75, 0) 
torso_joint:add_child(upper_torso_joint_translate)
upper_torso_joint_translate:add_child(upper_torso_joint)

local torso = gr.mesh('sphere', 'torso_geom')
torso:set_material(light_brown)
torso:scale(0.8, 0.75, 0.5)
torso_joint:translate(0, 1.5, 0)
torso_joint:add_child(torso)

local upper_torso = gr.mesh('sphere', 'upper_torso_geom')
upper_torso:set_material(dark_brown)
upper_torso:scale(1.0, 0.75, 0.5)
upper_torso_joint:add_child(upper_torso)

local lower_torso_joint = gr.joint('lower_torso_joint', {0, 0, 0}, {0, 0, 0})
lower_torso_joint:translate(0, -0.75, 0)
torso_joint:add_child(lower_torso_joint)

local lower_torso = gr.mesh('sphere', 'lower_torso_geom')
lower_torso:set_material(dark_brown)
lower_torso:scale(1.0, 0.75, 0.5)
lower_torso_joint:add_child(lower_torso)

local neck_joint_lower = gr.joint('neck_joint_lower', {-30, 0, 30}, {0, 0, 0})
neck_joint_lower:translate(0, 1.0, 0) 
upper_torso_joint:add_child(neck_joint_lower)

local neck_joint = gr.joint('neck_joint', {0, 0, 0}, {0, 0, 0})
neck_joint_lower:add_child(neck_joint)

local neck = gr.mesh('sphere', 'neck_geom')
neck:set_material(dark_brown)
neck:scale(0.3, 0.5, 0.3)
neck_joint:add_child(neck)

local neck_joint_upper = gr.joint('neck_joint_upper', {0, 0, 0}, {-30, 0, 30})
neck_joint:add_child(neck_joint_upper)

local head_joint = gr.joint('head_joint', {0, 0, 0}, {0, 0, 0})
head_joint:translate(0, 0.3, 0)
neck_joint_upper:add_child(head_joint)

local head = gr.mesh('suzanne', 'monke_head')
head:set_material(pinkish)
head:scale(0.6, 0.6, 0.6)
head_joint:add_child(head)

for side, direction in pairs({left = -1, right = 1}) do
    local shoulder_joint_translate = gr.joint('shoulder_translate_' .. side, {0, 0, 0}, {0, 0, 0})
    local shoulder_joint = gr.joint('shoulder_' .. side, {-60, 0, 120}, {0, 0, 0})
    shoulder_joint_translate:rotate('y', 180)
    shoulder_joint_translate:rotate('z', 20 * direction)
    
    shoulder_joint_translate:translate(0.8 * direction, 0.5, 0)
    upper_torso_joint:add_child(shoulder_joint_translate)
    shoulder_joint_translate:add_child(shoulder_joint)

    local upper_arm_joint = gr.joint('upper_arm_' .. side, {-90, 0, 90}, {-30, 0, 30})
    upper_arm_joint:translate(0, -0.8, 0)
    shoulder_joint:add_child(upper_arm_joint)

    local upper_arm = gr.mesh('sphere', 'upper_arm_geom_' .. side)
    upper_arm:set_material(dark_brown)
    upper_arm:scale(0.4, 1.0, 0.4) 
    upper_arm_joint:add_child(upper_arm)

    local elbow_joint_translate = gr.joint('elbow_translate_' .. side, {0, 0, 0}, {0, 0, 0})
    local elbow_joint = gr.joint('elbow_' .. side, {0, 0, 90}, {0, 0, 0})
    elbow_joint_translate:translate(0, -0.8, 0)
    upper_arm_joint:add_child(elbow_joint_translate)
    elbow_joint_translate:add_child(elbow_joint)

    local lower_arm_joint = gr.joint('lower_arm_' .. side, {-90, 0, 90}, {-30, 0, 30})
    lower_arm_joint:translate(0, -0.8, 0)
    elbow_joint:add_child(lower_arm_joint)

    local lower_arm = gr.mesh('sphere', 'lower_arm_geom_' .. side)
    lower_arm:set_material(dark_brown)
    lower_arm:scale(0.3, 0.8, 0.3)
    lower_arm_joint:add_child(lower_arm)

    local wrist_joint_translate = gr.joint('wrist_trans' .. side, {0, 0, 0}, {0, 0, 0})
    local bounds = {-90, 0, 0}
    if direction == 1 then 
        bounds = {0, 0, 90}
    end
    local wrist_joint = gr.joint('wrist_' .. side, {0, 0, 0}, bounds)

    -- wrist_joint_translate:rotate('x', 60)
    wrist_joint_translate:translate(0, -0.6, 0)
    -- wrist_joint_translate:rotate('x', -30)

    lower_arm_joint:add_child(wrist_joint_translate)
    wrist_joint_translate:add_child(wrist_joint)

    local hand_joint = gr.joint('hand_joint' .. side, {0, 0, 0}, {0, 0, 0});

    local hand = gr.mesh('sphere', 'hand_geom_' .. side)
    hand:set_material(pinkish)
    hand:scale(0.2, 0.4, 0.4)
    hand:translate(0, -0.3, 0)
    wrist_joint:add_child(hand_joint)
    hand_joint:add_child(hand)
end

for side, direction in pairs({left = -1, right = 1}) do

    local hip_joint = gr.joint('hip_' .. side, {-45, 0, 45}, {0, 0, 0})
    hip_joint:translate(0.5 * direction, -1.0, 0)
    lower_torso_joint:add_child(hip_joint)

    local upper_leg_joint = gr.joint('upper_leg_' .. side, {-90, 0, 90}, {-30, 0, 30})
    upper_leg_joint:translate(0, -0.4, 0)
    hip_joint:add_child(upper_leg_joint)

    local upper_leg = gr.mesh('sphere', 'upper_leg_geom_' .. side)
    upper_leg:set_material(dark_brown)
    upper_leg:scale(0.4, 1.0, 0.4)
    upper_leg_joint:add_child(upper_leg)

    local knee_joint_translate = gr.joint('knee_translate_' .. side, {0, 0, 0}, {0, 0, 0})
    local knee_joint = gr.joint('knee_' .. side, {-20, 0,90}, {0, 0, 0})
    knee_joint_translate:translate(0, -0.8, 0)
    upper_leg_joint:add_child(knee_joint_translate)
    knee_joint_translate:add_child(knee_joint)

    local lower_leg_joint = gr.joint('lower_leg_' .. side, {-90, 0, 90}, {-30, 0, 30})
    lower_leg_joint:translate(0, -0.8, 0)
    knee_joint:add_child(lower_leg_joint)

    local lower_leg = gr.mesh('sphere', 'my_lower_leg_' .. side)
    lower_leg:set_material(dark_brown)
    lower_leg:scale(0.35, 0.8, 0.35)
    lower_leg_joint:add_child(lower_leg)

    local ankle_joint_translate = gr.joint('ankle_translate_' .. side, {0, 0, 0}, {0, 0, 0})
    local ankle_joint = gr.joint('ankle_' .. side, {-20, 0, 90}, {0, 0, 0})
    ankle_joint_translate:translate(0, -0.8, 0)
    lower_leg_joint:add_child(ankle_joint_translate)
    ankle_joint_translate:add_child(ankle_joint)

    local foot_joint = gr.joint('foot_joint_' .. side, {0,0,0}, {0,0,0})
    local foot = gr.mesh('sphere', 'foot_geom_' .. side)
    foot:set_material(pinkish)
    foot:scale(0.4, 0.2, 0.5)
    foot:translate(0, 0, 0.25)
    ankle_joint:add_child(foot_joint)
    foot_joint:add_child(foot)
end

return root