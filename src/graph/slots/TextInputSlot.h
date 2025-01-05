#pragma once

#include <graph/base/baseSlot.h>

class TextInputSlot : public BaseSlot {
    typedef BaseSlot Parent;

private:
public:
    explicit TextInputSlot(const BaseStyle& vParentStyle);
    bool init() override;
};
