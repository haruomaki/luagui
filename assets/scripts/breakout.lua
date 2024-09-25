-- ブロックの大きさ
BlockHalfWidth = 0.01
BlockHalfHeight = 0.005

---丸を生成
---@return MeshObject
local function maru(x, y)
    local obj = __CurrentWorld:draw_circle({ x, y }, 0.01)
    local rb = obj:add_rigidbody_component()
    rb:add_shape({ shape = "circle", radius = 0.01, restitution = 0.9 })
    return obj
end

---四角を生成
---@return MeshObject
local function shikaku(hx, hy)
    local obj = __CurrentWorld:draw_rect(hx, hy)
    local rb = obj:add_rigidbody_component()
    rb:add_shape({ shape = "rect", halfWidth = hx, halfHeight = hy })
    return obj
end

---ブロックを生成
---@return MeshObject
local function block(x, y)
    local obj = __CurrentWorld:draw_rect(BlockHalfWidth, BlockHalfHeight)
    obj.position = { x, y }
    local rb = obj:add_rigidbody_component({ type = "dynamic" })
    rb:add_shape({
        shape = "rect",
        halfWidth = BlockHalfWidth,
        halfHeight = BlockHalfHeight,
        on_collision_enter = function(self, other)
            print("ブロックにぶつかりました", other.index)
        end
    })
    return obj
end

---直線を生成
---@param p1 Point
---@param p2 Point
---@param on_collision_enter? fun(self: Collider, other: Collider)
local function sen(p1, p2, on_collision_enter)
    local obj = __CurrentWorld:draw_line({ p1, p2 })
    local rb = obj:add_rigidbody_component()
    rb:add_shape({
        shape = "edge",
        points = { p1, p2 },
        on_collision_enter = on_collision_enter
    })
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
        isLoop = true,
        on_collision_enter = on_collision_enter
    })
end

run_window(800, 600, "Test Window", function()
    b2SetLengthUnitsPerMeter(0.01)
    local world = create_world()
    world.b2world.gravity = { 0, -0.2 }
    print(world.b2world.gravity)

    local b = block(0.003, 0.01)
    -- b:get_component("Rigidbody"):add_chain({
    --     points = { { 0.03, 0.03 }, { 0.02, 0.01 }, { 0.01, 0 }, { 0, 0 }, { 0, 0.01 } },
    --     on_collision_enter = function(self, collider)
    --         print(self, collider)
    --     end
    -- })

    -- 床と壁の剛体を作成
    wakka({ { -0.1, 0 }, { -0.1, 0.2 }, { 0.1, 0.2 }, { 0.1, 0 } })

    -- 落下判定を作成
    sen({ -0.5, -0.04 }, { 0.5, -0.04 }, function(self, other)
        printf("衝突しました！ %d,%d", self.index, other.index)
        other.owner:erase()
    end)

    -- Wait(1)
    -- b:erase()

    Forever(function()
        if GetKeyDown('Space') then
            maru(0, 0.02)
        end
    end)
end)
