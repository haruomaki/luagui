---@meta sound

---@class Music
Music = {}

---@class SoundSource : Component
SoundSource = {}

---音声データを読み込む。
---@param file_path string
---@return Music
function Music.load(file_path) end

---音源を表すコンポーネントを追加する。
---@param group_name string グループ名
---@param music Music 再生する音声
---@return SoundSource
function WorldObject:add_soundsource_component(group_name, music) end

---音声を再生する。
function SoundSource:play() end

---音声が再生中かどうか調べる。
---@return boolean
function SoundSource:is_playing() end
