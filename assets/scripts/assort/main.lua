run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    camera.owner.position = vec3.new(0, 0, 0)

    world.root:draw_circle({ 0, 0 }, 1)
    Forever(function()
        if GetKeyDown('Enter') then camera:toggle_mode() end
    end)
end)
