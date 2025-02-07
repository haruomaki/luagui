---@meta Text

---ロード済みのフォントを表すクラス。
---@class Font
Font = {}

---@type Font
__CurrentFont = nil

---ストレージからフォントを読み込み、カレントフォントを変更する。
---@param file_path string
---@return Font
function Font.load(file_path) end

---テキストコンポーネント。
---@class Text : Component
---@field message string 表示する文字列。
---@field font_size number フォントの大きさ。
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
---@field font_size number? フォントの大きさ
local TextOptions = {}
