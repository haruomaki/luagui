run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    camera.owner.position = vec3.new(0, 0, 0)

    local rb = Util.cube():add_rigidbody()
    rb.mass = 0
    rb.inertia = { 1, 1, 1 }
    rb.restitution = 0.5

    world.root:draw_circle({ 0, 0 }, 1)
    Forever(function()
        if GetKeyDown('Enter') then camera:toggle_mode() end
    end)
end)
