#pragma once

#include <Property.hpp>
#include <graphical_base.hpp>

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

    inline void pad(const size_t size) {
        if (size > this->size()) {
            this->resize(size);
        }
    }

    [[nodiscard]] vector<glm::vec3> getCoords() const {
        vector<glm::vec3> coords;
        for (auto &&info : *this) {
            coords.push_back(info.coord);
        }
        return coords;
    }
    [[nodiscard]] vector<float> getXs() const {
        vector<float> xs;
        for (auto &&info : *this) {
            xs.push_back(info.coord.x);
        }
        return xs;
    }
    [[nodiscard]] vector<float> getYs() const {
        vector<float> ys;
        for (auto &&info : *this) {
            ys.push_back(info.coord.y);
        }
        return ys;
    }
    [[nodiscard]] vector<float> getZs() const {
        vector<float> zs;
        for (auto &&info : *this) {
            zs.push_back(info.coord.z);
        }
        return zs;
    }

    void setCoords(const vector<glm::vec3> &coords) {
        const auto size = coords.size();
        pad(size);
        for (size_t i = 0; i < size; i++) {
            this->at(i).coord = coords[i];
        }
    }
    void setXs(const vector<float> &xs) {
        const auto size = xs.size();
        pad(size);
        for (size_t i = 0; i < size; i++) {
            this->at(i).coord.x = xs[i];
        }
    }
    void setYs(const vector<float> &ys) {
        const auto size = ys.size();
        pad(size);
        for (size_t i = 0; i < size; i++) {
            this->at(i).coord.y = ys[i];
        }
    }
    void setZs(const vector<float> &zs) {
        const auto size = zs.size();
        pad(size);
        for (size_t i = 0; i < size; i++) {
            this->at(i).coord.z = zs[i];
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
    PropertyGetSet<&InterleavedVertexInfoVector::getXs, &InterleavedVertexInfoVector::setXs> xs{this};
    PropertyGetSet<&InterleavedVertexInfoVector::getYs, &InterleavedVertexInfoVector::setYs> ys{this};
    PropertyGetSet<&InterleavedVertexInfoVector::getZs, &InterleavedVertexInfoVector::setZs> zs{this};
    PropertyGetSet<&InterleavedVertexInfoVector::getColors, &InterleavedVertexInfoVector::setColors> colors{this};
};
