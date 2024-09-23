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

run_window(800, 600, "Test Window", function()
    b2SetLengthUnitsPerMeter(0.01)
    local world = create_world()
    world.b2world.gravity = { 0, -0.2 }
    print(world.b2world.gravity)

    shikaku(0.003, 0.01)

    -- 床と壁の剛体を作成
    sen({ -0.1, 0 }, { 0, -0.02 })
    sen({ 0.1, 0 }, { 0, -0.02 })

    -- 落下判定を作成
    sen({ -0.5, -0.04 }, { 0.5, -0.04 }, function(self, other)
        printf("衝突しました！ %d,%d", self.index, other.index)
        other.owner:erase()
    end)

    Forever(function()
        if GetKeyDown('Space') then
            maru(0, 0.02)
        end
    end)
end)
