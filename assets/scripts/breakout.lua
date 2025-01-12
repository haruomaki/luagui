print(#package.searchers)
require("coroutines")

print(_CWD)

-- ブロックの大きさ
BlockHalfWidth = 0.01
BlockHalfHeight = 0.005

-- ボールの大きさ
BallRadius = 0.006

-- ステージの大きさ
StageHW = 0.1

-- バーの長さ
BarHW = 0.02

-- 同時衝突の際、現フレームで削除されたブロックを記録していく
-- local ERASED_BLOCKS = {}


---丸を生成
---@return MeshComponent
local function maru(x, y)
    local obj = __CurrentWorld.root:append_empty_child()
    obj.position = { x, y }
    local mesh = obj:add_mesh_component(BallMaterial, BallMesh)
    local rb = obj:add_rigidbody_component({ type = "dynamic", isBullet = true })
    rb:add_shape({ shape = "circle", radius = BallRadius, friction = 0, restitution = 1 })
    return mesh
end

---ブロックを生成
---@param parent WorldObject
---@return MeshComponent
local function block(parent, x, y)
    local block_obj = parent:append_empty_child()
    local mesh = block_obj:add_mesh_component(BlockMaterial, BlockMesh);
    local obj = mesh.owner
    obj.position = { x, y }
    local rb = obj:add_rigidbody_component()
    rb:add_shape({
        shape = "rect",
        friction = 0,
        restitution = 1,
        halfWidth = BlockHalfWidth,
        halfHeight = BlockHalfHeight,
        on_collision_enter = function(self, other)
            -- 削除済みチェック。複数のボールが同時にぶつかったときに二重削除を防止する。
            if not ERASED_BLOCKS[self.owner.id] then
                -- 音を再生して消滅する
                ERASED_BLOCKS[self.owner.id] = true
                self.owner:erase()
                Score = Score + 1
                play_music(BlockSound)
            end
        end
    })
    return mesh
end

---直線を生成
---@param p1 Point
---@param p2 Point
---@param on_collision_enter? fun(self: Collider, other: Collider)
---@return MeshComponent
local function sen(p1, p2, on_collision_enter)
    local mesh = __CurrentWorld.root:draw_line({ p1, p2 })
    local rb = mesh.owner:add_rigidbody_component()
    rb:add_shape({
        shape = "edge",
        points = { p1, p2 },
        on_collision_enter = on_collision_enter
    })
    return mesh
end

---折れ線を生成
---@param points Points
---@param on_collision_enter? fun(self: ChainCollider, collider: Collider)
local function oresen(points, on_collision_enter)
    local mesh = __CurrentWorld.root:draw_line(slice(points, 2, 4))
    local rb = mesh.owner:add_rigidbody_component()
    rb:add_chain({
        points = points,
        on_collision_enter = on_collision_enter
    })
end

---閉じた折れ線を生成
---@param points Points
---@param on_collision_enter? fun(self: ChainCollider, collider: Collider)
local function wakka(points, on_collision_enter)
    local mesh = __CurrentWorld.root:draw_line(points, true)
    local obj = mesh.owner
    local rb = obj:add_rigidbody_component()
    rb:add_chain({
        points = points,
        restitution = 1,
        isLoop = true,
        on_collision_enter = on_collision_enter
    })
end

run_window(800, 600, "ブロック崩し", function()
    b2SetLengthUnitsPerMeter(0.001)

    local world = create_world()
    local camera = supercamera_2d("quit", "zoom")
    camera.owner.position = { 0, 0.05 }
    camera.owner.scale_prop = 2
    world.b2world.gravity = { 0, -0.1 }

    local text_world = create_world()
    supercamera_2d("zoom")
    __CurrentWorld = world

    -- ブロックにボールがぶつかったときの音
    BlockSound = load_music("audio/泡がはじける.mp3")
    -- ブロックの画像
    BlockImage = load_image("images/ピンクレンガ.png")
    -- ブロックのマテリアルとメッシュ
    BlockMaterial = new_material(BlockImage)
    BlockMesh = new_mesh(
        { { -BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, BlockHalfHeight, 0 }, { -BlockHalfWidth, BlockHalfHeight, 0 } },
        { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })

    BallImage = load_image("images/青いガラス玉.png")
    BallMaterial = new_material(BallImage)
    BallMaterial.write_depth = false
    BallMesh = new_mesh(
        { { -BallRadius, -BallRadius, 0 }, { BallRadius, -BallRadius, 0 }, { BallRadius, BallRadius, 0 }, { -BallRadius, BallRadius, 0 } },
        { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })


    text_world.root:child_text("SCORE", { position = { 0.06, 0.02 }, scale = 0.5 })
    local score_text = text_world.root:child_text("mochi-mochi panic", { position = { 0.06, 0 } })

    -- ゲームのスコア
    Score = 0

    -- 床と壁の剛体を作成
    wakka({ { -StageHW, -0.1 }, { -StageHW, 0.2 }, { StageHW, 0.2 }, { StageHW, -0.1 } })

    -- 落下判定を作成
    sen({ -0.5, -0.07 }, { 0.5, -0.07 }, function(self, other)
        -- printf("衝突しました！ %d,%d (%p)", self.index, other.index, other.owner)
        other.owner:erase()
    end)

    -- 操作バーを追加
    local bar_obj = world.root:draw_rect(BarHW, 0.003).owner
    bar_obj.position = { 0, -0.05 }
    local bar = bar_obj:add_rigidbody_component({ type = "kinematic" })
    bar:add_shape({ shape = "rect", friction = 1, restitution = 1, halfWidth = 0.02, halfHeight = 0.003 })

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
                local m = maru(bar.position.x, -0.04).owner:get_component("Rigidbody")
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
                local mesh = block(block_container, BlockHalfWidth * 2 * i, BlockHalfHeight * 2 * j)
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
