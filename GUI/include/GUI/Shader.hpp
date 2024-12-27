#pragma once

#include "Resource.hpp"
#include <SumiGL/Shader.hpp>

class Shader : public GL::ProgramObject, public Resource {
  public:
    Shader(GL::ProgramObject &&pg)
        : GL::ProgramObject(std::move(pg)) {}
};
