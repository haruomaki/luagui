---------------
-- 便利な関数
---------------

Util = {}

function Util.cube()
    local material = Material.new()

    local a = 0.5
    local mesh = Mesh.new()
    mesh.use_index = true
    mesh.indices = VI { 0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5 }
    mesh.coords = V3 { { 0, 0, 0 }, { a, 0, 0 }, { 0, a, 0 }, { a, a, 0 }, { 0, 0, a }, { a, 0, a }, { 0, a, a }, { a, a, a } }

    local color_tmp = {}
    for i = 0, 7 do
        local r = math.sin(0.4 * i + 1) ^ 2
        local g = math.sin(0.4 * i + 2) ^ 2
        local b = math.sin(0.4 * i + 3) ^ 2
        color_tmp[i] = { r, g, b, 1 }
    end
    mesh.colors = CV(color_tmp)

    local obj = __CurrentWorld.root:append_empty_child()
    obj.rotation = quat.angle_z(math.pi / 6) * quat.angle_x(math.pi / 6)
    obj:add_mesh_component(material, mesh)
end
