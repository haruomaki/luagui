run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    camera.owner.position = vec3.new(0, 0, 0)

    world.root:draw_circle({ 0, 0 }, 1)
    Forever(function()
        if GetKey('Enter') then
            if camera.projection_mode == 'Perspective' then
                camera.projection_mode = 'Orthographic'
            else
                camera.projection_mode = 'Perspective'
            end
        end
    end)
end)
