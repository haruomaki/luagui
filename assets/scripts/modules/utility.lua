---------------
-- 便利な関数
---------------

Util = {}

---@return WorldObject
function Util.cube()
    local material = Material.new()
    material.point_size = 20
    material.both = true

    local a = 0.3
    local mesh = Mesh.new()
    mesh.use_index = true
    mesh.draw_mode = 'triangle_strip'
    mesh.indices = VI { 0, 2, 1, 3, 7, 2, 6, 0, 4, 1, 5, 7, 4, 6 }
    mesh.coords = V3 { { -a, -a, -a }, { a, -a, -a }, { -a, a, -a }, { a, a, -a }, { -a, -a, a }, { a, -a, a }, { -a, a, a }, { a, a, a } }

    local color_tmp = {}
    for i = 0, 7 do
        local r = math.sin(0.4 * i + 1) ^ 2
        local g = math.sin(0.4 * i + 2) ^ 2
        local b = math.sin(0.4 * i + 3) ^ 2
        color_tmp[i] = { r, g, b, 1 }
    end
    mesh.colors = CV(color_tmp)

    local obj = __CurrentWorld.root:append_empty_child()
    obj.id = "カラフルキューブ"
    obj.position = vec3.new(0, 2, 0)
    obj.rotation = quat.angle_z(math.pi / 6) * quat.angle_y(math.pi / 6) * quat.angle_x(math.pi / 6)
    obj:add_mesh_component(material, mesh)

    local rb = obj:add_rigidbody()
    rb:box_shape(a, a, a)
    rb.mass = 1
    rb.restitution = 0.8

    return obj
end

---@return WorldObject
function Util.grid()
    local material = Material.new()
    material.line_width = 1

    local mesh = Mesh.new()
    mesh.draw_mode = 'lines'

    -- 座標
    local coords_tmp = {}
    for i = -10, 10 do
        coords_tmp[4 * i + 40] = { i, 0, -10 }
        coords_tmp[4 * i + 41] = { i, 0, 10 }
        coords_tmp[4 * i + 42] = { -10, 0, i }
        coords_tmp[4 * i + 43] = { 10, 0, i }
    end
    mesh.coords = V3(coords_tmp)

    -- カラフルな色
    local color_tmp = {}
    for i = 0, 83 do
        local r = math.sin(0.4 * i + 1) ^ 2
        local g = math.sin(0.4 * i + 2) ^ 2
        local b = math.sin(0.4 * i + 3) ^ 2
        color_tmp[i] = { r, g, b, 1 }
    end
    mesh.colors = CV(color_tmp)

    local obj = __CurrentWorld.root:append_empty_child()
    obj.id = "カラフルグリッド"
    obj:add_mesh_component(material, mesh)
    local rb = obj:add_rigidbody()
    rb:plane_shape(0, 1, 0, 0)
    rb.restitution = 0.5
    return obj
end
