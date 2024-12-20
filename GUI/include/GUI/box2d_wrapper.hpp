#pragma once

#include <box2cpp/box2cpp.h>
#include <variant>

struct b2Chain_dummy {}; // NOLINT(readability-identifier-naming)

// Box2DのShapeたちをまとめるタグ付きユニオン
using ShapeVariant = std::variant<b2Circle, b2Capsule, b2Segment, b2Polygon, b2Chain_dummy>;

struct ShapeIdVariant {
    std::variant<b2ShapeId, b2ChainId> elem;

    ShapeIdVariant(b2ShapeId id)
        : elem(id) {}
    ShapeIdVariant(b2ChainId id)
        : elem(id) {}

    int index1() {
        return std::visit(
            [&](auto &s) {
                return ShapeIdVariant(s.Handle());
            },
            elem);
    }
};

struct ShapeRefVariant {
    std::variant<b2::ShapeRef, b2::ChainRef> elem;

    ShapeIdVariant Handle() {
        return std::visit(
            [&](auto &s) {
                return ShapeIdVariant(s.Handle());
            },
            elem);
    }
};

using ParamsVariant = std::variant<b2::Shape::Params, b2::Chain::Params>;
