local block = {}

---ブロックを置く対象となるメインのワールド
---@type World
block.main_world = __CurrentWorld

-- ブロックのマテリアルおよびメッシュを作成
local material = Material.new()
material.point_size = 20

local a = 0.5
local mesh = Mesh.new()
mesh.use_index = true
mesh.draw_mode = 'triangle_strip'
mesh.indices = VI { 0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5 }
mesh.coords = V3 { { -a, -a, -a }, { a, -a, -a }, { -a, a, -a }, { a, a, -a }, { -a, -a, a }, { a, -a, a }, { -a, a, a }, { a, a, a } }

local color_tmp = {}
for i = 0, 7 do
    local r = math.sin(0.4 * i + 1) ^ 2
    local g = math.sin(0.4 * i + 1.5) ^ 2
    local b = math.sin(0.4 * i + 1.8) ^ 2
    color_tmp[i] = { r, g, b, 1 }
end
mesh.colors = CV(color_tmp)


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

return block
