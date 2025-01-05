#pragma once

#include <graph/base/baseSlot.h>

class TextOutputSlot : public BaseSlot {
private:
public:
    explicit TextOutputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
