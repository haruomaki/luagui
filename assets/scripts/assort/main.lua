run_window(800, 600, "assort", function()
    local world = create_world()
    local camera = CreateCamera("quit", "move", "zoom")
    camera.owner.parent.position = vec3 { 0, 1.5, 4 }

    -- local grid = Util.grid()
    local cube = Util.cube()

    local block = require("block")

    -- 地面となるブロックを配置
    for i = -10, 10 do
        for j = -10, 10 do
            block.place(vec3 { i, -1, j })
        end
    end
    -- local b1 = block.place(vec3 { 0, -1, 0 })
    -- local b2 = block.place(vec3 { -1, -1, 0 })
    -- block.place(vec3 { -1, -1, -1 })
    -- block.place(vec3 { -1, -1, -1 })

    Font.load("fonts/UDEVGothic.ttf")
    -- Font.load("fonts/main.ttf")

    local ui = create_world()
    local ui_camera = CreateCamera2D()
    ui_camera.centering = TopLeft
    local text = ui.root:child_text()
    text.owner.position = vec3 { 0.002, -0.002, 0 }
    text.font_size = 24
    text.anchor = BottomRight

    Thread.forever("toggle camera mode", function()
        if GetKeyDown('M') then camera:toggle_mode() end

        local results = camera.owner:raycast_front()
        if (#results > 0) then
            text.message = results[1].hitObject.owner.id
        else
            text.message = "鯖 <-> 鯖󠄀"
            -- text.message = "ABCDEFGHIJK"
        end
    end)

    Wait(2)

    print("動かします。")
    cube.position = vec3 { 0, 2, 0 }
    Forever()
end)
