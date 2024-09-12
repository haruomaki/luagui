-- local function title()
--     print("タイトルです")
--     coroutine.yield()
-- end

-- create_window(100, 100, "たいとるお", function()
--     poyo()
--     title()
--     while true do
--         coroutine.yield()
--     end
-- end)
function coro_func()
    for i = 1, 5 do
        print("Step: " .. i)
        coroutine.yield() -- コルーチンを一時停止
    end
end

create_window(800, 600, "Test Window", coro_func)
