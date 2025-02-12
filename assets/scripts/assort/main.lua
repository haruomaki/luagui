local breakout = require("breakout.main")
local Player = require("Player")

run_window(800, 600, "assort", function()
    local world = create_world()
    world.bullet_world.gravity = vec3 { 0, -16, 0 }
    local player = Player.spawn()
    -- local camera = CreateCamera("quit", "move", "zoom")
    -- camera.owner.parent.position = vec3 { 0, 1.5, 4 }

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
    text.color = RGBA { 0.8, 0.7, 0.3 }

    Thread.forever("toggle camera mode", function()
        -- 注目中オブジェクトの更新
        local new_focus = nil
        local results = player.head:raycast_front(5)
        if (#results > 0) then
            new_focus = results[1].hitObject.owner
        end
        block.refresh_focus(new_focus)

        -- デバッグ情報の更新
        text.message = block.focus and block.focus.id or ""
    end)

    Wait(2)

    print("動かします。")
    cube.position = vec3 { 0, 2, 0 }
    -- breakout.main()
    Forever()
end)
