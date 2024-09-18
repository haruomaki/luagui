---@meta Window

---一つのウィンドウを表すクラス
---@class Window
local Window = {}

---@type Window
__CurrentWindow = {}

---@alias Keys
---| 'Space'
---| 'A'
---| 'B'
---| 'C'
---| 'Right'
---| 'Left'
---| 'Down'
---| 'Up'
---| 'Enter'
---| 'Escape'

--- キー入力をチェックする関数
---@param key Keys キーの名前
---@return boolean pressed 押されていればtrue
function GetKey(key) end

--- キーを押したイベントをチェックする関数
---@param key Keys キーの名前
---@return boolean pressed 今フレームに初めて押されればtrue
function GetKeyDown(key) end

--- キーを離したイベントをチェックする関数
---@param key Keys キーの名前
---@return boolean released 今フレームに初めて離されればtrue
function GetKeyUp(key) end
