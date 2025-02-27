local block = {}

---ブロックを置く対象となるメインのワールド
---@type World
block.main_world = __CurrentWorld

---現在注目中のブロック
---@type WorldObject | nil
block.focus = nil

---現在のレイキャスト結果
---@type RaycastHit | nil
block.raycast_result = nil

---現在注目中のブロックの面方向
---@type string
block.focus_surface = ""

-- ブロックのマテリアルおよびメッシュを作成
-- local wood_image = Image.load("images/白い木の板.jpg")
-- local material = Material.from_image(wood_image)
local material = Material.load("models/ブロック.glb")
material.point_size = 20

local a = 0.5
local mesh = Mesh.load("models/ブロック.glb")
-- mesh.draw_mode = 'triangles'
-- local p1 = { -a, -a, a }
-- local p2 = { a, -a, a }
-- local p3 = { -a, -a, -a }
-- local p4 = { a, -a, -a }
-- local p5 = { -a, a, a }
-- local p6 = { a, a, a }
-- local p7 = { -a, a, -a }
-- local p8 = { a, a, -a }
-- mesh.coords = V3 { p3, p4, p1, p1, p4, p2, p1, p2, p5, p5, p2, p6, p2, p4, p6, p6, p4, p8, p4, p3, p8, p8, p3, p7, p3, p1, p7, p7, p1, p5, p5, p6, p7, p7, p6, p8 }
-- mesh.uvs = V2 { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } * 6
-- print(mesh.coords)
-- print(mesh.uvs)
-- print(mesh.indices)


-- カーソルを当てたときのフレームのメッシュ
local frame_material = Material.new()
frame_material.line_width = 3
local frame_mesh = Mesh.new()
frame_mesh.use_index = true
frame_mesh.draw_mode = 'lines'
frame_mesh.indices = VI { 0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 1, 3, 4, 6, 5, 7, 0, 4, 1, 5, 2, 6, 3, 7 }
local b = a + 0.002
frame_mesh.coords = V3 { { -b, -b, -b }, { b, -b, -b }, { -b, b, -b }, { b, b, -b }, { -b, -b, b }, { b, -b, b }, { -b, b, b }, { b, b, b } }
frame_mesh.colors = RGBA { 0.9, 0.9, 0.9 } * 24


-- 設置するときの効果音
local put_sound = Music.load("audio/茶碗を置く.mp3")
function block.play_put_sound()
    play_music("効果音1", put_sound)
end

-- 破壊するときの効果音
local break_sound = Music.load("audio/鍵を置く.mp3")
function block.play_break_sound()
    play_music("効果音1", break_sound)
end

---指定の場所にブロックを置く
---@param pos vec3
function block.place(pos)
    local x = math.floor(pos.x + 0.5)
    local y = math.floor(pos.y + 0.5)
    local z = math.floor(pos.z + 0.5)

    local obj = block.main_world.root:append_empty_child()
    obj.name = "ブロック"
    obj.position = vec3 { x, y, z } + vec3 { 0.5, 0.5, 0.5 }
    obj:add_mesh_component(material, mesh)

    local rb = obj:add_rigidbody()
    rb:box_shape(a, a, a)
    rb.mass = 0
    rb.restitution = 0.8

    return obj
end

---@param obj WorldObject
local function add_frame(obj)
    local m = obj:add_mesh_component(frame_material, frame_mesh)
    m.name = "注目中ブロックの枠線"
end

---@param obj WorldObject
local function remove_frame(obj)
    obj:get_component_by_name("注目中ブロックの枠線"):erase()
end

function block.erase_focusing()
    if block.focus ~= nil and block.focus.name == "ブロック" then
        -- print("破壊！")
        remove_frame(block.focus)
        block.focus:erase()
        block.focus = nil
        block.play_break_sound()
    end
end

---レイキャストを行い、注目中のブロックを更新する
---@param player_head WorldObject
function block.refresh_focus(player_head)
    local new_focusing = nil
    block.focus_surface = ""
    local results = player_head:raycast_front(5)
    block.raycast_result = results[1] -- 先頭の物体にレイが当たったら保存。当たらなかったらnil。
    if (block.raycast_result ~= nil) then
        new_focusing = block.raycast_result.hitObject.owner

        -- 面の判定（法線の成分で判断）
        local normal = block.raycast_result.hitNormal
        for key, value in pairs(Direction) do
            if normal:dot(value) > 0.7 then
                block.focus_surface = key
            end
        end
    end

    if new_focusing ~= block.focus then
        if block.focus ~= nil then remove_frame(block.focus) end
        if new_focusing ~= nil then add_frame(new_focusing) end
        block.focus = new_focusing
    end
end

return block
