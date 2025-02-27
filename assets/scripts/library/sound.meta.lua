---@meta sound

---@class Music
Music = {}

---@class SoundSource : Component
---@field group_name string グループ名（読み取り専用）
SoundSource = {}

---音声データを読み込む。
---@param file_path string
---@return Music
function Music.load(file_path) end

---グループ内の最大音量を取得する。
---@param group_name string グループ名
---@return number volume 音量
function Music.set_group_volume(group_name) end

---グループ内の最大音量を設定する。
---@param group_name string グループ名
---@param volume number 音量
function Music.set_group_volume(group_name, volume) end

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
