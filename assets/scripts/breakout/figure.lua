-- ブロックの大きさ
BlockHalfWidth = 0.01
BlockHalfHeight = 0.005

-- ボールの大きさ
BallRadius = 0.006

-- ステージの大きさ
StageHW = 0.1

-- バーの大きさ
BarHW = 0.02
BarHH = 0.004

-- 同時衝突の際、現フレームで削除されたブロックを記録していく
-- local ERASED_BLOCKS = {}


local figure = {}

---丸を生成
---@return MeshComponent
function figure.maru(x, y)
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
function figure.block(parent, x, y)
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
function figure.sen(p1, p2, on_collision_enter)
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
function figure.oresen(points, on_collision_enter)
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
function figure.wakka(points, on_collision_enter)
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

return figure
