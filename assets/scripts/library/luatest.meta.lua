---@meta

---@param x integer 引数の数
---@return integer 引数の2倍の値
function twice(x) end

---@param width integer ウィンドウの幅
---@param height integer ウィンドウの高さ
---@param title string ウィンドウタイトル
---@param scenario function ゲームロジックなどのコルーチン
function create_window(width, height, title, scenario) end

---@return World
function create_world() end

---@class World
local World = {}

---直線を描画する。
---@param points table<number, table<number, number>> 頂点の座標
function World:draw_line(points) end

---円を描画する。
---@param center table<number, number> 中心の座標
---@param radius number 半径
---@param segments? integer 分割数（デフォルト: 48）
function World:draw_circle(center, radius, segments) end
