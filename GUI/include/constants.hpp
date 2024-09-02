#pragma once

constexpr float inch_meter = 0.0254f; // 1インチは2.54cm
// constexpr float px_meter = inch_meter / 157.35f; // 72dpiのピクセル幅
// constexpr float px_meter = 1; // 72dpiのピクセル幅
// px_meterを1にすると、float値1が1ピクセルというスケールになる
// TODO: HiDPI用のスケールに対応していない。システム設定が100%でも125%でも、各物体のピクセル上の大きさは変わらない。
