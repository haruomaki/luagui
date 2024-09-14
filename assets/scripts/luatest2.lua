create_window(800, 600, "Test Window", function()
    local world = create_world()

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    world:draw_line(arr)
    world:draw_circle({ 0, 0 }, 0.04)
    local c2 = world:draw_circle({ -0.01, 0 }, 0.03)

    c2:add_update_component(function()
        -- while true do
        --     print("あいうえお")
        --     Yield()
        -- end
        print("あいうえお")
    end)

    local radius = 0.01
    WaitWith(10, function(t)
        local theta = 2 * math.pi * (t / 2)
        c2.position = { radius * math.cos(theta), radius * math.sin(theta) }
    end)
end)
