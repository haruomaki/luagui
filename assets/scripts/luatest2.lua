create_window(800, 600, "Test Window", function()
    local world = create_world()

    local arr = { { 0, 0 }, { 0.03, 0.01 }, { 0.01, 0.03 }, { 0, 0 } }
    world:draw_line(arr)
    world:draw_circle({ 0, 0 }, 0.04)
    world:draw_circle({ -0.01, 0 }, 0.03)

    Wait(30)
end)
