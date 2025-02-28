local breakout = require("breakout.main")
local Player = require("Player")

local bgm1 = Music.load("audio/BGM/雨のち小夜時雨.mp3")

run_window(800, 600, "assort", function()
    -- 背景色。
    Window.background_color = Hex("#86aae3")

    -- マウスカーソルを非表示にし、無制限に移動。
    Mouse.disable()

    local world = create_world()
    world.bullet_world.gravity = vec3 { 0, -18, 0 }
    local player = Player.spawn()
    -- local camera = CreateCamera("quit", "move", "zoom")
    -- camera.owner.parent.position = vec3 { 0, 1.5, 4 }

    -- BGMを流す
    Music.set_group_volume("BGM", 0.25)
    Thread.forever("BGMを流すスレッド", function()
        play_music_blocking("BGM", bgm1)
    end)

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
    text.color = Hex("#324537")

    -- クロスヘア
    local cross_hair = create_world()
    local cross_hair_camera = CreateCamera2D()
    local glass_image = Image.load("images/クロスヘア.png")
    local glass_mat = Material.from_image(glass_image)
    -- Util.rect(glass_mat, V3 { { 0, 0, 0 }, { 0.05, 0, 0 }, { 0.05, 0.05, 0 }, { 0, 0.05, 0 } })
    Util.rect(glass_mat, 0.006, 0.006)


    -- ジャンプアイコン
    create_world()
    local jmp_camera = CreateCamera2D()
    jmp_camera.centering = BottomLeft
    local jmp_image = Image.load("images/ジャンプアイコン.png")
    local jmps = {}
    for i = 1, Player.MAX_AIR_JUMP do
        jmps[i] = Util.rect_image(jmp_image, 0.02, TopRight)
    end



    Thread.forever("toggle camera mode", function()
        -- 注目中オブジェクトの更新
        block.refresh_focus(player.head)

        -- デバッグ情報の更新
        text.message = block.focus and block.focus.name or ""

        -- ブロックの設置/破壊
        if Mouse.left and world.root:find_component_by_name("左クリック長押し") == nil then
            world.root:add_update_component("左クリック長押し", function()
                local break_lasttime = 0
                while not Mouse.LeftUp() and block.focus do
                    if get_time() - break_lasttime > 0.25 then
                        break_lasttime = get_time()
                        block.erase_focusing()
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

        -- ジャンプアイコンの表示/非表示
        for i = 1, Player.MAX_AIR_JUMP do
            if i <= Player.air_jump then
                jmps[i].position = vec3 { (i - 1) * 0.015 + 0.002, 0.002, 0 }
            else
                jmps[i].position = vec3 { 0, -100, 0 }
            end
        end
    end)

    Forever()
end)
