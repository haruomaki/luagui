---------------
-- 便利な関数
---------------

Util = {}

function Util.cube()
    local img = Image.load("images/青いガラス玉.png")
    local material = Material.from_image(img)

    local a = 0.1
    local mesh = Mesh.new()
    mesh.coords = V3 { { 0, 0, 0 }, { a, 0, 0 }, { 0, a, 0 }, { a, a, 0 }, { 0, 0, a }, { a, 0, a }, { 0, a, a }, { a, a, a } }
    mesh.uvs = V2 { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 } }

    local obj = __CurrentWorld.root:append_empty_child()
    obj:add_mesh_component(material, mesh)
end
