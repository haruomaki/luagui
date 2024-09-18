---@meta World

---@return World
function create_world() end

---@class World
---@field b2world b2World
local World = {}

---直線を描画する。
---@param points Points 頂点の座標
---@return MeshObject
function World:draw_line(points) end

---円を描画する。
---@param center table<number, number> 中心の座標
---@param radius number 半径
---@param segments? integer 分割数（デフォルト: 48）
---@return MeshObject
function World:draw_circle(center, radius, segments) end
