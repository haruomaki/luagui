run_window(800, 600, "ブロック崩し", function()
    -- 背景色。
    Window.background_color = Hex("#403938")

    local breakout = require("main")
    breakout.main()
end)
