#pragma once

#include <GUI.hpp>
#include <Property.hpp>

struct InterleavedVertexInfo {
    glm::vec3 coord = {0, 0, 0};
    RGBA color = {0, 0, 0, 0};
};

struct InterleavedVertexInfoVector : public vector<InterleavedVertexInfo> {
    // コンストラクタのオーバーロード
    InterleavedVertexInfoVector() = default;

    // vector<T> を TVector に代入するための代入演算子のオーバーロード
    InterleavedVertexInfoVector &operator=(const vector<InterleavedVertexInfo> &other) {
        vector<InterleavedVertexInfo>::operator=(other);
        return *this;
    }

    [[nodiscard]] vector<glm::vec3> getCoords() const {
        vector<glm::vec3> coords;
        for (auto &&info : *this) {
            coords.push_back(info.coord);
        }
        return coords;
    }

    void setCoords(const vector<glm::vec3> &coords) {
        const auto size = coords.size();
        if (size > this->size()) {
            this->resize(size);
        }
        for (size_t i = 0; i < size; i++) {
            this->at(i).coord = coords[i];
        }
    }
    [[nodiscard]] vector<RGBA> getColors() const {
        vector<RGBA> colors;
        for (auto &&info : *this) {
            colors.push_back(info.color);
        }
        return colors;
    }

    void setColors(const vector<RGBA> &colors) {
        const auto size = colors.size();
        if (size > this->size()) {
            this->resize(size);
        }
        for (size_t i = 0; i < size; i++) {
            this->at(i).color = colors[i];
        }
    }

    PropertyGetSet<&InterleavedVertexInfoVector::getCoords, &InterleavedVertexInfoVector::setCoords> coords{this};
    PropertyGetSet<&InterleavedVertexInfoVector::getColors, &InterleavedVertexInfoVector::setColors> colors{this};
};
