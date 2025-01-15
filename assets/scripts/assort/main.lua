run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    camera.owner.position = vec3.new(0, 0, 0)

    Util.cube()

    V2 { { 0, 1, 1 } }
    print(V3.new { { 0, 1 }, { 12 } })

    world.root:draw_circle({ 0, 0 }, 1)
    Forever(function()
        if GetKeyDown('Enter') then camera:toggle_mode() end
    end)
end)
