---@meta Text

---@class Font
local Font = {}

---@type Font
__CurrentFont = {}

---テキストを表示する
---@param text string 表示する文字列
---@param options? TextOptions 位置や色などのオプション
function WorldObject:draw_text(text, options) end

---draw_textに指定できるオプション集
---@class TextOptions
---@field position Point? 位置
local TextOptions = {}
