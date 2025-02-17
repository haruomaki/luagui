local block = {}

---ブロックを置く対象となるメインのワールド
---@type World
block.main_world = __CurrentWorld

---現在注目中のブロック
---@type WorldObject | nil
block.focus = nil

-- ブロックのマテリアルおよびメッシュを作成
local wood_image = Image.load("images/白い木の板.jpg")
local material = Material.from_image(wood_image)
material.point_size = 20

local a = 0.5
local mesh = Mesh.new()
mesh.draw_mode = 'triangles'
local p1 = { -a, -a, a }
local p2 = { a, -a, a }
local p3 = { -a, -a, -a }
local p4 = { a, -a, -a }
local p5 = { -a, a, a }
local p6 = { a, a, a }
local p7 = { -a, a, -a }
local p8 = { a, a, -a }
mesh.coords = V3 { p3, p4, p1, p1, p4, p2, p1, p2, p5, p5, p2, p6, p2, p4, p6, p6, p4, p8, p4, p3, p8, p8, p3, p7, p3, p1, p7, p7, p1, p5, p5, p6, p7, p7, p6, p8 }
mesh.uvs = V2 { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } * 6


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


---指定の場所にブロックを置く
---@param pos vec3
function block.place(pos)
    local obj = block.main_world.root:append_empty_child()
    obj.id = "ブロック"
    obj.position = pos + vec3 { 0.5, 0.5, 0.5 }
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

---@param new_focusing WorldObject | nil
function block.refresh_focus(new_focusing)
    if new_focusing ~= block.focus then
        if block.focus ~= nil then remove_frame(block.focus) end
        if new_focusing ~= nil then add_frame(new_focusing) end
        block.focus = new_focusing
    end
end

return block
