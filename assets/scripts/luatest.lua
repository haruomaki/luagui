-- Hello world を出力するだけ
print("Hello world from lua!")
print(twice(123))

local hi = function()
    io.write("はい")
end

for _ = 1, 5 do
    hi()
end
print()

-- local function receive_till(target)
--     while true do
--         local input = io.read():lower() -- コンソールから文字列を受け取る（小文字に変換）
--         if input == target then
--             return                      -- targetが入力されたらループを抜ける
--         end
--     end
-- end

local function game_logic()
    print("「りんご」と入力してね")
    local yield = function()
        coroutine.yield(332)
    end
    yield()

    -- coroutine.yield(332)
    -- coroutine.yield(receive_till("りんご"))
    print("オッケー！次は「みかん」と入力してね")
    coroutine.yield(999)
    -- coroutine.yield(receive_till("みかん"))
    print("クリア！")
end

-- コルーチンの実行
local co = coroutine.create(game_logic)
local ret = coroutine.resume(co)
print(ret)
coroutine.resume(co)
coroutine.resume(co)

-- Rustのenumに似た構造をLuaで表現
local Message = {
    Quit = function() return { type = "Quit" } end,
    Move = function(x, y) return { type = "Move", x = x, y = y } end,
    Write = function(s) return { type = "Write", s = s } end,
    ChangeColor = function(r, g, b) return { type = "ChangeColor", r = r, g = g, b = b } end,
}

-- 例えば、Moveのバリアントを作成
local move = Message.Move(10, 20)

-- バリアントのタイプと値をチェック
if move.type == "Move" then
    print("Move to position:", move.x, move.y)
end
