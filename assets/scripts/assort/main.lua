run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move")
    camera.owner.position = vec3.new(0, 0, -0.1)
    -- camera.owner.scale_prop = 2
    -- world.b2world.gravity = { 0, -0.1 }

    -- local text_world = create_world()
    -- supercamera_2d("zoom")
    -- __CurrentWorld = world

    camera.owner.position = vec3.new(0, 0, 0)

    world.root:draw_circle({ 0, 0 }, 0.05)
    Forever()
end)
