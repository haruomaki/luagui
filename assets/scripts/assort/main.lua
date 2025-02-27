local breakout = require("breakout.main")
local Player = require("Player")

run_window(800, 600, "assort", function()
    Mouse.disable()

    local world = create_world()
    world.bullet_world.gravity = vec3 { 0, -18, 0 }
    local player = Player.spawn()
    -- local camera = CreateCamera("quit", "move", "zoom")
    -- camera.owner.parent.position = vec3 { 0, 1.5, 4 }

    -- キューブを生成
    local cube = Util.cube()
    cube:add_update_component("落下したキューブのリスポーン", ForeverFun(function()
        if cube.position.y < -10 then
            cube.position = vec3 { 0, 3, 0 }
            cube:get_component("Rigidbody").linear_velocity = vec3 { 0, -10, 0 }
        end
    end))

    local block = require("block")

    -- 地面となるブロックを配置
    for i = -10, 10 do
        for j = -30, 30 do
            block.place(vec3 { i, -1, j })
        end
    end
    block.place(vec3 { -1, 0, -1 })
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
        block.refresh_focus(player.head)

        -- デバッグ情報の更新
        text.message = block.focus and block.focus.id or ""

        -- ブロックの設置/破壊
        if Mouse.left and world.root:find_component_by_name("左クリック長押し") == nil then
            world.root:add_update_component("左クリック長押し", function()
                local break_lasttime = 0
                while not Mouse.LeftUp() and block.focus do
                    if get_time() - break_lasttime > 0.25 then
                        break_lasttime = get_time()
                        print("破壊！")
                        block.erase_focusing()
                        block.play_break_sound()
                    end
                    Yield()
                end
            end)
        end
        if Mouse.right and world.root:find_component_by_name("右クリック長押し") == nil then
            world.root:add_update_component("右クリック長押し", function()
                local place_lasttime = 0
                while not Mouse.RightUp() and block.focus do
                    if get_time() - place_lasttime > 0.2 then
                        place_lasttime = get_time()
                        local p = block.focus.position + Direction[block.focus_surface] - vec3 { 0.5, 0.5, 0.5 }
                        print("設置！", p)
                        block.place(p)
                        block.play_put_sound()
                    end
                    Yield()
                end
            end)
        end
    end)

    Forever()
end)
