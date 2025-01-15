---@meta mesh

---描画設定。
---@class Material
---@field write_depth boolean デプスバッファに書き込むかどうか
Material = {}

---物体形状。
---@class Mesh
Mesh = {}

---@class MeshComponent : Component
MeshComponent = {}

---マテリアルを作成する。
---@param image Image テクスチャ画像
---@return Material
function Material.from_image(image) end

---空のメッシュを作成する。
---@return Mesh
function Mesh.new() end

---メッシュを作成する。
---@return Mesh
---@param coords Points
---@param uvs Points
function Mesh.new(coords, uvs) end

---姿かたちを描画するコンポーネントを付与。
---@param material Material
---@param mesh Mesh
---@return MeshComponent
function WorldObject:add_mesh_component(material, mesh) end
