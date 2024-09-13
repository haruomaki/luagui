create_window(800, 600, "Test Window", function()
    local world = create_world()

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    world:draw_line(arr)
    world:draw_circle({ 0, 0 }, 0.04)
    local c2 = world:draw_circle({ -0.01, 0 }, 0.03)

    local radius = 0.01
    WaitWith(10, function(t)
        c2:set_position({ radius * math.cos(t), radius * math.sin(t), 0 })
    end)
end)
