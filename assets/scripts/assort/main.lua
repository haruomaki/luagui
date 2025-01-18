run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    local grid = Util.grid()
    local cube = Util.cube()

    Thread.forever("toggle camera mode", function()
        if GetKeyDown('M') then camera:toggle_mode() end
    end)

    Wait(2)

    print("動かします。")
    cube.position = vec3 { 0, 2, 0 }
    Forever(function()
        local results = camera.owner:raycast_front()
        for _, r in ipairs(results) do
            print(r.hitObject.owner.id)
        end
    end)
end)
