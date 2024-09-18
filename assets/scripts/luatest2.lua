---丸を生成
---@param world World
local function maru(world, x, y)
    local obj = world:draw_circle({ x, y }, 0.01)
    local rb = obj:add_rigidbody_component()
    rb:add_shape({ shape = "circle", radius = 0.01 })
end

create_window(800, 600, "Test Window", function()
    local world = create_world()
    world.b2world.gravity = { 0, -0.1 }
    print(world.b2world.gravity)

    -- local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    -- local line_obj = world:draw_line(arr)
    -- local rb = line_obj:add_rigidbody_component()     -- 物理演算を追加
    -- rb:add_shape({ shape = "circle", radius = 0.01 }) -- 衝突形状を追加

    -- world:draw_circle({ 0, 0 }, 0.04)
    -- local c2 = world:draw_circle({ -0.01, 0 }, 0.03)

    -- 床と壁の剛体を作成
    local yuka_obj = world:draw_line({ { -1, -0.04 }, { 1, -0.04 } })
    local yuka_rb = yuka_obj:add_rigidbody_component()
    yuka_rb:add_shape({ shape = "line" })

    -- c2:add_update_component(function()
    --     print("3...")
    --     maru(world, -0.02, 0.02)

    --     Wait(1.7)
    --     print("2...")
    --     maru(world, 0.01, 0.01)

    --     Wait(1.2)
    --     print("1...")
    --     maru(world, 0.02, 0.015)

    --     Wait(0.9)
    --     print("0!")

    --     Interval(function()
    --         print("あいうえお")
    --     end, 2)
    -- end)

    -- local radius = 0.01
    -- WaitWith(10, function(t)
    --     local theta = 2 * math.pi * (t / 2)
    --     c2.position = { radius * math.cos(theta), radius * math.sin(theta) }
    -- end)

    maru(world, 0, 0)

    Wait(1)

    maru(world, 0.005, 0)

    Wait(1)

    maru(world, 0.01, 0.02)

    Wait(1)

    Wait(10)
end)
