local figure = require("figure")

run_window(800, 600, "ブロック崩し", function()
    b2SetLengthUnitsPerMeter(0.001)

    local world = create_world()
    local camera = CreateCamera2D("quit", "zoom")
    camera.owner.position = vec3.new(0, 0.05, 0)
    camera.owner.scale_prop = 2
    world.b2world.gravity = { 0, -0.1 }

    -- 音声や画像の読み込み
    require("resource")

    world.root:child_text("SCORE", { position = { 0.15, 0.08 } })
    local score_text = world.root:child_text("mochi-mochi panic", { position = { 0.15, 0.05 } })
    score_text.font_size = 80
    score_text.color = RGBA { 0.3, 0.7, 0.4, 1 }

    -- ゲームのスコア
    Score = 0

    -- 床と壁の剛体を作成
    figure.wakka({ { -StageHW, -0.1 }, { -StageHW, 0.2 }, { StageHW, 0.2 }, { StageHW, -0.1 } })

    -- 落下判定を作成
    figure.sen({ -0.5, -0.07 }, { 0.5, -0.07 }, function(self, other)
        -- printf("衝突しました！ %d,%d (%p)", self.index, other.index, other.owner)
        other.owner:erase()
    end)

    -- 操作バーを追加
    local bar_obj = world.root:append_empty_child()
    bar_obj.position = vec3.new(0, -0.05, 0)
    bar_obj:add_mesh_component(BarMaterial, BarMesh)
    local bar = bar_obj:add_rigidbody2d_component({ type = "kinematic" })
    bar:add_shape({ shape = "rect", friction = 1, restitution = 1, halfWidth = BarHW, halfHeight = BarHH })

    bar_obj:add_update_component("バーのキー操作", ForeverFun(function()
        -- バーをキー操作で動かす
        local bar_x = bar.position.x
        local dt = 1 / Screen.refreshRate
        if GetKey('Right') then bar_x = bar_x + 0.2 * dt end
        if GetKey('Left') then bar_x = bar_x - 0.2 * dt end

        local clamped = math.clamp(bar_x, -StageHW + BarHW, StageHW - BarHW)
        bar.position = { clamped, -0.05 }

        -- スペースキーで連射
        if GetKeyDown('Space') then
            bar_obj:add_update_component("ショット", IntervalFun(function()
                local m = figure.maru(bar.position.x, -0.04).owner:get_component("Rigidbody2D")
                local theta = (math.random() - 0.5) * 0.2
                local speed = math.random() * 0.1 + 0.25
                m.linear_velocity = { speed * math.sin(theta), speed * math.cos(theta) }
            end, 0.06))
        end

        if GetKeyUp('Space') then
            bar_obj:get_component_by_id("ショット"):erase()
        end

        -- 削除済みチェック用リストを毎フレーム初期化する
        ERASED_BLOCKS = {}

        -- テキストを毎フレーム更新
        score_text.message = string.format("%d", Score)
    end))

    -- フリッパーを作成
    -- local flipper_left_obj = sen({ 0, 0 }, { 0.04, 0.04 })
    -- local flipper_left_rb = flipper_left_obj:get_component("Rigidbody")
    -- flipper_left_rb.angular_velocity = 4

    -- local flipper_right_obj = sen({ 0, 0 }, { -0.04, 0.04 })
    -- local flipper_right_rb = flipper_right_obj:get_component("Rigidbody")
    -- flipper_right_rb.angular_velocity = -4


    while true do
        -- ブロックを配置
        local block_container = world.root:append_empty_child()
        for i = -4, 4, 1 do
            for j = 5, 18, 1 do
                local mesh = figure.block(block_container, BlockHalfWidth * 2 * i, BlockHalfHeight * 2 * j)
                mesh.owner.id = string.format("(%d,%d)のブロック", i, j)
                -- print(mesh.owner.id, mesh.owner)
            end
        end

        WaitUntil(function()
            return #block_container:children() == 0
        end)

        print("クリアー！すべてのブロックを消しました。")
        Wait(2)
    end
end)
