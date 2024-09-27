-- ブロックの大きさ
BlockHalfWidth = 0.01
BlockHalfHeight = 0.005

-- ステージの大きさ
StageHW = 0.1

-- バーの長さ
BarHW = 0.02

---丸を生成
---@return MeshObject
local function maru(x, y)
    local obj = __CurrentWorld:draw_circle({ x, y }, 0.003)
    local rb = obj:add_rigidbody_component({ type = "dynamic", isBullet = true })
    rb:add_shape({ shape = "circle", radius = 0.003, friction = 0, restitution = 1 })
    return obj
end

---ブロックを生成
---@return MeshObject
local function block(x, y)
    local obj = __CurrentWorld:draw_rect(BlockHalfWidth, BlockHalfHeight)
    obj.position = { x, y }
    local rb = obj:add_rigidbody_component()
    rb:add_shape({
        shape = "rect",
        friction = 0,
        restitution = 1,
        halfWidth = BlockHalfWidth,
        halfHeight = BlockHalfHeight,
        on_collision_enter = function(self, other)
            print("ブロックにぶつかりました", other.index)
            self.owner:erase()
        end
    })
    return obj
end

---直線を生成
---@param p1 Point
---@param p2 Point
---@param on_collision_enter? fun(self: Collider, other: Collider)
---@return MeshObject
local function sen(p1, p2, on_collision_enter)
    local obj = __CurrentWorld:draw_line({ p1, p2 })
    local rb = obj:add_rigidbody_component()
    rb:add_shape({
        shape = "edge",
        points = { p1, p2 },
        on_collision_enter = on_collision_enter
    })
    return obj
end

---折れ線を生成
---@param points Points
---@param on_collision_enter? fun(self: ChainCollider, collider: Collider)
local function oresen(points, on_collision_enter)
    local obj = __CurrentWorld:draw_line(slice(points, 2, 4))
    local rb = obj:add_rigidbody_component()
    rb:add_chain({
        points = points,
        on_collision_enter = on_collision_enter
    })
end

---閉じた折れ線を生成
---@param points Points
---@param on_collision_enter? fun(self: ChainCollider, collider: Collider)
local function wakka(points, on_collision_enter)
    local obj = __CurrentWorld:draw_line(points, true)
    local rb = obj:add_rigidbody_component()
    rb:add_chain({
        points = points,
        restitution = 1,
        isLoop = true,
        on_collision_enter = on_collision_enter
    })
end

run_window(800, 600, "Test Window", function()
    b2SetLengthUnitsPerMeter(0.001)
    local world = create_world()
    local camera = supercamera_2d("quit", "zoom")
    camera.position = { 0, 0.05 }
    camera.scale_prop = 3.5
    world.b2world.gravity = { 0, -0.1 }
    print(world.b2world.gravity)

    -- ブロックを配置
    for i = 0, 5, 1 do
        for j = 0, 9, 1 do
            block(-0.03 + 0.02 * i, 0.08 + 0.01 * j)
        end
    end

    -- 床と壁の剛体を作成
    wakka({ { -StageHW, -0.1 }, { -StageHW, 0.2 }, { StageHW, 0.2 }, { StageHW, -0.1 } })

    -- 落下判定を作成
    sen({ -0.5, -0.07 }, { 0.5, -0.07 }, function(self, other)
        printf("衝突しました！ %d,%d", self.index, other.index)
        other.owner:erase()
    end)

    -- 操作バーを追加
    local bar_obj = world:draw_rect(BarHW, 0.003)
    bar_obj.position = { 0, -0.05 }
    local bar = bar_obj:add_rigidbody_component({ type = "kinematic" })
    bar:add_shape({ shape = "rect", friction = 1, restitution = 1, halfWidth = 0.02, halfHeight = 0.003 })
    bar_obj:add_update_component(function()
        Forever(function()
            local bar_x = bar.position.x
            local dt = 1 / 60 -- TODO: フレームレートを取得
            if GetKey('Right') then bar_x = bar_x + 0.2 * dt end
            if GetKey('Left') then bar_x = bar_x - 0.2 * dt end

            local clamped = math.clamp(bar_x, -StageHW + BarHW, StageHW - BarHW)
            bar.position = { clamped, -0.05 }
        end)
    end)

    -- フリッパーを作成
    -- local flipper_left_obj = sen({ 0, 0 }, { 0.04, 0.04 })
    -- local flipper_left_rb = flipper_left_obj:get_component("Rigidbody")
    -- flipper_left_rb.angular_velocity = 4

    -- local flipper_right_obj = sen({ 0, 0 }, { -0.04, 0.04 })
    -- local flipper_right_rb = flipper_right_obj:get_component("Rigidbody")
    -- flipper_right_rb.angular_velocity = -4

    local cooldown = 0
    Forever(function()
        if GetKey('Space') and cooldown == 0 then
            print(bar.position.x - BarHW, bar.position.x + BarHW)
            cooldown = 3
            local bar_x = bar.position.x
            local m = maru(bar_x, -0.04):get_component("Rigidbody")
            local theta = (math.random() - 0.5) * 0.2
            local speed = math.random() * 0.1 + 0.25
            m.linear_velocity = { speed * math.sin(theta), speed * math.cos(theta) }
        end
        if cooldown > 0 then
            cooldown = cooldown - 1
        end
    end)
end)
