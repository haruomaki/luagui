---@meta sound

---@class Music
local Music = {}

---@class SoundSource : Component
local SoundSource = {}

---音声データを読み込む。
---@param file_path string
---@return Music
function load_music(file_path) end

---音源を表すコンポーネントを追加する。
---@param music Music
---@return SoundSource
function WorldObject:add_soundsource_component(music) end

---音声を再生する。
function SoundSource:play() end

---音声が再生中かどうか調べる。
---@return boolean
function SoundSource:is_playing() end
