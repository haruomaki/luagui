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
---| 'D'
---| 'E'
---| 'F'
---| 'G'
---| 'H'
---| 'I'
---| 'J'
---| 'K'
---| 'L'
---| 'M'
---| 'N'
---| 'O'
---| 'P'
---| 'Q'
---| 'R'
---| 'S'
---| 'T'
---| 'U'
---| 'V'
---| 'W'
---| 'X'
---| 'Y'
---| 'Z'
---| 'Right'
---| 'Left'
---| 'Down'
---| 'Up'
---| 'Enter'
---| 'Escape'
---| 'LeftShift'
---| 'LeftControl'
---| 'LeftAlt'
---| 'LeftSuper'
---| 'RightShift'
---| 'RightControl'
---| 'RightAlt'
---| 'RightSuper'
---| 'Menu'

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

---ウィンドウを閉じるよう要求する。
function CloseWindow() end

---スクリーンに関するプロパティをまとめた静的クラス
---@class Screen
---@field refreshRate integer 《読み取り専用》ディスプレイのリフレッシュレート
Screen = {}

---マウスカーソルに関するプロパティをまとめた静的クラス
---@class Mouse
---@field pos vec2 マウスカーソルの位置
---@field diff vec2 マウスカーソルの移動速度
Mouse = {}

---マウスカーソルを無効化する。
---@param flag boolean? trueだとカーソルが消え、仮想的に無制限移動できるようになる
function Mouse.disable(flag) end
