#pragma once

#include <graph/base/baseSlot.h>

class StringInputSlot : public BaseSlot {
private:
public:
    explicit StringInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
