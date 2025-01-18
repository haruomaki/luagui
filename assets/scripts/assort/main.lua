run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")

    local grid = Util.grid()
    local cube = Util.cube()

    Font.load("fonts/UDEVGothic.ttf")

    local ui = create_world()
    local ui_camera = CreateCamera2D()
    ui_camera.centering = 'TopLeft'
    local text = ui.root:child_text()
    text.owner.position = vec3 { 0.01, -0.02, 0 }

    Thread.forever("toggle camera mode", function()
        if GetKeyDown('M') then camera:toggle_mode() end

        local results = camera.owner:raycast_front()
        if (#results > 0) then
            text.message = results[1].hitObject.owner.id
        else
            text.message = ""
        end
    end)

    Wait(2)

    print("動かします。")
    cube.position = vec3 { 0, 2, 0 }
    Forever()
end)
