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
    obj.name = "カラフルキューブ"
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
    obj.name = "カラフルグリッド"
    obj:add_mesh_component(material, mesh)
    local rb = obj:add_rigidbody()
    rb:plane_shape(0, 1, 0, 0)
    rb.restitution = 0.5
    return obj
end

---二次元のメッシュを簡単に作成する。
---@param material Material
---@param coords V3
---@return WorldObject
function Util.mesh2d(material, coords)
    local mesh = Mesh.new()
    mesh.draw_mode = 'triangle_fan'
    mesh.coords = coords
    mesh.uvs = V2 { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } }

    local obj = __CurrentWorld.root:append_empty_child()
    obj.name = "四角"
    obj.position = vec3.new(0, 0, 0)
    obj:add_mesh_component(material, mesh)

    return obj
end

---矩形のメッシュを簡単に作成する。
---@param material Material 表示する物体のマテリアル
---@param width number 矩形の幅
---@param height number 矩形の高さ
---@param anchor? vec2 中心のオフセット。デフォルトはど真ん中。
---@return WorldObject
function Util.rect(material, width, height, anchor)
    anchor = anchor or Center
    return Util.mesh2d(material,
        V3 {
            { width * (anchor.x - 1) / 2, height * (anchor.y - 1) / 2 },
            { width * (anchor.x + 1) / 2, height * (anchor.y - 1) / 2 },
            { width * (anchor.x + 1) / 2, height * (anchor.y + 1) / 2 },
            { width * (anchor.x - 1) / 2, height * (anchor.y + 1) / 2 },
        })
end

---画像から矩形のメッシュを簡単に作成する。
---@param image Image 画像
---@param scale? number 拡大率
---@param anchor? vec2 中心のオフセット。デフォルトはど真ん中。
---@return WorldObject
function Util.rect_image(image, scale, anchor)
    scale = scale or 1
    anchor = anchor or Center
    local material = Material.from_image(image) -- TODO: マテリアルを使いまわす
    local width = image.width * px_meter * scale
    local height = image.height * px_meter * scale
    return Util.mesh2d(material,
        V3 {
            { width * (anchor.x - 1) / 2, height * (anchor.y - 1) / 2 },
            { width * (anchor.x + 1) / 2, height * (anchor.y - 1) / 2 },
            { width * (anchor.x + 1) / 2, height * (anchor.y + 1) / 2 },
            { width * (anchor.x - 1) / 2, height * (anchor.y + 1) / 2 },
        })
end

--- 16進数の色コードをRGB値に変換する関数
---@param hex string
---@return RGBA
function Hex(hex)
    hex = hex:gsub("#", "")
    local r = tonumber(hex:sub(1, 2), 16) / 255
    local g = tonumber(hex:sub(3, 4), 16) / 255
    local b = tonumber(hex:sub(5, 6), 16) / 255
    return RGBA { r, g, b, 1 }
end
