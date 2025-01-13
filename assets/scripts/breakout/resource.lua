-- ブロックにボールがぶつかったときの音
BlockSound = load_music("audio/泡がはじける.mp3")

-- ブロックの画像
BlockImage = load_image("images/ピンクレンガ.png")
-- ブロックのマテリアルとメッシュ
BlockMaterial = new_material(BlockImage)
BlockMesh = new_mesh(
    { { -BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, BlockHalfHeight, 0 }, { -BlockHalfWidth, BlockHalfHeight, 0 } },
    { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })

-- ボールの画像
BallImage = load_image("images/青いガラス玉.png")
-- ボールのマテリアルとメッシュ
BallMaterial = new_material(BallImage)
BallMaterial.write_depth = false
BallMesh = new_mesh(
    { { -BallRadius, -BallRadius, 0 }, { BallRadius, -BallRadius, 0 }, { BallRadius, BallRadius, 0 }, { -BallRadius, BallRadius, 0 } },
    { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })

-- ボールの画像
BarImage = load_image("images/不思議なバー.png")
-- ボールのマテリアルとメッシュ
BarMaterial = new_material(BarImage)
BarMesh = new_mesh(
    { { -BarHW, -BarHH, 0 }, { BarHW, -BarHH, 0 }, { BarHW, BarHH, 0 }, { -BarHW, BarHH, 0 } },
    { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })
