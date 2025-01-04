---@meta mesh

---描画設定。
---@class Material
Material = {}

---物体形状。
---@class Mesh
Mesh = {}

---@class MeshComponent : Component
MeshComponent = {}

---マテリアルを作成する。
---@param image Image テクスチャ画像
---@return Material
function new_material(image) end

---マテリアルを作成する。
---@return Mesh
function new_mesh() end

---姿かたちを描画するコンポーネントを付与。
---@param material Material
---@param mesh Mesh
---@return MeshComponent
function WorldObject:add_mesh_component(material, mesh) end
