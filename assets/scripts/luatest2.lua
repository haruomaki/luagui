create_window(800, 600, "Test Window", function()
    local world = create_world()
    world.b2world.gravity = { 0, -0.03 }
    print(world.b2world.gravity)

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    local line_obj = world:draw_line(arr)
    local rbc = line_obj:add_rigidbody_component() -- 物理演算を追加
    rbc.body:add_shape({})                         -- 衝突形状を追加

    world:draw_circle({ 0, 0 }, 0.04)
    local c2 = world:draw_circle({ -0.01, 0 }, 0.03)

    c2:add_update_component(function()
        print("3...")
        Wait(1.7)
        print("2...")
        Wait(1.2)
        print("1...")
        Wait(0.9)
        print("0!")

        Interval(function()
            print("あいうえお")
        end, 2)
    end)

    local radius = 0.01
    WaitWith(10, function(t)
        local theta = 2 * math.pi * (t / 2)
        c2.position = { radius * math.cos(theta), radius * math.sin(theta) }
    end)
end)
