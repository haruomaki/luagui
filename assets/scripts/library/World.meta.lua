---@meta World

---@return World
function create_world() end

---@class World
---@field b2world b2World
local World = {}

---@type World
__CurrentWorld = nil

---直線を描画する。
---@param points Points 頂点の座標
---@return MeshObject
function World:draw_line(points) end

---矩形を描画する。
---@param hx number 横幅の半分
---@param hy number 縦幅の半分
---@return MeshObject
function World:draw_rect(hx, hy) end

---円を描画する。
---@param center table<number, number> 中心の座標
---@param radius number 半径
---@param segments? integer 分割数（デフォルト: 48）
---@return MeshObject
function World:draw_circle(center, radius, segments) end
