---------------
-- 便利な関数
---------------

Util = {}

---@return WorldObject
function Util.cube()
    local material = Material.new()
    material.point_size = 20

    local a = 0.3
    local mesh = Mesh.new()
    mesh.use_index = true
    mesh.draw_mode = 'triangle_strip'
    mesh.indices = VI { 0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5 }
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
    obj.position = vec3.new(0, 2, 0)
    obj.rotation = quat.angle_z(math.pi / 6) * quat.angle_y(math.pi / 6) * quat.angle_x(math.pi / 6)
    obj:add_mesh_component(material, mesh)

    local rb = obj:add_rigidbody()
    rb:box_shape(a, a, a)
    rb.mass = 1
    rb.restitution = 0.8

    return obj
end
