run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    local grid = Util.grid()
    local cube = Util.cube()

    Thread.forever("toggle camera mode", function()
        if GetKeyDown('Enter') then camera:toggle_mode() end
    end)

    Wait(2)

    print("動かします。")
    cube.position = vec3 { 0, 2, 0 }
    Forever(function()
        local results = world:raycast(vec3 { 0, 0.1, 5 }, vec3 { 0, 0, -10 })
        print(#results)
    end)
end)
