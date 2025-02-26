---@meta mesh

---描画設定。
---@class Material
---@field write_depth boolean デプスバッファに書き込むかどうか
---@field both boolean ポリゴンの裏面を描画するかどうか
---@field line_width number 線の太さ
---@field point_size number 点の大きさ
Material = {}

---@alias DrawMode
---| 'points'
---| 'lines'
---| 'line_strip'
---| 'line_loop'
---| 'triangles'
---| 'triangle_strip'
---| 'triangle_fan'
---| 'quads'
---| 'quad_strip'
---| 'polygon'

---物体形状。
---@class Mesh
---@field indices VI
---@field coords V3
---@field colors CV
---@field uvs V2
---@field use_index boolean
---@field draw_mode DrawMode
Mesh = {}

---@class MeshComponent : Component
MeshComponent = {}

---空のマテリアルを作成する。
---@return Material
function Material.new() end

---マテリアルを作成する。
---@param image Image テクスチャ画像
---@return Material
function Material.from_image(image) end

---3Dモデルファイルからマテリアルを読み込む。
---@param file_path string ファイルパス
---@return Material
function Material.load(file_path) end

---空のメッシュを作成する。
---@return Mesh
function Mesh.new() end

---メッシュを作成する。
---@return Mesh
---@param coords V3
---@param uvs V2
function Mesh.new(coords, uvs) end

---姿かたちを描画するコンポーネントを付与。
---@param material Material
---@param mesh Mesh
---@return MeshComponent
function WorldObject:add_mesh_component(material, mesh) end

---ファイルからメッシュを読み込む。
---@param file_path string ファイルパス
---@return Mesh
function Mesh.load(file_path) end
