---@meta utils

---@diagnostic disable-next-line: duplicate-doc-alias
---@alias Point table<number, number>
---@alias Points table<number, Point>

-- HACK: プロパティとして使うためにPointを二重定義
---@class Point
---@field x number
---@field y number
local Point = {}

---今実行中のスクリプトのパス（assetsディレクトリからの相対パス）が格納されている。
---@type string
__CWD__ = ""
