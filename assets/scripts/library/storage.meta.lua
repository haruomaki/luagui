---@meta storage

---ストレージの操作を担う静的クラス。
---@class Storage
Storage = nil

---指定のパス直下にあるファイルの一覧を取得する。
---@param path string
---@return table<integer, string>
function Storage.ls(path) end
