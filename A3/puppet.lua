function create_puppet()
    -- Create the root node for the puppet
    local myroot = gr.node('root')

    -- Define material properties
    local torso_material = gr.material({0.6, 0.4, 0.2}, {0.8, 0.6, 0.4}, 10)
    local neck_material = gr.material({0.5, 0.3, 0.2}, {0.7, 0.5, 0.4}, 10)
    local head_material = gr.material({1.0, 0.8, 0.6}, {1.0, 0.8, 0.6}, 10)
    local upper_arm_material = gr.material({0.6, 0.3, 0.3}, {0.8, 0.5, 0.5}, 10)
    local lower_arm_material = gr.material({0.5, 0.3, 0.3}, {0.7, 0.4, 0.4}, 10)
    local hand_material = gr.material({0.6, 0.5, 0.4}, {0.8, 0.7, 0.6}, 10)
    local upper_leg_material = gr.material({0.5, 0.4, 0.3}, {0.7, 0.6, 0.5}, 10)
    local lower_leg_material = gr.material({0.4, 0.3, 0.2}, {0.6, 0.5, 0.4}, 10)
    local foot_material = gr.material({0.3, 0.2, 0.1}, {0.5, 0.4, 0.3}, 10)

    -- Create the main torso joint
    local torso_joint = gr.joint('torso_joint', {0, 0, 0}, {0, 0, 0})
    myroot:add_child(torso_joint)

    -- Create the upper torso joint (for the arms)
    local upper_torso_joint_translate = gr.joint('upper_torso_joint_translate', {0, 0, 0}, {0, 0, 0})
    local upper_torso_joint = gr.joint('upper_torso_joint', {0, 0, 0}, {0, 0, 0})
    upper_torso_joint_translate:translate(0, 0.75, 0)  -- Position above main torso
    torso_joint:add_child(upper_torso_joint_translate)
    upper_torso_joint_translate:add_child(upper_torso_joint)

    local torso = gr.mesh('sphere', 'my_torso')
    torso:set_material(torso_material)
    torso:scale(1.0, 0.75, 0.5)
    torso_joint:add_child(torso)


    -- Create the upper torso geometry node and attach it to the upper torso joint
    local upper_torso = gr.mesh('sphere', 'my_upper_torso')
    upper_torso:set_material(torso_material)
    upper_torso:scale(1.0, 0.75, 0.5)  -- Scale upper torso to be flatter and wider
    upper_torso_joint:add_child(upper_torso)

    -- Create the lower torso joint (for the legs)
    local lower_torso_joint = gr.joint('lower_torso_joint', {0, 0, 0}, {0, 0, 0})
    lower_torso_joint:translate(0, -0.75, 0)  -- Position below main torso
    torso_joint:add_child(lower_torso_joint)

    -- Create the lower torso geometry node and attach it to the lower torso joint
    local lower_torso = gr.mesh('sphere', 'my_lower_torso')
    lower_torso:set_material(torso_material)
    lower_torso:scale(1.0, 0.75, 0.5)  -- Scale lower torso to match
    lower_torso_joint:add_child(lower_torso)

    -- Create neck joint (directly attached to upper torso joint)
    local neck_joint_lower = gr.joint('neck_joint_lower', {-30, 0, 30}, {0, 0, 0})
    neck_joint_lower:translate(0, 1.0, 0)  -- Position neck above upper torso
    upper_torso_joint:add_child(neck_joint_lower)

    local neck_joint = gr.joint('neck_joint', {0, 0, 0}, {0, 0, 0})
     -- Position neck above upper torso
     neck_joint_lower:add_child(neck_joint)

    -- Create the neck geometry node and attach it to the neck joint
    local neck = gr.mesh('sphere', 'my_neck')
    neck:set_material(neck_material)
    neck:scale(0.3, 0.5, 0.3)  -- Scale neck to be thinner and taller
    neck_joint:add_child(neck)

    local neck_joint_upper = gr.joint('neck_joint_upper', {0, 0, 0}, {-30, 0, 30})
    neck_joint:add_child(neck_joint_upper)

    -- Create head joint (directly attached to neck joint)
    local head_joint = gr.joint('head_joint', {0, 0, 0}, {0, 0, 0})
    head_joint:translate(0, 0.5, 0)  -- Position head above neck
    neck_joint_upper:add_child(head_joint)

    -- Create the head geometry node and attach it to the head joint
    local head = gr.mesh('suzanne', 'my_head')
    head:set_material(head_material)
    head:scale(0.6, 0.6, 0.6)  -- Scale head to be a reasonable size
    head_joint:add_child(head)

    -- Create arms with joints for shoulder, elbow, wrist, and hand
    for side, direction in pairs({left = -1, right = 1}) do
        -- Shoulder Joint
        local shoulder_joint_translate = gr.joint('shoulder_' .. side, {0, 0, 0}, {0, 0, 0})
        local shoulder_joint = gr.joint('shoulder_' .. side, {-60, 0, 60}, {0, 0, 0})
        shoulder_joint_translate:rotate('z', 20 * direction)
        shoulder_joint_translate:translate(0.8 * direction, 0.5, 0)  -- Position shoulder joint
        upper_torso_joint:add_child(shoulder_joint_translate)
        shoulder_joint_translate:add_child(shoulder_joint)

        -- Upper Arm Joint
        local upper_arm_joint = gr.joint('upper_arm_' .. side, {-90, 0, 90}, {-30, 0, 30})
        upper_arm_joint:translate(0, -0.8, 0)  -- Position upper arm joint
        shoulder_joint:add_child(upper_arm_joint)

        -- Upper Arm Geometry Node
        local upper_arm = gr.mesh('sphere', 'my_upper_arm_' .. side)
        upper_arm:set_material(upper_arm_material)
        upper_arm:scale(0.4, 1.0, 0.4)  -- Scale upper arm
        upper_arm_joint:add_child(upper_arm)

        -- Elbow Joint
        local elbow_joint_translate = gr.joint('elbow_' .. side, {0, 0, 0}, {0, 0, 0})
        local elbow_joint = gr.joint('elbow_' .. side, {-90, 0, 0}, {0, 0, 0})
        elbow_joint_translate:translate(0, -0.8, 0)  -- Position elbow joint
        upper_arm_joint:add_child(elbow_joint_translate)
        elbow_joint_translate:add_child(elbow_joint)

        -- Lower Arm Joint
        local lower_arm_joint = gr.joint('lower_arm_' .. side, {-90, 0, 90}, {-30, 0, 30})
        lower_arm_joint:translate(0, -0.8, 0)  -- Position lower arm joint
        elbow_joint:add_child(lower_arm_joint)

        -- Lower Arm Geometry Node
        local lower_arm = gr.mesh('sphere', 'my_lower_arm_' .. side)
        lower_arm:set_material(lower_arm_material)
        lower_arm:scale(0.3, 0.8, 0.3)  -- Scale lower arm
        lower_arm_joint:add_child(lower_arm)

        -- Wrist Joint
        local wrist_joint_translate = gr.joint('wrist_trans' .. side, {0, 0, 0}, {0, 0, 0})
        local wrist_joint = gr.joint('wrist_' .. side, {0, 0, 0}, {-(45 -direction * 45), 0, 45 + direction * 45})
        wrist_joint_translate:rotate('x', -60)
        wrist_joint_translate:translate(0, -0.6, 0)
          -- Position wrist joint
        lower_arm_joint:add_child(wrist_joint_translate)
        wrist_joint_translate:add_child(wrist_joint)

        local hand_joint = gr.joint('hand_joint' .. side, {0, 0, 0}, {0, 0, 0});

        -- Hand Geometry Node
        local hand = gr.mesh('sphere', 'my_hand_' .. side)
        hand:set_material(hand_material)
        hand:scale(0.2, 0.4, 0.4)  -- Scale hand to be smaller
        hand:translate(0, -0.3, -0.4)
        wrist_joint:add_child(hand_joint)
        hand_joint:add_child(hand)
    end

    -- Create legs with joints for hip, knee, ankle, and foot
    for side, direction in pairs({left = -1, right = 1}) do
        -- Hip Joint
        local hip_joint = gr.joint('hip_' .. side, {-45, 0, 45}, {0, 0, 0})
        hip_joint:translate(0.5 * direction, -1.0, 0)  -- Position hip joint
        lower_torso_joint:add_child(hip_joint)

        -- Upper Leg Joint
        local upper_leg_joint = gr.joint('upper_leg_' .. side, {-90, 0, 90}, {-30, 0, 30})
        upper_leg_joint:translate(0, -0.4, 0)  -- Position upper leg joint
        hip_joint:add_child(upper_leg_joint)

        -- Upper Leg Geometry Node
        local upper_leg = gr.mesh('sphere', 'my_upper_leg_' .. side)
        upper_leg:set_material(upper_leg_material)
        upper_leg:scale(0.4, 1.0, 0.4)  -- Scale upper leg
        upper_leg_joint:add_child(upper_leg)

        -- Knee Joint
        local knee_joint_translate = gr.joint('knee_' .. side, {0, 0, 0}, {0, 0, 0})
        local knee_joint = gr.joint('knee_' .. side, {-20, 0,90}, {0, 0, 0})
        knee_joint_translate:translate(0, -0.8, 0)  -- Position knee joint
        upper_leg_joint:add_child(knee_joint_translate)
        knee_joint_translate:add_child(knee_joint)

        -- Lower Leg Joint
        local lower_leg_joint = gr.joint('lower_leg_' .. side, {-90, 0, 90}, {-30, 0, 30})
        lower_leg_joint:translate(0, -0.8, 0)  -- Position lower leg joint
        knee_joint:add_child(lower_leg_joint)

        -- Lower Leg Geometry Node
        local lower_leg = gr.mesh('sphere', 'my_lower_leg_' .. side)
        lower_leg:set_material(lower_leg_material)
        lower_leg:scale(0.35, 0.8, 0.35)  -- Scale lower leg
        lower_leg_joint:add_child(lower_leg)

        -- Ankle Joint
        local ankle_joint_translate = gr.joint('ankle_' .. side, {0, 0, 0}, {0, 0, 0})
        local ankle_joint = gr.joint('ankle_' .. side, {-20, 0, 90}, {0, 0, 0})
        ankle_joint_translate:translate(0, -0.8, 0)  -- Position ankle joint
        lower_leg_joint:add_child(ankle_joint_translate)
        ankle_joint_translate:add_child(ankle_joint)

        -- Foot Geometry Node
        local foot_joint = gr.joint('foot_joint_' .. side, {0,0,0}, {0,0,0})
        local foot = gr.mesh('sphere', 'my_foot_' .. side)
        foot:set_material(foot_material)
        foot:scale(0.4, 0.2, 0.5)  -- Scale foot
        foot:translate(0, 0, 0.25)
        ankle_joint:add_child(foot_joint)
        foot_joint:add_child(foot)
    end

    -- Set the translation of the main torso joint to elevate it from the ground
    torso_joint:translate(0, 1.5, 0)

    -- Return the root node of the puppet
    return myroot
end



return create_puppet()