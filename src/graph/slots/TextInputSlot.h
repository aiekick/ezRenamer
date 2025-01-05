#pragma once

#include <graph/base/baseSlot.h>

class TextInputSlot : public BaseSlot {
private:
public:
    explicit TextInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
