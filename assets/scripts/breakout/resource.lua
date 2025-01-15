-- ブロックにボールがぶつかったときの音
BlockSound = Music.load("audio/泡がはじける.mp3")

-- ブロックの画像
BlockImage = Image.load("images/ピンクレンガ.png")
-- ブロックのマテリアルとメッシュ
BlockMaterial = Material.from_image(BlockImage)
BlockMesh = Mesh.new(
    V3 { { -BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, -BlockHalfHeight, 0 }, { BlockHalfWidth, BlockHalfHeight, 0 }, { -BlockHalfWidth, BlockHalfHeight, 0 } },
    V2 { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })

-- ボールの画像
BallImage = Image.load("images/青いガラス玉.png")
-- ボールのマテリアルとメッシュ
BallMaterial = Material.from_image(BallImage)
BallMaterial.write_depth = false
BallMesh = Mesh.new(
    V3 { { -BallRadius, -BallRadius, 0 }, { BallRadius, -BallRadius, 0 }, { BallRadius, BallRadius, 0 }, { -BallRadius, BallRadius, 0 } },
    V2 { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })

-- ボールの画像
BarImage = Image.load("images/不思議なバー.png")
-- ボールのマテリアルとメッシュ
BarMaterial = Material.from_image(BarImage)
BarMesh = Mesh.new(
    V3 { { -BarHW, -BarHH, 0 }, { BarHW, -BarHH, 0 }, { BarHW, BarHH, 0 }, { -BarHW, BarHH, 0 } },
    V2 { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } })
