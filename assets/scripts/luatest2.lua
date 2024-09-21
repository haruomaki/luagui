---丸を生成
---@return MeshObject
local function maru(x, y)
    local obj = CurrentWorld:draw_circle({ x, y }, 0.01)
    local rb = obj:add_rigidbody_component()
    rb:add_shape({ shape = "circle", radius = 0.01 })
    return obj
end

---直線を生成
---@param p1 Point
---@param p2 Point
local function sen(p1, p2)
    local obj = CurrentWorld:draw_line({ p1, p2 })
    local rb = obj:add_rigidbody_component()
    rb:add_shape({ shape = "edge", points = { p1, p2 } })
end

run_window(800, 600, "Test Window", function()
    local world = create_world()
    world.b2world.gravity = { 0, -0.2 }
    print(world.b2world.gravity)
    CurrentWorld = world

    -- 床と壁の剛体を作成
    sen({ -0.1, 0 }, { 0, -0.02 })
    sen({ 0.1, 0 }, { 0, -0.02 })

    -- 落下判定を作成
    sen({ -0.5, -0.04 }, { 0.5, -0.04 })

    Forever(function()
        if GetKeyDown('Space') then
            maru(0, 0.02)
        end
    end)
end)
