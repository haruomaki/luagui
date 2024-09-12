-- coroutines.lua

---指定の秒数待機する
---@param seconds integer 待機する秒数
function Wait(seconds)
    local start = os.time()
    while os.time() <= start + seconds do
        coroutine.yield()
    end
end

function WaitUntil(condition)
    while not condition() do
        coroutine.yield()
    end
end
