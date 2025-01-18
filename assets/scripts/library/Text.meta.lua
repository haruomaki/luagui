---@meta Text

---@class Font
local Font = {}

---@type Font
__CurrentFont = {}

---テキストコンポーネント。
---@class Text : Component
---@field message string 表示する文字列。
local Text = {}

---テキストを表示する
---@param text? string 表示する文字列
---@param options? TextOptions 位置や色などのオプション
---@return Text
function WorldObject:child_text(text, options) end

---draw_textに指定できるオプション集
---@class TextOptions
---@field position Point? 位置
---@field scale number? 拡大率
local TextOptions = {}
