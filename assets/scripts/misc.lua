---@meta misc

---書式に沿って文字列を表示する。
---@param format string|number
---@param ... any
function printf(format, ...)
    print(string.format(format, ...))
end
