---@meta World

---@return World
function create_world() end

---@class World
---@field b2world b2World
local World = {}

-- TODO: 別ファイルに移動
---@class b2World
---@field gravity b2Vec2 重力加速度のベクトル
local b2World = {}

---@class b2Vec2
local b2Vec2 = {}

---直線を描画する。
---@param points table<number, table<number, number>> 頂点の座標
---@return MeshObject
function World:draw_line(points) end

---円を描画する。
---@param center table<number, number> 中心の座標
---@param radius number 半径
---@param segments? integer 分割数（デフォルト: 48）
---@return MeshObject
function World:draw_circle(center, radius, segments) end
