run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    camera.owner.position = vec3.new(0, 0, 0)

    Util.cube()

    V2 { { 0, 1, 1 } }
    print(V2.new { { 0, 1 }, { 12 } })
    -- V2.hello()

    world.root:draw_circle({ 0, 0 }, 1)
    Forever(function()
        if GetKeyDown('Enter') then camera:toggle_mode() end
    end)
end)
