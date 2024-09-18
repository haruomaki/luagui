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
